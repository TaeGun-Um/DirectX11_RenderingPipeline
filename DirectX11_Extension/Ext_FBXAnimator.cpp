#include "PrecompileHeader.h"
#include "Ext_FBXAnimator.h"
#include <cstdio>  // printf
#include <DirectXMath.h>
using namespace DirectX;

//─────────────────────────────────────────────────────────────────────────
// 생성자/소멸자
//─────────────────────────────────────────────────────────────────────────
Ext_FBXAnimator::Ext_FBXAnimator()
{
}

Ext_FBXAnimator::~Ext_FBXAnimator()
{
    // (Assimp::Importer 내부 메모리는 자동 해제)
}

//─────────────────────────────────────────────────────────────────────────
// [1] T-Pose FBX(메시 + 바인드 포즈 스켈레톤) 로드
//─────────────────────────────────────────────────────────────────────────
bool Ext_FBXAnimator::LoadMeshFBX(
    const std::string& _TposeFilename,
    std::vector<Ext_DirectXVertexData>& _OutVertices,
    std::vector<unsigned int>& _OutIndices)
{
    // 1) Assimp로 T-Pose FBX 읽기
    MeshScene = MeshImporter.ReadFile(
        _TposeFilename,
        aiProcess_Triangulate
        | aiProcess_ConvertToLeftHanded
        | aiProcess_GenSmoothNormals
        | aiProcess_CalcTangentSpace
        | aiProcess_JoinIdenticalVertices
        | aiProcess_LimitBoneWeights
    );

    if (!MeshScene
        || (MeshScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        || !MeshScene->mRootNode)
    {
        MsgAssert("FBX메시 로드 실패");
        return false;
    }

    // 2) GlobalInverseTransform 계산 (루트 노드의 변환의 역행렬)
    GlobalInverseTransform = MeshScene->mRootNode->mTransformation;
    GlobalInverseTransform.Inverse();

    // 디버그: GlobalInverseTransform 출력
    PrintAiMatrix(GlobalInverseTransform, "GlobalInverseTransform (Inverse of Root)");

    // 3) 본 정보만 추출 (BoneNameToInfo, BoneCount 세팅)
    ExtractBonesFromMesh();

    // 4) FinalBoneMatrices 초기화: BoneCount 개수만큼 단위행렬로 시작
    FinalBoneMatrices.clear();
    FinalBoneMatrices.resize(BoneCount, aiMatrix4x4()); // aiMatrix4x4 기본 생성자는 단위행렬입니다.

    // 5) aiMesh → Ext_DirectXVertexData, _OutIndices 벡터 채우기
    if (MeshScene->mNumMeshes < 1)
    {
        MsgAssert("FBX메시 로드 실패");
        return false;
    }

    // 여기서는 편의상 첫 번째 메시만 사용
    const aiMesh* aiMeshPtr = MeshScene->mMeshes[0];

    // 5-1) 버텍스/인덱스 벡터 초기화
    _OutVertices.clear();
    _OutIndices.clear();
    _OutVertices.reserve(aiMeshPtr->mNumVertices);
    _OutIndices.reserve(aiMeshPtr->mNumFaces * 3);

    // 5-2) 정점 정보 채우기: Position, Normal, TexCoord, Color, BONEID/WEIGHT는 나중에 채움
    for (unsigned int v = 0; v < aiMeshPtr->mNumVertices; ++v)
    {
        Ext_DirectXVertexData vData;

        // (A) 위치
        {
            const aiVector3D& aiPos = aiMeshPtr->mVertices[v];
            vData.POSITION = float4(aiPos.x, aiPos.y, aiPos.z, 1.0f);
        }

        // (B) 법선
        if (aiMeshPtr->HasNormals())
        {
            const aiVector3D& aiNorm = aiMeshPtr->mNormals[v];
            vData.NORMAL = float4(aiNorm.x, aiNorm.y, aiNorm.z, 0.0f);
        }
        else
        {
            vData.NORMAL = float4(0.0f, 1.0f, 0.0f, 0.0f);
        }

        // (C) 텍스처 좌표 (0번 채널만 사용)
        if (aiMeshPtr->HasTextureCoords(0))
        {
            const aiVector3D& aiUV = aiMeshPtr->mTextureCoords[0][v];
            vData.TEXCOORD = float4(aiUV.x, aiUV.y, 0.0f, 0.0f);
        }
        else
        {
            vData.TEXCOORD = float4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        // (D) 컬러: 흰색 (기본값)
        vData.COLOR = float4(1.0f, 1.0f, 1.0f, 1.0f);

        // (E) BONEID, WEIGHT는 Ext_DirectXVertexData 생성자에서 0으로 초기화되어 있다고 가정
        //     →(별도 처리 없음)

        _OutVertices.push_back(vData);
    }

    // 5-3) aiBone → 정점별 BoneID, Weight 채우기 (최대 4개 본만 사용)
    for (unsigned int b = 0; b < aiMeshPtr->mNumBones; ++b)
    {
        const aiBone* aiBonePtr = aiMeshPtr->mBones[b];
        std::string boneName = aiBonePtr->mName.C_Str();

        // BoneNameToInfo에서 ID 가져오기
        auto boneIt = BoneNameToInfo.find(boneName);
        if (boneIt == BoneNameToInfo.end())
            continue;

        int boneID = boneIt->second.ID;

        // 이 aiBone이 영향 주는 모든 (VertexID, Weight) 만큼 반복
        for (unsigned int w = 0; w < aiBonePtr->mNumWeights; ++w)
        {
            unsigned int vertID = aiBonePtr->mWeights[w].mVertexId;
            float         weight = aiBonePtr->mWeights[w].mWeight;

            Ext_DirectXVertexData& destV = _OutVertices[vertID];

            // 네 슬롯(0~3) 중 빈 슬롯을 찾아 boneID, weight 설정
            for (int k = 0; k < 4; ++k)
            {
                bool isEmptySlot = false;
                switch (k)
                {
                case 0: isEmptySlot = (destV.WEIGHT.x == 0.0f); break;
                case 1: isEmptySlot = (destV.WEIGHT.y == 0.0f); break;
                case 2: isEmptySlot = (destV.WEIGHT.z == 0.0f); break;
                case 3: isEmptySlot = (destV.WEIGHT.w == 0.0f); break;
                }
                if (isEmptySlot)
                {
                    switch (k)
                    {
                    case 0:
                        destV.BONEID.x = static_cast<unsigned int>(boneID);
                        destV.WEIGHT.x = weight;
                        break;
                    case 1:
                        destV.BONEID.y = static_cast<unsigned int>(boneID);
                        destV.WEIGHT.y = weight;
                        break;
                    case 2:
                        destV.BONEID.z = static_cast<unsigned int>(boneID);
                        destV.WEIGHT.z = weight;
                        break;
                    case 3:
                        destV.BONEID.w = static_cast<unsigned int>(boneID);
                        destV.WEIGHT.w = weight;
                        break;
                    }
                    break;
                }
            }
        }
    }

    // 5-4) 인덱스 채우기 (각 Face마다 반드시 3개의 인덱스)
    for (unsigned int f = 0; f < aiMeshPtr->mNumFaces; ++f)
    {
        const aiFace& face = aiMeshPtr->mFaces[f];
        _OutIndices.push_back(face.mIndices[0]);
        _OutIndices.push_back(face.mIndices[2]);
        _OutIndices.push_back(face.mIndices[1]);
    }

    return true;
}

//─────────────────────────────────────────────────────────────────────────
// [2] 애니메이션 데이터만 담긴 FBX 로드
//─────────────────────────────────────────────────────────────────────────
bool Ext_FBXAnimator::LoadAnimationFBX(const std::string& _AnimFilename)
{
    // 1) AnimScene에 애니메이션 FBX 읽기
    AnimScene = AnimImporter.ReadFile(
        _AnimFilename,
        aiProcess_Triangulate
        | aiProcess_ConvertToLeftHanded
        | aiProcess_GenSmoothNormals    // 메시가 없어도 무시되지만, 안전하게 두어도 됨
        | aiProcess_CalcTangentSpace
        | aiProcess_JoinIdenticalVertices
        | aiProcess_LimitBoneWeights
    );

    if (!AnimScene
        || (AnimScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        || !AnimScene->mRootNode)
    {
        MsgAssert("FBX애니메이션 로드 실패");
        return false;
    }

    // 2) 애니메이션이 하나라도 들어 있는지 확인
    if (AnimScene->mNumAnimations == 0)
    {
        MsgAssert("FBX 안에 애니메이션 없음");
        return false;
    }

    // 일단 첫 번째 애니메이션을 기본으로 선택
    CurrentAnimIndex = 0;
    CurrentAnimation = AnimScene->mAnimations[0];

    // 3) BoneNameToAnimChannel 맵핑 구성: 
    //    (본 이름 → aiNodeAnim* 채널) 
    BoneNameToAnimChannel.clear();
    for (unsigned int a = 0; a < AnimScene->mNumAnimations; ++a)
    {
        const aiAnimation* anim = AnimScene->mAnimations[a];
        printf(">>> Anim[%u] Name=\"%s\", Channels=%u\n",
            a,
            anim->mName.C_Str(),
            anim->mNumChannels);

        // 각 채널마다 nodeName(=본 이름), 그 키프레임 배열 정보를 저장
        for (unsigned int c = 0; c < anim->mNumChannels; ++c)
        {
            const aiNodeAnim* channel = anim->mChannels[c];
            std::string boneName = channel->mNodeName.C_Str();
            BoneNameToAnimChannel[boneName] = channel;
            printf("    Channel[%u]: boneName=\"%s\"\n", c, boneName.c_str());
        }
    }

    return true;
}

//─────────────────────────────────────────────────────────────────────────
// [3] 애니메이션 인덱스 변경
//─────────────────────────────────────────────────────────────────────────
bool Ext_FBXAnimator::SetAnimation(unsigned int _AnimIndex)
{
    if (!AnimScene)
        return false;

    if (_AnimIndex >= AnimScene->mNumAnimations)
        return false;

    CurrentAnimIndex = _AnimIndex;
    CurrentAnimation = AnimScene->mAnimations[CurrentAnimIndex];

    BoneNameToAnimChannel.clear();
    for (unsigned int c = 0; c < CurrentAnimation->mNumChannels; ++c)
    {
        const aiNodeAnim* channel = CurrentAnimation->mChannels[c];
        std::string boneName = channel->mNodeName.C_Str();
        BoneNameToAnimChannel[boneName] = channel;
    }
    return true;
}

//─────────────────────────────────────────────────────────────────────────
// [4] 프레임마다 호출: 본별 최종 스킨 행렬 계산
//─────────────────────────────────────────────────────────────────────────
void Ext_FBXAnimator::UpdateAnimation(float _TimeInSeconds)
{
    if (!MeshScene || !AnimScene || !CurrentAnimation)
        return;

    // (A) FinalBoneMatrices 초기화 (BoneCount만큼)
    for (unsigned int i = 0; i < BoneCount; ++i)
    {
        FinalBoneMatrices[i] = aiMatrix4x4(); // 단위행렬
    }

    // 디버그: 호출 시점 시간 출력
    printf("=== UpdateAnimation: Time(%.4f sec) ===\n", _TimeInSeconds);

    // (B) MeshScene의 루트 노드를 재귀 순회하면서 본별 행렬 계산
    aiMatrix4x4 identity; // 단위행렬
    ReadNodeHierarchy(_TimeInSeconds, MeshScene->mRootNode, identity);

    // 디버그: 계산된 본 개수 확인
    printf("[UpdateAnimation] FinalBoneMatrices size = %zu\n", FinalBoneMatrices.size());
}

//─────────────────────────────────────────────────────────────────────────
// [5] CB_SkinnedMatrix 형태로 FinalBoneMatrices 복사 (쉐이더용)
//─────────────────────────────────────────────────────────────────────────
CB_SkinnedMatrix Ext_FBXAnimator::RenderSkinnedMesh()
{
    CB_SkinnedMatrix cb = {};

    // 최종 BoneMatrices를 DirectX::XMMATRIX 배열로 복사
    for (size_t i = 0; i < FinalBoneMatrices.size() && i < MAX_BONES; ++i)
    {
        // aiMatrix4x4 → DirectX::XMMATRIX 변환
        const aiMatrix4x4& m = FinalBoneMatrices[i];
        XMMATRIX xm = XMMATRIX(
            m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4
        );
        cb.Bones[i] = xm;

        // 첫 몇 개만 디버그 출력 (예: i < 3)
        if (i < 3)
        {
            XMFLOAT4X4 f;
            XMStoreFloat4x4(&f, xm);
            printf("[RenderSkinnedMesh] Bone[%zu] XMMatrix:\n", i);
            printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._11, f._12, f._13, f._14);
            printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._21, f._22, f._23, f._24);
            printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._31, f._32, f._33, f._34);
            printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._41, f._42, f._43, f._44);
        }
    }

    // 나머지는 단위행렬로 채움
    for (size_t i = FinalBoneMatrices.size(); i < MAX_BONES; ++i)
    {
        cb.Bones[i] = XMMatrixIdentity();
    }

    return cb;
}

//─────────────────────────────────────────────────────────────────────────
// [D] 본 정보 추출: MeshScene에 들어 있는 모든 aiBone 이름을 모아서 Unique하게 정리
//─────────────────────────────────────────────────────────────────────────
void Ext_FBXAnimator::ExtractBonesFromMesh()
{
    BoneCount = 0;
    BoneNameToInfo.clear();

    // ① 모든 메시의 aiBone 이름을 모으기
    std::set<std::string> allBoneNames;
    for (unsigned int m = 0; m < MeshScene->mNumMeshes; ++m)
    {
        const aiMesh* mesh = MeshScene->mMeshes[m];
        if (!mesh->HasBones())
            continue;

        for (unsigned int b = 0; b < mesh->mNumBones; ++b)
        {
            allBoneNames.insert(std::string(mesh->mBones[b]->mName.C_Str()));
        }
    }

    // ② 중복 없는 Bone 이름 각각에 대해 첫 번째 aiBone의 OffsetMatrix를 찾아 저장
    BoneCount = 0;
    for (auto& boneName : allBoneNames)
    {
        aiMatrix4x4 foundOffset;
        bool found = false;

        // 아까 모아둔 이름이 나오면 해당 aiBone의 OffsetMatrix를 가져옴
        for (unsigned int m = 0; m < MeshScene->mNumMeshes && !found; ++m)
        {
            const aiMesh* mesh = MeshScene->mMeshes[m];
            if (!mesh->HasBones())
                continue;

            for (unsigned int b = 0; b < mesh->mNumBones; ++b)
            {
                const aiBone* bone = mesh->mBones[b];
                if (boneName == bone->mName.C_Str())
                {
                    foundOffset = bone->mOffsetMatrix;
                    found = true;
                    break;
                }
            }
        }

        BoneInfo bi;
        bi.ID = BoneCount;
        bi.OffsetMatrix = foundOffset;
        BoneNameToInfo[boneName] = bi;

        // 디버그: 본 이름, ID, OffsetMatrix 출력
        printf("Registered Bone: \"%s\" -> ID=%d\n", boneName.c_str(), BoneCount);
        PrintAiMatrix(foundOffset, ("OffsetMatrix_" + boneName).c_str());

        ++BoneCount;
    }

    printf("Total unique bones found: %u\n", BoneCount);
}

//─────────────────────────────────────────────────────────────────────────
// [E] 재귀적으로 노드 트리 순회하며 바인드포즈 or 애니메이션 TRS를 적용해서
//     최종 본 행렬(FinalBoneMatrices)에 넣어 주는 함수
//─────────────────────────────────────────────────────────────────────────
aiMatrix4x4 Ext_FBXAnimator::ReadNodeHierarchy(
    float _TimeInSeconds,
    const aiNode* _Node,
    const aiMatrix4x4& _ParentTransform)
{
    std::string nodeName = _Node->mName.C_Str();

    // (1) 바인드-포즈 트랜스폼
    aiMatrix4x4 nodeTransform = _Node->mTransformation;

    // (2) 애니메이션 채널이 있으면, 바인드-포즈를 무시하고 TRS를 보간해서 사용
    auto animIt = BoneNameToAnimChannel.find(nodeName);
    if (animIt != BoneNameToAnimChannel.end())
    {
        const aiNodeAnim* channel = animIt->second;
        printf("[ReadNodeHierarchy] Bone \"%s\" has anim channel → Interpolate TRS\n", nodeName.c_str());

        // (2-1) 시간(초)를 틱 단위로 바꾼 뒤 전체 길이를 넘어가면 래핑
        float ticks = TimeInTicks(_TimeInSeconds);
        float duration = static_cast<float>(CurrentAnimation->mDuration);
        float animTime = fmod(ticks, duration);

        // (2-2) 보간 처리
        aiVector3D interpPosition;
        aiQuaternion  interpRotation;
        aiVector3D interpScaling;

        CalcInterpolatedPosition(interpPosition, animTime, channel);
        CalcInterpolatedRotation(interpRotation, animTime, channel);
        CalcInterpolatedScaling(interpScaling, animTime, channel);

        // (2-3) T, R, S 행렬 생성
        aiMatrix4x4 T, R, S;
        aiMatrix4x4::Translation(interpPosition, T);
        R = aiMatrix4x4(interpRotation.GetMatrix());
        aiMatrix4x4::Scaling(interpScaling, S);

        nodeTransform = T * R * S;
    }
    else
    {
        // 애니메이션 채널이 없으면 바인드포즈 유지
        printf("[ReadNodeHierarchy] Node \"%s\" no anim channel → Use bind-pose\n", nodeName.c_str());
    }

    // (3) 부모 행렬과 곱해 글로벌 행렬 계산
    aiMatrix4x4 globalTransform = _ParentTransform * nodeTransform;

    // (4) 이 노드가 실제 본인지 확인 (BoneNameToInfo에 존재하는지)
    auto boneIt = BoneNameToInfo.find(nodeName);
    if (boneIt != BoneNameToInfo.end())
    {
        int boneID = boneIt->second.ID;
        const aiMatrix4x4& offset = boneIt->second.OffsetMatrix;

        // 최종 스킨 행렬 = GlobalInverseTransform × globalTransform × offset
        aiMatrix4x4 finalBone = GlobalInverseTransform * globalTransform * offset;
        FinalBoneMatrices[boneID] = finalBone;

        // 디버그: 최종 본 행렬 출력
        printf("[ReadNodeHierarchy] FinalBoneMatrices[%d] for \"%s\":\n", boneID, nodeName.c_str());
        PrintAiMatrix(finalBone, ("FinalBone_" + nodeName).c_str());
    }

    // (5) 자식 노드 순회
    for (unsigned int i = 0; i < _Node->mNumChildren; ++i)
    {
        ReadNodeHierarchy(_TimeInSeconds, _Node->mChildren[i], globalTransform);
    }

    return globalTransform;
}

//─────────────────────────────────────────────────────────────────────────
// [F] 애니메이션 키프레임 보간 함수들
//─────────────────────────────────────────────────────────────────────────
unsigned int Ext_FBXAnimator::FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    for (unsigned int i = 0; i + 1 < _NodeAnim->mNumPositionKeys; ++i)
    {
        if (_AnimTime < static_cast<float>(_NodeAnim->mPositionKeys[i + 1].mTime))
            return i;
    }
    return _NodeAnim->mNumPositionKeys - 1;
}

unsigned int Ext_FBXAnimator::FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    for (unsigned int i = 0; i + 1 < _NodeAnim->mNumRotationKeys; ++i)
    {
        if (_AnimTime < static_cast<float>(_NodeAnim->mRotationKeys[i + 1].mTime))
            return i;
    }
    return _NodeAnim->mNumRotationKeys - 1;
}

unsigned int Ext_FBXAnimator::FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    for (unsigned int i = 0; i + 1 < _NodeAnim->mNumScalingKeys; ++i)
    {
        if (_AnimTime < static_cast<float>(_NodeAnim->mScalingKeys[i + 1].mTime))
            return i;
    }
    return _NodeAnim->mNumScalingKeys - 1;
}

void Ext_FBXAnimator::CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    if (_NodeAnim->mNumPositionKeys == 1)
    {
        _Out = _NodeAnim->mPositionKeys[0].mValue;
        printf("  [CalcPosition] Single key: (%.4f, %.4f, %.4f)\n",
            _Out.x, _Out.y, _Out.z);
        return;
    }

    unsigned int index = FindPositionIndex(_AnimTime, _NodeAnim);
    unsigned int nextIndex = index + 1;
    if (nextIndex >= _NodeAnim->mNumPositionKeys)
    {
        _Out = _NodeAnim->mPositionKeys[index].mValue;
        printf("  [CalcPosition] Last key: index=%u, pos=(%.4f, %.4f, %.4f)\n",
            index, _Out.x, _Out.y, _Out.z);
        return;
    }

    float time0 = static_cast<float>(_NodeAnim->mPositionKeys[index].mTime);
    float time1 = static_cast<float>(_NodeAnim->mPositionKeys[nextIndex].mTime);
    float factor = (_AnimTime - time0) / (time1 - time0);

    const aiVector3D& start = _NodeAnim->mPositionKeys[index].mValue;
    const aiVector3D& end = _NodeAnim->mPositionKeys[nextIndex].mValue;

    printf("  [CalcPosition] idx=%u,next=%u,time0=%.4f,time1=%.4f,fact=%.4f\n",
        index, nextIndex, time0, time1, factor);

    _Out = start + (end - start) * factor;
}

void Ext_FBXAnimator::CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    if (_NodeAnim->mNumRotationKeys == 1)
    {
        _Out = _NodeAnim->mRotationKeys[0].mValue;
        printf("  [CalcRotation] Single key quat=(%.4f,%.4f,%.4f,%.4f)\n",
            _Out.x, _Out.y, _Out.z, _Out.w);
        return;
    }

    unsigned int index = FindRotationIndex(_AnimTime, _NodeAnim);
    unsigned int nextIndex = index + 1;
    if (nextIndex >= _NodeAnim->mNumRotationKeys)
    {
        _Out = _NodeAnim->mRotationKeys[index].mValue;
        return;
    }

    float time0 = static_cast<float>(_NodeAnim->mRotationKeys[index].mTime);
    float time1 = static_cast<float>(_NodeAnim->mRotationKeys[nextIndex].mTime);
    float factor = (_AnimTime - time0) / (time1 - time0);

    const aiQuaternion& startQ = _NodeAnim->mRotationKeys[index].mValue;
    const aiQuaternion& endQ = _NodeAnim->mRotationKeys[nextIndex].mValue;

    printf("  [CalcRotation] idx=%u,next=%u,time0=%.4f,time1=%.4f,fact=%.4f\n",
        index, nextIndex, time0, time1, factor);

    aiQuaternion::Interpolate(_Out, startQ, endQ, factor);
    _Out = _Out.Normalize();
}

void Ext_FBXAnimator::CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    if (_NodeAnim->mNumScalingKeys == 1)
    {
        _Out = _NodeAnim->mScalingKeys[0].mValue;
        printf("  [CalcScaling] Single key: (%.4f, %.4f, %.4f)\n",
            _Out.x, _Out.y, _Out.z);
        return;
    }

    unsigned int index = FindScalingIndex(_AnimTime, _NodeAnim);
    unsigned int nextIndex = index + 1;
    if (nextIndex >= _NodeAnim->mNumScalingKeys)
    {
        _Out = _NodeAnim->mScalingKeys[index].mValue;
        printf("  [CalcScaling] Last key: idx=%u, scale=(%.4f,%.4f,%.4f)\n",
            index, _Out.x, _Out.y, _Out.z);
        return;
    }

    float time0 = static_cast<float>(_NodeAnim->mScalingKeys[index].mTime);
    float time1 = static_cast<float>(_NodeAnim->mScalingKeys[nextIndex].mTime);
    float factor = (_AnimTime - time0) / (time1 - time0);

    const aiVector3D& startS = _NodeAnim->mScalingKeys[index].mValue;
    const aiVector3D& endS = _NodeAnim->mScalingKeys[nextIndex].mValue;

    printf("  [CalcScaling] idx=%u,next=%u,time0=%.4f,time1=%.4f,fact=%.4f\n",
        index, nextIndex, time0, time1, factor);

    _Out = startS + (endS - startS) * factor;
}

//─────────────────────────────────────────────────────────────────────────
// [G] 초 단위 → Assimp 틱 단위 변환
//─────────────────────────────────────────────────────────────────────────
float Ext_FBXAnimator::TimeInTicks(float _TimeInSeconds) const
{
    if (CurrentAnimation && CurrentAnimation->mTicksPerSecond != 0.0f)
    {
        return _TimeInSeconds * static_cast<float>(CurrentAnimation->mTicksPerSecond);
    }
    return _TimeInSeconds * 25.0f; // 기본 25fps 가정
}

//─────────────────────────────────────────────────────────────────────────
// [H] aiMatrix4x4 디버깅용 출력
//─────────────────────────────────────────────────────────────────────────
void Ext_FBXAnimator::PrintAiMatrix(const aiMatrix4x4& m, const char* name)
{
    if (name && name[0] != '\0')
        printf("[%s]\n", name);

    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.a1, m.a2, m.a3, m.a4);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.b1, m.b2, m.b3, m.b4);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.c1, m.c2, m.c3, m.c4);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.d1, m.d2, m.d3, m.d4);
}

void Ext_FBXAnimator::PrintXMMATRIX(const DirectX::XMMATRIX& xm, const char* name)
{
    if (name && name[0] != '\0')
        printf("[%s]\n", name);

    XMFLOAT4X4 f;
    XMStoreFloat4x4(&f, xm);

    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._11, f._12, f._13, f._14);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._21, f._22, f._23, f._24);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._31, f._32, f._33, f._34);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", f._41, f._42, f._43, f._44);
}