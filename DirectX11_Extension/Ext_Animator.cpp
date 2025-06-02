#include "PrecompileHeader.h"
#include "Ext_Animator.h"
#include <DirectXMath.h>
#include "Ext_DirectXMesh.h"

#include <DirectX11_Base//Base_Directory.h>

// ���̷�Ż �޽� ����
void Ext_Animator::SetSkeltalMesh(std::string_view _MeshName)
{
    SkeletalMesh = Ext_DirectXMesh::FindSkeletalMesh(_MeshName);
    FinalBoneMatrices.clear();
    FinalBoneMatrices.resize(SkeletalMesh->GetBoneCount(), aiMatrix4x4());
}

// �ִϸ��̼� ����
bool Ext_Animator::LoadAnimation(std::string_view _FilePath)
{
    std::shared_ptr<AnimationData> NewAnimData = std::make_shared<AnimationData>();

    // 1) �ִϸ��̼� FBX �б� �� DirectX(LH) ��ǥ��� ��ȯ
    const aiScene* NewAnimScene = NewAnimData->AnimImporter.ReadFile(_FilePath.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights);
    if (!NewAnimScene || (NewAnimScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) /*|| !AnimSceneAIAnimScenemRootNode*/)
    {
        MsgAssert("FBX �ִϸ��̼� �ε� ����");
        return false;
    }

    // 2) �ִϸ��̼� ���� Ȯ��
    if (NewAnimScene->mNumAnimations == 0)
    {
        MsgAssert("�ִϸ��̼� ä���� ��� ���� ����");
        return false;
    }

    // 3) �⺻ �ִϸ��̼� ä�� ����
    std::vector<std::string> TranslationNodes; // ��Ʈ��� ������ ã�°�
    CurrentAnimation = NewAnimScene->mAnimations[0];
    BoneNameToAnimChannel.clear();
    for (unsigned int c = 0; c < CurrentAnimation->mNumChannels; ++c)
    {
        const aiNodeAnim* Channel = CurrentAnimation->mChannels[c];
        BoneNameToAnimChannel[Channel->mNodeName.C_Str()] = Channel;
        
        if (Channel->mNumPositionKeys > 0) // ��ġ Ű�� �ϳ��� ������ Translation�� �����̴°�
        {
            TranslationNodes.push_back(Channel->mNodeName.C_Str());
        }
    }

    NewAnimData->AIAnimScene = NewAnimScene;
    std::string NewName = Base_Directory::GetFileName(_FilePath);
    std::string UpperName = Base_String::ToUpper(NewName);

    if (AnimationDatas.find(UpperName) != AnimationDatas.end())
    {
        MsgAssert("�̹� ������ �̸��� �ִϸ��̼��� ��ϵǾ� �ֽ��ϴ�. " + UpperName);
        return false;
    }

    const aiNode* RootNode = SkeletalMesh->GetMeshScene()->mRootNode; // �� ��Ʈ ��� ��������
    RootMotionBoneName = FindRootMotionNode(RootNode, TranslationNodes); // �ֻ��� Translation ä��(�θ� ���� ä��)�� ã�Ƽ� ����

    AnimationDatas[UpperName] = NewAnimData; // �� �������� ����

    return true;
}

// ����� �ִϸ��̼� ����
bool Ext_Animator::SetAnimation(std::string_view _AnimName)
{
    std::string UpperName = Base_String::ToUpper(_AnimName.data());
    auto Iter = AnimationDatas.find(UpperName);
    if (Iter == AnimationDatas.end())
    {
        MsgAssert("�̷� �̸��� �ִϸ��̼��� ��ϵ� ���� �����ϴ�. " + UpperName);
        return false;
    }

    std::shared_ptr<AnimationData> CurAnimData = Iter->second;

    if (!CurAnimData->AIAnimScene)
    {
        MsgAssert("�ִϸ��̼��� ������ �ʾ� ����� �� �����ϴ�.");
        return false;
    }

    AccumulatedTime = 0.f;
    // (2) CurrentAnimation ���� �� ä�� �� ����
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

// ���õ� �ִϸ��̼� ���
void Ext_Animator::UpdateAnimation(float _DeltaTime)
{
    if (!CurrentAnimation) // aiScene�� ���ų�
    {
        return;
    }

    // 1) FinalBoneMatrices �ʱ�ȭ
    for (size_t i = 0; i < SkeletalMesh->GetBoneCount(); ++i)
    {
        FinalBoneMatrices[i] = aiMatrix4x4();
    }

    AccumulatedTime += _DeltaTime;

    const aiNode* RootNode = SkeletalMesh->GetMeshScene()->mRootNode;
    aiMatrix4x4 Identity; // �⺻ ������ �� �׵����
    ReadNodeHierarchy(AccumulatedTime, RootNode, Identity);

    RenderSkinnedMesh(); // �� �������� CB�� �� ����
}

static constexpr char ROOT_BONE_NAME[] = "mixamorig:Hips";

// ��������� ��� Ʈ���� ��ȸ�Ͽ� FinalBoneMatrices�� �� ����
aiMatrix4x4 Ext_Animator::ReadNodeHierarchy(float _AccumulatedTime, const aiNode* _CurNode, const aiMatrix4x4& _ParentTransform)
{
    // ���� ��� �̸�
    std::string CurNodeName = _CurNode->mName.C_Str();
    aiMatrix4x4 Mat;

    // �ִϸ��̼� ä�� ���� ����
    auto AnimIter = BoneNameToAnimChannel.find(CurNodeName);
    if (AnimIter != BoneNameToAnimChannel.end())
    {
        const aiNodeAnim* Channel = AnimIter->second;

        // �� �� ƽ ��ȯ
        float Ticks = TimeInTicks(_AccumulatedTime);
        float Duration = static_cast<float>(CurrentAnimation->mDuration);
        float AnimTime = fmod(Ticks, Duration);

        // ���� �� TRS
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

        // �ϴ� ���(��Ʈ��� ���ſ�)
        //InterpPosition.z = 0.0f;

        aiMatrix4x4 TranslationMat;
        aiMatrix4x4 RotationMat;
        aiMatrix4x4 ScaleMat;
        aiMatrix4x4::Translation(InterpPosition, TranslationMat);
        RotationMat = aiMatrix4x4(InterpRotation.GetMatrix());
        aiMatrix4x4::Scaling(InterpScale, ScaleMat);

        Mat = TranslationMat * RotationMat * ScaleMat;
    }

    // �θ� ��ȯ�� ���� �۷ι� ��ȯ ���
    aiMatrix4x4 GlobalTransform = _ParentTransform * Mat;

    // �� ��尡 ��(BoneNameToInfo�� ����)�̶�� ���� ��Ų ��� ���
    auto BoneIter = SkeletalMesh->GetBoneInfomations().find(CurNodeName);
    if (BoneIter != SkeletalMesh->GetBoneInfomations().end())
    {
        int BoneID = BoneIter->second.ID;
        const aiMatrix4x4& OffsetMat = BoneIter->second.OffsetMatrix;

        aiMatrix4x4 FinalBoneTransform = GlobalTransform * OffsetMat;
        FinalBoneMatrices[BoneID] = FinalBoneTransform;
    }

    // (6) �ڽ� ��� ��ȸ
    for (unsigned int i = 0; i < _CurNode->mNumChildren; ++i)
    {
        ReadNodeHierarchy(_AccumulatedTime, _CurNode->mChildren[i], GlobalTransform);
    }

    return GlobalTransform;
}

std::string Ext_Animator::FindRootMotionNode(const aiNode* _RootNode, const std::vector<std::string>& _TranslationNodes)
{
    // �� Ʈ������ �̸����� aiNode*�� ã���ִ� ����(���� ���)
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

    // ��Translation�� �ִ� ä�� �̸��顱 �� �ֻ��� ��带 ã�´�.
    for (const auto& BoneName : _TranslationNodes)
    {
        // �� Ʈ������ �� �̸��� aiNode*�� ã�´�
        const aiNode* Node = FindNodeByName(FindNodeByName, _RootNode, BoneName);
        if (!Node)
        {
            continue;
        }

        // �θ� ������ Ÿ�� �ö󰡸鼭, �θ� ä�� �ʿ� ���ϴ��� �˻�
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

        // �θ� ä���� ������ ���ֻ��� Translation ä�Ρ��� ����
        if (!hasParentChannel)
        {
            return BoneName;
        }
    }

    // �ĺ��� �ϳ��� ���ų�, ��� �θ� ������, �׳� ù ��°�� ��ȯ
    if (!_TranslationNodes.empty())
    {
        return _TranslationNodes.front();
    }

    return "";  // _TranslationNodes�� ��� ������ �� ���ڿ� ��ȯ
}

//��������������������������������������������������������������������������������������������������������������������������������������������������
// [5] CB_SkinnedMatrix ���·� FinalBoneMatrices ���� (���̴���)
//��������������������������������������������������������������������������������������������������������������������������������������������������
void Ext_Animator::RenderSkinnedMesh()
{
    // FinalBoneMatrices[i]�� �״�� XMMATRIX �����ڿ� �Ѱ� �ָ� �˴ϴ�
    for (size_t i = 0; i < FinalBoneMatrices.size() && i < MAX_BONES; ++i)
    {
        const aiMatrix4x4& m = FinalBoneMatrices[i];

        // ���� �켱(row-major)����� XMMATRIX ����
        DirectX::XMMATRIX xm = DirectX::XMMATRIX(
            m.a1, m.a2, m.a3, m.a4,
            m.b1, m.b2, m.b3, m.b4,
            m.c1, m.c2, m.c3, m.c4,
            m.d1, m.d2, m.d3, m.d4
        );

        // HLSL ���� row_major�� �б� ������, �߰� Transpose ���� �״�� �ѱ�ϴ�.
        CBMat.Bones[i] = xm;
    }

    // ������ ���� ������ķ� ä���
    for (size_t i = FinalBoneMatrices.size(); i < MAX_BONES; ++i)
    {
        CBMat.Bones[i] = DirectX::XMMatrixIdentity();
    }
}

// ��Ÿ Ÿ���� assimp ƽ ������ ��ȯ
float Ext_Animator::TimeInTicks(float _AccumulatedTime) const
{
    if (CurrentAnimation && CurrentAnimation->mTicksPerSecond != 0.0f)
    {
        return _AccumulatedTime * static_cast<float>(CurrentAnimation->mTicksPerSecond);
    }
    // ���� �� �ִϸ��̼��� ��TicksPerSecond = 0���̶��, �⺻ 25fps(ƽ)�� ����
    return _AccumulatedTime * 25.0f;
}

//��������������������������������������������������������������������������������������������������������������������������������������������������
// [F] �ִϸ��̼� Ű������ ���� �Լ���
//��������������������������������������������������������������������������������������������������������������������������������������������������
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
