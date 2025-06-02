#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// GPU�� �ѱ� ��� ���� ����ü
constexpr unsigned int MAX_BONES = 100;
struct CB_SkinnedMatrix
{
    float4x4 Bones[MAX_BONES];
};

// �ִϸ��̼� ��� ������
struct AnimationData
{
    Assimp::Importer AnimImporter;
    const aiScene* AIAnimScene = nullptr;
};

class Ext_Animator
{
public:
    Ext_Animator() {}
    ~Ext_Animator() {}

    void SetSkeltalMesh(std::string_view _MeshName); // ���̷�Ż �޽� ����
    bool LoadAnimation(std::string_view _FilePath); // �ִϸ��̼� ����

    bool SetAnimation(std::string_view _AnimName); // ����� �ִϸ��̼� ����
    void UpdateAnimation(float _AccumulatedTime); // ���õ� �ִϸ��̼� ���
    
    // Getter
    const CB_SkinnedMatrix& GetCBMat() { return CBMat; }

private:
    aiMatrix4x4 ReadNodeHierarchy(float _AccumulatedTime, const aiNode* _Node, const aiMatrix4x4& _ParentTransform); // ��������� ��� Ʈ���� ��ȸ�Ͽ� FinalBoneMatrices�� �� ����
    void RenderSkinnedMesh(); // CB_SkinnedMatrix �� ��ȯ
    float TimeInTicks(float _AccumulatedTime) const; // ��Ÿ Ÿ���� assimp ƽ ������ ��ȯ
    std::string FindRootMotionNode(const aiNode* sceneRoot, const std::vector<std::string>& candidates); // ��Ʈ��� ��� ã��

    // �ִϸ��̼� ä�ο��� Ư�� �ð�(_AnimTimeInSeconds)�� �ش��ϴ�
    // Position / Rotation / Scaling Ű�� �ε��� �� ������ ó���ϴ� �Լ���
    void CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);

    std::shared_ptr<class Ext_SkeltalMesh> SkeletalMesh; // ���̷��� ����(assimp ���� ����)
    std::map<std::string, std::shared_ptr<AnimationData>> AnimationDatas; // �ִϸ��̼ǵ�
    const aiAnimation* CurrentAnimation = nullptr; // ���� ������ �ִϸ��̼�
    std::string RootMotionBoneName; // �ִϸ��̼� ��Ʈ����� ���� ��� �̸�
    float AccumulatedTime = 0.f;

    // LoadAnimationFBX()�� ȣ���� �� ä������, UpdateAnimation()���� ���� �̸� �������� aiNodeAnim* �ش� ��� ������ ã�� TRS �������� ���
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;
    std::vector<aiMatrix4x4> FinalBoneMatrices; // �ִϸ��̼� ���� �� Offset ��� ��� ���� ���� ��Ų ���
    CB_SkinnedMatrix CBMat; // ������ۿ� ���� ��
    
};