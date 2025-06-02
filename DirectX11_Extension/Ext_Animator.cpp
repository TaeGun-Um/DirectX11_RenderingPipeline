#include "PrecompileHeader.h"
#include "Ext_Animator.h"
#include <DirectXMath.h>
#include "Ext_DirectXMesh.h"

#include <DirectX11_Base//Base_Directory.h>

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
    std::shared_ptr<AnimationData> NewAnimData = std::make_shared<AnimationData>();

    // 1) 애니메이션 FBX 읽기 → DirectX(LH) 좌표계로 변환
    const aiScene* NewAnimScene = NewAnimData->AnimImporter.ReadFile(_FilePath.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights);
    if (!NewAnimScene || (NewAnimScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) /*|| !AnimSceneAIAnimScenemRootNode*/)
    {
        MsgAssert("FBX 애니메이션 로드 실패");
        return false;
    }

    // 2) 애니메이션 개수 확인
    if (NewAnimScene->mNumAnimations == 0)
    {
        MsgAssert("애니메이션 채널이 들어 있지 않음");
        return false;
    }

    // 3) 기본 애니메이션 채널 매핑
    std::vector<std::string> TranslationNodes; // 루트모션 끄려고 찾는것
    CurrentAnimation = NewAnimScene->mAnimations[0];
    BoneNameToAnimChannel.clear();
    for (unsigned int c = 0; c < CurrentAnimation->mNumChannels; ++c)
    {
        const aiNodeAnim* Channel = CurrentAnimation->mChannels[c];
        BoneNameToAnimChannel[Channel->mNodeName.C_Str()] = Channel;
        
        if (Channel->mNumPositionKeys > 0) // 위치 키가 하나라도 있으면 Translation이 움직이는것
        {
            TranslationNodes.push_back(Channel->mNodeName.C_Str());
        }
    }

    NewAnimData->AIAnimScene = NewAnimScene;
    std::string NewName = Base_Directory::GetFileName(_FilePath);
    std::string UpperName = Base_String::ToUpper(NewName);

    if (AnimationDatas.find(UpperName) != AnimationDatas.end())
    {
        MsgAssert("이미 동일한 이름의 애니메이션이 등록되어 있습니다. " + UpperName);
        return false;
    }

    const aiNode* RootNode = SkeletalMesh->GetMeshScene()->mRootNode; // 씬 루트 노드 가져오기
    RootMotionBoneName = FindRootMotionNode(RootNode, TranslationNodes); // 최상위 Translation 채널(부모가 없는 채널)을 찾아서 저장

    AnimationDatas[UpperName] = NewAnimData; // 다 끝났으면 저장

    return true;
}

// 재생할 애니메이션 선택
bool Ext_Animator::SetAnimation(std::string_view _AnimName)
{
    std::string UpperName = Base_String::ToUpper(_AnimName.data());
    auto Iter = AnimationDatas.find(UpperName);
    if (Iter == AnimationDatas.end())
    {
        MsgAssert("이런 이름의 애니메이션은 등록된 적이 없습니다. " + UpperName);
        return false;
    }

    std::shared_ptr<AnimationData> CurAnimData = Iter->second;

    if (!CurAnimData->AIAnimScene)
    {
        MsgAssert("애니메이션을 만들지 않아 재생할 수 없습니다.");
        return false;
    }

    AccumulatedTime = 0.f;
    // (2) CurrentAnimation 지정 및 채널 맵 갱신
    CurrentAnimation = CurAnimData->AIAnimScene->mAnimations[0];
    BoneNameToAnimChannel.clear();
    for (size_t i = 0; i < CurrentAnimation->mNumChannels; ++i)
    {
        const aiNodeAnim* Channel = CurrentAnimation->mChannels[i];
        std::string BoneName = Channel->mNodeName.C_Str();
        BoneNameToAnimChannel[BoneName] = Channel;
    }

    return true;
}

// 선택된 애니메이션 재생
void Ext_Animator::UpdateAnimation(float _DeltaTime)
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

    AccumulatedTime += _DeltaTime;

    const aiNode* RootNode = SkeletalMesh->GetMeshScene()->mRootNode;
    aiMatrix4x4 Identity; // 기본 생성자 → 항등행렬
    ReadNodeHierarchy(AccumulatedTime, RootNode, Identity);

    RenderSkinnedMesh(); // 다 돌았으면 CB에 값 저장
}

static constexpr char ROOT_BONE_NAME[] = "mixamorig:Hips";

// 재귀적으로 노드 트리를 순회하여 FinalBoneMatrices에 값 적용
aiMatrix4x4 Ext_Animator::ReadNodeHierarchy(float _AccumulatedTime, const aiNode* _CurNode, const aiMatrix4x4& _ParentTransform)
{
    // 현재 노드 이름
    std::string CurNodeName = _CurNode->mName.C_Str();
    aiMatrix4x4 Mat;

    // 애니메이션 채널 보간 여부
    auto AnimIter = BoneNameToAnimChannel.find(CurNodeName);
    if (AnimIter != BoneNameToAnimChannel.end())
    {
        const aiNodeAnim* Channel = AnimIter->second;

        // 초 → 틱 변환
        float Ticks = TimeInTicks(_AccumulatedTime);
        float Duration = static_cast<float>(CurrentAnimation->mDuration);
        float AnimTime = fmod(Ticks, Duration);

        // 보간 후 TRS
        aiVector3D InterpPosition;
        aiQuaternion InterpRotation;
        aiVector3D InterpScale;
        CalcInterpolatedPosition(InterpPosition, AnimTime, Channel);
        CalcInterpolatedRotation(InterpRotation, AnimTime, Channel);
        CalcInterpolatedScaling(InterpScale, AnimTime, Channel);


        if (CurNodeName == RootMotionBoneName)
        {
            InterpPosition.x = 0.0f;
            InterpPosition.y = 0.0f;
            InterpPosition.z = 0.0f;
        }

        // 일단 사용(루트모션 제거용)
        //InterpPosition.z = 0.0f;

        aiMatrix4x4 TranslationMat;
        aiMatrix4x4 RotationMat;
        aiMatrix4x4 ScaleMat;
        aiMatrix4x4::Translation(InterpPosition, TranslationMat);
        RotationMat = aiMatrix4x4(InterpRotation.GetMatrix());
        aiMatrix4x4::Scaling(InterpScale, ScaleMat);

        Mat = TranslationMat * RotationMat * ScaleMat;
    }

    // 부모 변환과 곱해 글로벌 변환 계산
    aiMatrix4x4 GlobalTransform = _ParentTransform * Mat;

    // 이 노드가 본(BoneNameToInfo에 존재)이라면 최종 스킨 행렬 계산
    auto BoneIter = SkeletalMesh->GetBoneInfomations().find(CurNodeName);
    if (BoneIter != SkeletalMesh->GetBoneInfomations().end())
    {
        int BoneID = BoneIter->second.ID;
        const aiMatrix4x4& OffsetMat = BoneIter->second.OffsetMatrix;

        aiMatrix4x4 FinalBoneTransform = GlobalTransform * OffsetMat;
        FinalBoneMatrices[BoneID] = FinalBoneTransform;
    }

    // (6) 자식 노드 순회
    for (unsigned int i = 0; i < _CurNode->mNumChildren; ++i)
    {
        ReadNodeHierarchy(_AccumulatedTime, _CurNode->mChildren[i], GlobalTransform);
    }

    return GlobalTransform;
}

std::string Ext_Animator::FindRootMotionNode(const aiNode* _RootNode, const std::vector<std::string>& _TranslationNodes)
{
    // 씬 트리에서 이름으로 aiNode*를 찾아주는 람다(내부 사용)
    auto FindNodeByName = [&](auto&& _Self, const aiNode* _Node, const std::string& _Name) -> const aiNode*
        {
            if (!_Node) return nullptr;
            if (_Node->mName.C_Str() == _Name)
            {
                return _Node;
            }
            for (unsigned int i = 0; i < _Node->mNumChildren; ++i)
            {
                const aiNode* found = _Self(_Self, _Node->mChildren[i], _Name);
                if (found)
                {
                    return found;
                }
            }
            return nullptr;
        };

    // “Translation이 있는 채널 이름들” 중 최상위 노드를 찾는다.
    for (const auto& BoneName : _TranslationNodes)
    {
        // 씬 트리에서 그 이름의 aiNode*를 찾는다
        const aiNode* Node = FindNodeByName(FindNodeByName, _RootNode, BoneName);
        if (!Node)
        {
            continue;
        }

        // 부모 계층을 타고 올라가면서, 부모가 채널 맵에 속하는지 검사
        const aiNode* Parent = Node->mParent;
        bool hasParentChannel = false;
        while (Parent != nullptr)
        {
            if (BoneNameToAnimChannel.find(Parent->mName.C_Str()) != BoneNameToAnimChannel.end())
            {
                hasParentChannel = true;
                break;
            }
            Parent = Parent->mParent;
        }

        // 부모 채널이 없으면 “최상위 Translation 채널”로 간주
        if (!hasParentChannel)
        {
            return BoneName;
        }
    }

    // 후보가 하나도 없거나, 모두 부모가 있으면, 그냥 첫 번째를 반환
    if (!_TranslationNodes.empty())
    {
        return _TranslationNodes.front();
    }

    return "";  // _TranslationNodes가 비어 있으면 빈 문자열 반환
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
        DirectX::XMMATRIX xm = DirectX::XMMATRIX(
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
        CBMat.Bones[i] = DirectX::XMMatrixIdentity();
    }
}

// 델타 타임을 assimp 틱 단위로 변환
float Ext_Animator::TimeInTicks(float _AccumulatedTime) const
{
    if (CurrentAnimation && CurrentAnimation->mTicksPerSecond != 0.0f)
    {
        return _AccumulatedTime * static_cast<float>(CurrentAnimation->mTicksPerSecond);
    }
    // 만약 이 애니메이션이 “TicksPerSecond = 0”이라면, 기본 25fps(틱)로 가정
    return _AccumulatedTime * 25.0f;
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
