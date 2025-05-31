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
    //──────────────────────────────────────────────────────────────
    // 1) Assimp로 T-Pose FBX 읽기 (좌표계 변환 플래그 aiProcess_ConvertToLeftHanded 제거)
    //──────────────────────────────────────────────────────────────
    MeshScene = MeshImporter.ReadFile(_TposeFilename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights);

    if (!MeshScene || (MeshScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !MeshScene->mRootNode)
    {
        MsgAssert("FBX 메시 로드 실패");
        return false;
    }

    //──────────────────────────────────────────────────────────────
    // 2) GlobalInverseTransform 계산 (루트 노드 변환의 역행렬)
    //──────────────────────────────────────────────────────────────
    {
        aiMatrix4x4 inv = MeshScene->mRootNode->mTransformation;
        inv.Inverse();
        GlobalInverseTransform = inv;
    }

    //──────────────────────────────────────────────────────────────
    // 3) 본 정보만 추출 (BoneNameToInfo, BoneCount 세팅)
    //──────────────────────────────────────────────────────────────
    {
        ExtractBonesFromMesh();
        // FinalBoneMatrices 크기만큼 모두 단위행렬로 초기화
        FinalBoneMatrices.clear();
        FinalBoneMatrices.resize(BoneCount, aiMatrix4x4());
    }

    //──────────────────────────────────────────────────────────────
    // 4) 메시(Vertex/Index) + 본 정보(BoneID/Weight) 추출
    //    - 여기서는 편의상 첫 번째 메시(aiMeshes[0])만 처리
    //──────────────────────────────────────────────────────────────
    if (MeshScene->mNumMeshes < 1)
    {
        MsgAssert("FBX 메시 로드 실패: 메시 개수 없음");
        return false;
    }

    const aiMesh* aiMeshPtr = MeshScene->mMeshes[0];
    _OutVertices.clear();
    _OutIndices.clear();
    _OutVertices.reserve(aiMeshPtr->mNumVertices);
    _OutIndices.reserve(aiMeshPtr->mNumFaces * 3);

    // (4-1) 정점 정보 채우기: Position, Normal, TexCoord, Color
    for (unsigned int v = 0; v < aiMeshPtr->mNumVertices; ++v)
    {
        Ext_DirectXVertexData vData;

        // (A) Position → float4(x,y,z,1)
        const aiVector3D& ap = aiMeshPtr->mVertices[v];
        vData.POSITION = float4(ap.x, ap.y, ap.z, 1.0f);

        // (B) Normal → float4(nx,ny,nz,0)
        if (aiMeshPtr->HasNormals())
        {
            const aiVector3D& an = aiMeshPtr->mNormals[v];
            vData.NORMAL = float4(an.x, an.y, an.z, 0.0f);
        }
        else
        {
            vData.NORMAL = float4(0, 1, 0, 0);
        }

        // (C) TexCoord UV (0번 채널만) → float4(u, 1-v, 0, 0)
        if (aiMeshPtr->HasTextureCoords(0))
        {
            const aiVector3D& au = aiMeshPtr->mTextureCoords[0][v];
            vData.TEXCOORD = float4(au.x, 1.0f - au.y, 0.0f, 0.0f);
        }
        else
        {
            vData.TEXCOORD = float4(0, 0, 0, 0);
        }

        // (D) Color: 흰색
        vData.COLOR = float4(1, 1, 1, 1);

        // (E) BONEID, WEIGHT: 기본 생성자에서 모두 0으로 초기화되어 있음
        //     → 5-3) 절에서 나중에 채워 줌

        _OutVertices.push_back(vData);
    }

    // (4-2) 본(Bone) 가중치 채우기 (aiBone → 정점별 BoneID, Weight)
    //       최대 4개 본만 저장(이미 vData.WEIGHT는 0으로 초기화됨)
    for (unsigned int b = 0; b < aiMeshPtr->mNumBones; ++b)
    {
        const aiBone* aiBonePtr = aiMeshPtr->mBones[b];
        std::string    boneName(aiBonePtr->mName.C_Str());

        auto it = BoneNameToInfo.find(boneName);
        if (it == BoneNameToInfo.end())
        {
            continue;
        }

        int boneID = it->second.ID;
        for (unsigned int w = 0; w < aiBonePtr->mNumWeights; ++w)
        {
            unsigned int vertID = aiBonePtr->mWeights[w].mVertexId;
            float         weight = aiBonePtr->mWeights[w].mWeight;
            Ext_DirectXVertexData& destV = _OutVertices[vertID];

            // 네 슬롯(0~3) 중 빈 슬롯에 BoneID/Weight 넣기
            for (int k = 0; k < 4; ++k)
            {
                bool isEmpty = false;
                switch (k)
                {
                case 0: isEmpty = (destV.WEIGHT.x == 0.0f); break;
                case 1: isEmpty = (destV.WEIGHT.y == 0.0f); break;
                case 2: isEmpty = (destV.WEIGHT.z == 0.0f); break;
                case 3: isEmpty = (destV.WEIGHT.w == 0.0f); break;
                }
                if (isEmpty)
                {
                    switch (k)
                    {
                    case 0:
                        destV.BONEID.x = (uint32_t)boneID;
                        destV.WEIGHT.x = weight;
                        break;
                    case 1:
                        destV.BONEID.y = (uint32_t)boneID;
                        destV.WEIGHT.y = weight;
                        break;
                    case 2:
                        destV.BONEID.z = (uint32_t)boneID;
                        destV.WEIGHT.z = weight;
                        break;
                    case 3:
                        destV.BONEID.w = (uint32_t)boneID;
                        destV.WEIGHT.w = weight;
                        break;
                    }
                    break;
                }
            }
        }
    }

    // (4-3) 인덱스 채우기 (Face마다 3개씩 → CW→CCW로 순서 뒤집기)
    for (unsigned int f = 0; f < aiMeshPtr->mNumFaces; ++f)
    {
        const aiFace& face = aiMeshPtr->mFaces[f];
        if (face.mNumIndices == 3)
        {
            _OutIndices.push_back(face.mIndices[0]);
            _OutIndices.push_back(face.mIndices[2]);
            _OutIndices.push_back(face.mIndices[1]);
        }
        else
        {
            // 삼각형이 아닌 다각형일 경우, (필요하다면) 자유롭게 처리
            for (unsigned int j = 0; j < face.mNumIndices; ++j)
                _OutIndices.push_back(face.mIndices[j]);
        }
    }

    return true;
}

//─────────────────────────────────────────────────────────────────────────
// [2] 애니메이션 데이터만 담긴 FBX 로드
//─────────────────────────────────────────────────────────────────────────
bool Ext_FBXAnimator::LoadAnimationFBX(const std::string& _AnimFilename)
{
    // 1) AnimScene에 애니메이션 FBX 읽기
    AnimScene = AnimImporter.ReadFile(_AnimFilename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights);

    if (!AnimScene || (AnimScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !AnimScene->mRootNode)
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
        
        // 각 채널마다 nodeName(=본 이름), 그 키프레임 배열 정보를 저장
        for (unsigned int c = 0; c < anim->mNumChannels; ++c)
        {
            const aiNodeAnim* channel = anim->mChannels[c];
            std::string boneName = channel->mNodeName.C_Str();
            BoneNameToAnimChannel[boneName] = channel;
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

    // (B) MeshScene의 루트 노드를 재귀 순회하면서 본별 행렬 계산
    aiMatrix4x4 identity; // 단위행렬
    ReadNodeHierarchy(_TimeInSeconds, MeshScene->mRootNode, identity);
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
    BoneNameToInfo.clear();
    BoneCount = 0;

    std::set<std::string> uniqueBoneNames;
    // FBX 안에 있는 모든 메시의 모든 aiBone 이름 수집
    for (unsigned int m = 0; m < MeshScene->mNumMeshes; ++m)
    {
        aiMesh* mesh = MeshScene->mMeshes[m];
        if (!mesh->HasBones()) continue;

        for (unsigned int b = 0; b < mesh->mNumBones; ++b)
            uniqueBoneNames.insert(mesh->mBones[b]->mName.C_Str());
    }

    // 수집된 고유 Bone 이름을 순서대로 BoneID 부여 및 OffsetMatrix 저장
    for (auto& boneName : uniqueBoneNames)
    {
        // 첫 번째로 등장하는 aiBone에서 OffsetMatrix 가져오기
        aiMatrix4x4 offset;
        bool found = false;
        for (unsigned int m = 0; m < MeshScene->mNumMeshes && !found; ++m)
        {
            aiMesh* mesh = MeshScene->mMeshes[m];
            if (!mesh->HasBones()) continue;

            for (unsigned int b = 0; b < mesh->mNumBones; ++b)
            {
                if (boneName == mesh->mBones[b]->mName.C_Str())
                {
                    offset = mesh->mBones[b]->mOffsetMatrix;
                    found = true;
                    break;
                }
            }
        }

        BoneInfo bi;
        bi.ID = BoneCount;
        bi.OffsetMatrix = offset; // 모델공간 → 본공간
        BoneNameToInfo[boneName] = bi;

        ++BoneCount;
    }
}

//─────────────────────────────────────────────────────────────────────────
// [E] 재귀적으로 노드 트리 순회하며 바인드포즈 or 애니메이션 TRS를 적용해서
//     최종 본 행렬(FinalBoneMatrices)에 넣어 주는 함수
//─────────────────────────────────────────────────────────────────────────
aiMatrix4x4 Ext_FBXAnimator::ReadNodeHierarchy(float _TimeInSeconds, const aiNode* _Node, const aiMatrix4x4& _ParentTransform)
{
    std::string nodeName = _Node->mName.C_Str();

    // (1) 바인드-포즈 트랜스폼
    aiMatrix4x4 nodeTransform = _Node->mTransformation;

    // (2) 애니메이션 채널이 있으면, 바인드-포즈를 무시하고 TRS를 보간해서 사용
    auto animIt = BoneNameToAnimChannel.find(nodeName);
    if (animIt != BoneNameToAnimChannel.end())
    {
        const aiNodeAnim* channel = animIt->second;

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
        return;
    }

    unsigned int index = FindPositionIndex(_AnimTime, _NodeAnim);
    unsigned int nextIndex = index + 1;
    if (nextIndex >= _NodeAnim->mNumPositionKeys)
    {
        _Out = _NodeAnim->mPositionKeys[index].mValue;
        return;
    }

    float time0 = static_cast<float>(_NodeAnim->mPositionKeys[index].mTime);
    float time1 = static_cast<float>(_NodeAnim->mPositionKeys[nextIndex].mTime);
    float factor = (_AnimTime - time0) / (time1 - time0);

    const aiVector3D& start = _NodeAnim->mPositionKeys[index].mValue;
    const aiVector3D& end = _NodeAnim->mPositionKeys[nextIndex].mValue;

    _Out = start + (end - start) * factor;
}

void Ext_FBXAnimator::CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    if (_NodeAnim->mNumRotationKeys == 1)
    {
        _Out = _NodeAnim->mRotationKeys[0].mValue;
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

    aiQuaternion::Interpolate(_Out, startQ, endQ, factor);
    _Out = _Out.Normalize();
}

void Ext_FBXAnimator::CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    if (_NodeAnim->mNumScalingKeys == 1)
    {
        _Out = _NodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int index = FindScalingIndex(_AnimTime, _NodeAnim);
    unsigned int nextIndex = index + 1;
    if (nextIndex >= _NodeAnim->mNumScalingKeys)
    {
        _Out = _NodeAnim->mScalingKeys[index].mValue;
        return;
    }

    float time0 = static_cast<float>(_NodeAnim->mScalingKeys[index].mTime);
    float time1 = static_cast<float>(_NodeAnim->mScalingKeys[nextIndex].mTime);
    float factor = (_AnimTime - time0) / (time1 - time0);

    const aiVector3D& startS = _NodeAnim->mScalingKeys[index].mValue;
    const aiVector3D& endS = _NodeAnim->mScalingKeys[nextIndex].mValue;

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