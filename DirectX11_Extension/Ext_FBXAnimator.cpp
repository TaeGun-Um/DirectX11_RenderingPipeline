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
    // 1) 메시 + 바인드포즈 FBX 읽기 (DirectX 좌표계로 변환)
    MeshScene = MeshImporter.ReadFile(
        _TposeFilename,
        aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_CalcTangentSpace
        | aiProcess_JoinIdenticalVertices
        | aiProcess_LimitBoneWeights
        | aiProcess_ConvertToLeftHanded    // ← DirectX(LH) 기준으로 읽도록
        | aiProcess_FlipUVs               // UV V축을 상하 반전
    );
    if (!MeshScene
        || (MeshScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        || !MeshScene->mRootNode)
    {
        MsgAssert("FBX 메시 로드 실패");
        return false;
    }

    // 2) 루트 노드 변환 역행렬(바인드포즈 보정용)
    {
        aiMatrix4x4 inv = MeshScene->mRootNode->mTransformation;
        inv.Inverse();
        GlobalInverseTransform = inv;
    }

    // 3) 본 정보 추출 (BoneNameToInfo, BoneCount 세팅)
    ExtractBonesFromMesh();
    FinalBoneMatrices.clear();
    FinalBoneMatrices.resize(BoneCount, aiMatrix4x4());

    // 4) 첫 번째 메시(aiMeshes[0])만 처리
    if (MeshScene->mNumMeshes < 1)
    {
        MsgAssert("FBX 메시 로드 실패: 메시 개수 없음");
        return false;
    }

    const aiMesh* AimeshPtr = MeshScene->mMeshes[0];
    _OutVertices.clear();
    _OutIndices.clear();
    _OutVertices.reserve(AimeshPtr->mNumVertices);
    _OutIndices.reserve(AimeshPtr->mNumFaces * 3);

    // 4-1) 정점 정보 채우기: POSITION, COLOR, TEXCOORD, NORMAL, BONEID, WEIGHT
    for (unsigned int v = 0; v < AimeshPtr->mNumVertices; ++v)
    {
        Ext_DirectXVertexData VData;

        // (A) POSITION
        const aiVector3D& Ap = AimeshPtr->mVertices[v];
        VData.POSITION = float4(Ap.x, Ap.y, Ap.z, 1.0f);

        // (B) NORMAL
        if (AimeshPtr->HasNormals())
        {
            const aiVector3D& An = AimeshPtr->mNormals[v];
            VData.NORMAL = float4(An.x, An.y, An.z, 0.0f);
        }
        else
        {
            VData.NORMAL = float4(0.0f, 1.0f, 0.0f, 0.0f);
        }

        // ▼ aiProcess_FlipUVs 를 켰다면, 여기서는 1.0f - y를 제거하고 그대로 써야 한다! ▼
        if (AimeshPtr->HasTextureCoords(0))
        {
            const aiVector3D& Au = AimeshPtr->mTextureCoords[0][v];
            // aiProcess_FlipUVs 가 켜지면 Assimp가 로드하면서 (0,0)이 아래에서 위로 뒤집어 놓음
            VData.TEXCOORD = float4(Au.x, Au.y, 0.0f, 0.0f);
        }
        else
        {
            VData.TEXCOORD = float4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        // (D) COLOR: 기본 흰색
        VData.COLOR = float4(1.0f, 1.0f, 1.0f, 1.0f);

        // (E) BONEID, WEIGHT: 생성자에서 모두 0으로 초기화됨
        //     → 아래에서 aiBone 정보를 기반으로 채워 줌

        _OutVertices.push_back(VData);
    }

    // 4-2) 본( aiBone ) 가중치 채우기 (최대 4개)
    for (unsigned int b = 0; b < AimeshPtr->mNumBones; ++b)
    {
        const aiBone* AibonePtr = AimeshPtr->mBones[b];
        std::string    BoneName(AibonePtr->mName.C_Str());

        auto It = BoneNameToInfo.find(BoneName);
        if (It == BoneNameToInfo.end())
            continue;

        int BoneID = It->second.ID;
        for (unsigned int w = 0; w < AibonePtr->mNumWeights; ++w)
        {
            unsigned int VertID = AibonePtr->mWeights[w].mVertexId;
            float         Weight = AibonePtr->mWeights[w].mWeight;
            Ext_DirectXVertexData& DestV = _OutVertices[VertID];

            // 네 슬롯(0~3) 중 빈 슬롯에 BoneID/Weight 저장
            for (int k = 0; k < 4; ++k)
            {
                bool IsEmpty = false;
                switch (k)
                {
                case 0:  IsEmpty = (DestV.WEIGHT.x == 0.0f); break;
                case 1:  IsEmpty = (DestV.WEIGHT.y == 0.0f); break;
                case 2:  IsEmpty = (DestV.WEIGHT.z == 0.0f); break;
                case 3:  IsEmpty = (DestV.WEIGHT.w == 0.0f); break;
                }
                if (IsEmpty)
                {
                    switch (k)
                    {
                    case 0:
                        DestV.BONEID.x = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.x = Weight;
                        break;
                    case 1:
                        DestV.BONEID.y = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.y = Weight;
                        break;
                    case 2:
                        DestV.BONEID.z = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.z = Weight;
                        break;
                    case 3:
                        DestV.BONEID.w = static_cast<uint32_t>(BoneID);
                        DestV.WEIGHT.w = Weight;
                        break;
                    }
                    break;
                }
            }
        }
    }

    // 4-3) 인덱스 채우기 (삼각형 → CCW 순서)
    for (unsigned int f = 0; f < AimeshPtr->mNumFaces; ++f)
    {
        const aiFace& Face = AimeshPtr->mFaces[f];
        if (Face.mNumIndices == 3)
        {
            // 원래 FBX가 CW 순서라면, (0,1,2) → CCW를 위해 (0,2,1)로 뒤집기
            _OutIndices.push_back(Face.mIndices[0]);
            _OutIndices.push_back(Face.mIndices[2]);
            _OutIndices.push_back(Face.mIndices[1]);
        }
        else
        {
            // 삼각형이 아닌 경우, 인덱스를 있는 그대로 넣되
            // 반드시 CCW가 되도록 FBX 에디터에서 Triangulate를 미리 수행해 두세요.
            for (unsigned int j = 0; j < Face.mNumIndices; ++j)
            {
                _OutIndices.push_back(Face.mIndices[j]);
            }
        }
    }

    return true;
}

//─────────────────────────────────────────────────────────────────────────
// [2] 애니메이션 데이터만 담긴 FBX 로드
//─────────────────────────────────────────────────────────────────────────
bool Ext_FBXAnimator::LoadAnimationFBX(const std::string& _AnimFilename)
{
    // 1) 애니메이션 FBX 읽기 → DirectX(LH) 좌표계로 변환
    AnimScene = AnimImporter.ReadFile(
        _AnimFilename,
        aiProcess_Triangulate
        | aiProcess_GenSmoothNormals
        | aiProcess_CalcTangentSpace
        | aiProcess_JoinIdenticalVertices
        | aiProcess_LimitBoneWeights
        | aiProcess_ConvertToLeftHanded    // ← 반드시 추가
    );
    if (!AnimScene || (AnimScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !AnimScene->mRootNode)
    {
        MsgAssert("FBX 애니메이션 로드 실패");
        return false;
    }

    // 2) 애니메이션 개수 확인
    if (AnimScene->mNumAnimations == 0)
    {
        MsgAssert("애니메이션 채널이 들어 있지 않음");
        return false;
    }

    // 3) 기본 애니메이션 채널 매핑
    CurrentAnimIndex = 0;
    CurrentAnimation = AnimScene->mAnimations[0];
    BoneNameToAnimChannel.clear();
    for (unsigned int c = 0; c < CurrentAnimation->mNumChannels; ++c)
    {
        const aiNodeAnim* channel = CurrentAnimation->mChannels[c];
        BoneNameToAnimChannel[channel->mNodeName.C_Str()] = channel;
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
    // 0) 유효성 검사
    if (!MeshScene || !AnimScene || !CurrentAnimation)
    {
        return;
    }

    // 1) FinalBoneMatrices 초기화
    for (unsigned int i = 0; i < BoneCount; ++i)
    {
        FinalBoneMatrices[i] = aiMatrix4x4();
    }

    const std::string NodeName = "mixamorig:Hips";
    const aiNode* FirstNode = FindNodeByName(MeshScene->mRootNode, NodeName);

    // 2-1) 루트부터 hips까지의 글로벌 바인드 포즈 구하기
    aiMatrix4x4 NodeTransform = FirstNode->mTransformation;
    NodeTransform.Inverse();
    GlobalInverseTransform = NodeTransform;

    aiMatrix4x4 identity; // 기본 생성자 → 항등행렬
    ReadNodeHierarchy(_TimeInSeconds, FirstNode, identity);
}

//─────────────────────────────────────────────────────────────────────────
// [5] CB_SkinnedMatrix 형태로 FinalBoneMatrices 복사 (쉐이더용)
//─────────────────────────────────────────────────────────────────────────
CB_SkinnedMatrix Ext_FBXAnimator::RenderSkinnedMesh()
{
    CB_SkinnedMatrix cb = {};

    // FinalBoneMatrices[i]를 그대로 XMMATRIX 생성자에 넘겨 주면 됩니다
    for (size_t i = 0; i < FinalBoneMatrices.size() && i < MAX_BONES; ++i)
    {
        const aiMatrix4x4& m = FinalBoneMatrices[i];

        // “행 우선(row-major)”대로 XMMATRIX 생성
        XMMATRIX xm = XMMATRIX(
            m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4
        );

        // HLSL 쪽이 row_major로 읽기 때문에, 추가 Transpose 없이 그대로 넘깁니다.
        cb.Bones[i] = xm;
    }

    // 나머지 본은 단위행렬로 채우기
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
    // (1) 노드 이름
    std::string nodeName = _Node->mName.C_Str();

    // (2) 바인드 포즈(기본) 행렬
    //aiMatrix4x4 nodeTransform = _Node->mTransformation;
    aiMatrix4x4 Mat;

    // (3) 애니메이션 채널 보간 여부
    auto animIt = BoneNameToAnimChannel.find(nodeName);
    if (animIt != BoneNameToAnimChannel.end())
    {
        const aiNodeAnim* channel = animIt->second;

        // 초 → 틱 변환
        float ticks = TimeInTicks(_TimeInSeconds);
        float duration = static_cast<float>(CurrentAnimation->mDuration);
        float animTime = fmod(ticks, duration);

        // 보간 후 TRS
        aiVector3D interpPos;
        aiQuaternion interpRot;
        aiVector3D interpScale;
        CalcInterpolatedPosition(interpPos, animTime, channel);
        CalcInterpolatedRotation(interpRot, animTime, channel);
        CalcInterpolatedScaling(interpScale, animTime, channel);

        aiMatrix4x4 T, R, S;
        aiMatrix4x4::Translation(interpPos, T);
        R = aiMatrix4x4(interpRot.GetMatrix());
        aiMatrix4x4::Scaling(interpScale, S);

        Mat = T * R * S;
    }

    // (4) 부모 변환과 곱해 글로벌 변환 계산
    aiMatrix4x4 globalTransform = _ParentTransform * Mat;

    // (5) 이 노드가 본(BoneNameToInfo에 존재)이라면 최종 스킨 행렬 계산
    auto boneIt = BoneNameToInfo.find(nodeName);
    if (boneIt != BoneNameToInfo.end())
    {
        int boneID = boneIt->second.ID;
        const aiMatrix4x4& offset = boneIt->second.OffsetMatrix;

        // 최종 스킨 행렬: GlobalInverseTransform × globalTransform × offsetMatrix
        aiMatrix4x4 finalBone = globalTransform * offset;
        FinalBoneMatrices[boneID] = finalBone;
    }

    // (6) 자식 노드 순회
    for (unsigned int i = 0; i < _Node->mNumChildren; ++i)
    {
        ReadNodeHierarchy(_TimeInSeconds,
            _Node->mChildren[i],
            globalTransform);
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
    // 만약 이 애니메이션이 “TicksPerSecond = 0”이라면, 기본 25fps(틱)로 가정
    return _TimeInSeconds * 25.0f;
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



const aiNode* Ext_FBXAnimator::FindNodeByName(const aiNode* node, const std::string& name)
{
    if (!node) return nullptr;

    if (std::string(node->mName.C_Str()) == name)
        return node;

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        const aiNode* found = FindNodeByName(node->mChildren[i], name);
        if (found)
            return found;
    }
    return nullptr;
}


// “node”의 전역 Transform(최상위(RootNode)에서부터 이 노드에 이르는 모든 mTransformation을 곱한 결과)”을 계산
aiMatrix4x4 Ext_FBXAnimator::GetGlobalTransform(const aiNode* node)
{
    aiMatrix4x4 mat = node->mTransformation;
    const aiNode* parent = node->mParent;
    while (parent)
    {
        mat = parent->mTransformation * mat;
        parent = parent->mParent;
    }
    return mat;
}