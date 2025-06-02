#include "PrecompileHeader.h"
#include "Ext_Animator.h"
#include <DirectXMath.h>
#include "Ext_DirectXMesh.h"

#include <cstdio>  // printf

// 스켈레탈 메시 적용
void Ext_Animator::SetSkeltalMesh(std::string_view _MeshName)
{
    SkeletalMesh = Ext_DirectXMesh::FindSkeletalMesh(_MeshName);
    FinalBoneMatrices.clear();
    FinalBoneMatrices.resize(SkeletalMesh->GetBoneCount(), aiMatrix4x4());
}

// 애니메이션 생성
bool Ext_Animator::LoadAnimation(std::string_view _FilePath)
{
    // 1) 애니메이션 FBX 읽기 → DirectX(LH) 좌표계로 변환
    AIAnimScene = AnimImporter.ReadFile(_FilePath.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights);
    if (!AIAnimScene || (AIAnimScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) /*|| !AnimSceneAIAnimScenemRootNode*/)
    {
        MsgAssert("FBX 애니메이션 로드 실패");
        return false;
    }

    // 2) 애니메이션 개수 확인
    if (AIAnimScene->mNumAnimations == 0)
    {
        MsgAssert("애니메이션 채널이 들어 있지 않음");
        return false;
    }

    // 3) 기본 애니메이션 채널 매핑
    //CurrentAnimIndex = 0;
    CurrentAnimation = AIAnimScene->mAnimations[0];
    BoneNameToAnimChannel.clear();
    for (unsigned int c = 0; c < CurrentAnimation->mNumChannels; ++c)
    {
        const aiNodeAnim* channel = CurrentAnimation->mChannels[c];
        BoneNameToAnimChannel[channel->mNodeName.C_Str()] = channel;
    }

    return true;
}

// 재생할 애니메이션 선택
bool Ext_Animator::SetAnimation(UINT _AnimIndex)
{
    if (!AIAnimScene)
    {
        MsgAssert("애니메이션을 만들지 않아 재생할 수 없습니다.");
        return false;
    }

    if (_AnimIndex >= AIAnimScene->mNumAnimations)
    {
        return false;
    }

    CurrentAnimIndex = _AnimIndex;
    CurrentAnimation = AIAnimScene->mAnimations[CurrentAnimIndex];

    BoneNameToAnimChannel.clear();
    for (unsigned int c = 0; c < CurrentAnimation->mNumChannels; ++c)
    {
        const aiNodeAnim* channel = CurrentAnimation->mChannels[c];
        std::string boneName = channel->mNodeName.C_Str();
        BoneNameToAnimChannel[boneName] = channel;
    }
    return true;
}

bool Ext_Animator::ChangeAnimation(UINT _AnimIndex)
{
    return false;
}

// 선택된 애니메이션 재생
void Ext_Animator::UpdateAnimation(float _TimeInSeconds)
{
    if (!CurrentAnimation) // aiScene이 없거나
    {
        return;
    }

    // 1) FinalBoneMatrices 초기화
    for (size_t i = 0; i < SkeletalMesh->GetBoneCount(); ++i)
    {
        FinalBoneMatrices[i] = aiMatrix4x4();
    }

    const std::string NodeName = "mixamorig:Hips";
    const aiNode* FirstNode = FindNodeByName(SkeletalMesh->GetMeshScene()->mRootNode, NodeName);

    // 2-1) 루트부터 hips까지의 글로벌 바인드 포즈 구하기
    aiMatrix4x4 NodeTransform = FirstNode->mTransformation;

    aiMatrix4x4 identity; // 기본 생성자 → 항등행렬
    ReadNodeHierarchy(_TimeInSeconds, FirstNode, identity);

    RenderSkinnedMesh(); // 다 돌았으면 CB에 값 저장
}

// 재귀적으로 노드 트리를 순회하여 FinalBoneMatrices에 값 적용
aiMatrix4x4 Ext_Animator::ReadNodeHierarchy(float _TimeInSeconds, const aiNode* _Node, const aiMatrix4x4& _ParentTransform)
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
    auto boneIt = SkeletalMesh->GetBoneInfomations().find(nodeName);
    if (boneIt != SkeletalMesh->GetBoneInfomations().end())
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
// [5] CB_SkinnedMatrix 형태로 FinalBoneMatrices 복사 (쉐이더용)
//─────────────────────────────────────────────────────────────────────────
void Ext_Animator::RenderSkinnedMesh()
{
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
        CBMat.Bones[i] = xm;
    }

    // 나머지 본은 단위행렬로 채우기
    for (size_t i = FinalBoneMatrices.size(); i < MAX_BONES; ++i)
    {
        CBMat.Bones[i] = XMMatrixIdentity();
    }
}

//─────────────────────────────────────────────────────────────────────────
// [F] 애니메이션 키프레임 보간 함수들
//─────────────────────────────────────────────────────────────────────────
unsigned int Ext_Animator::FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    for (unsigned int i = 0; i + 1 < _NodeAnim->mNumPositionKeys; ++i)
    {
        if (_AnimTime < static_cast<float>(_NodeAnim->mPositionKeys[i + 1].mTime))
            return i;
    }
    return _NodeAnim->mNumPositionKeys - 1;
}

unsigned int Ext_Animator::FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    for (unsigned int i = 0; i + 1 < _NodeAnim->mNumRotationKeys; ++i)
    {
        if (_AnimTime < static_cast<float>(_NodeAnim->mRotationKeys[i + 1].mTime))
            return i;
    }
    return _NodeAnim->mNumRotationKeys - 1;
}

unsigned int Ext_Animator::FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim)
{
    for (unsigned int i = 0; i + 1 < _NodeAnim->mNumScalingKeys; ++i)
    {
        if (_AnimTime < static_cast<float>(_NodeAnim->mScalingKeys[i + 1].mTime))
            return i;
    }
    return _NodeAnim->mNumScalingKeys - 1;
}

void Ext_Animator::CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
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

void Ext_Animator::CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
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

void Ext_Animator::CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim)
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
float Ext_Animator::TimeInTicks(float _TimeInSeconds) const
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
void Ext_Animator::PrintAiMatrix(const aiMatrix4x4& m, const char* name)
{
    if (name && name[0] != '\0')
        printf("[%s]\n", name);

    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.a1, m.a2, m.a3, m.a4);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.b1, m.b2, m.b3, m.b4);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.c1, m.c2, m.c3, m.c4);
    printf("  [ %8.4f  %8.4f  %8.4f  %8.4f ]\n", m.d1, m.d2, m.d3, m.d4);
}

void Ext_Animator::PrintXMMATRIX(const DirectX::XMMATRIX& xm, const char* name)
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



const aiNode* Ext_Animator::FindNodeByName(const aiNode* node, const std::string& name)
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
aiMatrix4x4 Ext_Animator::GetGlobalTransform(const aiNode* node)
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