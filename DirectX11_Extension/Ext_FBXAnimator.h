#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

using namespace DirectX;

// GPU�� �ѱ� ��� ���� ����ü
constexpr unsigned int MAX_BONES = 100;
struct CB_SkinnedMatrix
{
    XMMATRIX Bones[MAX_BONES];
};

// ��ҿ� ���ô� Ext_DirectXVertexData ����� �����ϼ���.
// (Ext_DirectXVertexData�� Position, Normal, TexCoord, Color, BONEID(XMUINT4), WEIGHT(XMFLOAT4) ���� ����� �����ϴ�.)
#include "Ext_DirectXVertexData.h"

class Ext_FBXAnimator
{
public:
    Ext_FBXAnimator();
    ~Ext_FBXAnimator();

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [1] T-Pose FBX(�޽� + ���ε� ���� ���̷���) �ε�
    //
    // - _TposeFilename: �޽� + ���ε� ���� ���̷����� �Բ� ��� �ִ� FBX ���
    // - _OutVertices, _OutIndices: �� �����͸� GPU ���۷� �÷��� �������� �� �ֵ��� ä�� �ݴϴ�.
    //   ���ÿ� ���������� ExtractBonesFromMesh()�� ȣ���Ͽ� BoneNameToInfo, BoneCount�� �����մϴ�.
    bool LoadMeshFBX(
        const std::string& _TposeFilename,
        std::vector<Ext_DirectXVertexData>& _OutVertices,
        std::vector<unsigned int>& _OutIndices);

    CB_SkinnedMatrix CB;

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [2] �ִϸ��̼� �����͸� ��� FBX �ε�
    //
    // - _AnimFilename: ���� �ִϸ��̼� ä��(Ű������ ����)�� ��� �ִ� FBX ���
    //   (�޽� �����ʹ� �־ ���������, ���̷���/���ε����� ���� ������ ���ǰ� �޽� ������ ���õ˴ϴ�)
    // 
    // ���������� AnimScene�� �о CurrentAnimation, BoneNameToAnimChannel�� �����մϴ�.
    bool LoadAnimationFBX(const std::string& _AnimFilename);

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [3] (������) �ִϸ��̼� Ŭ���� FBX �ȿ� ���� �� ���� ��� �ε��� ����
    //      �⺻���� ù ��°(anim index=0)�� ���õ˴ϴ�.
    bool SetAnimation(unsigned int _AnimIndex);

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [4] �� ������ ȣ��: _TimeInSeconds(�� ����) �������� FinalBoneMatrices�� ���
    //
    // ���������� ��MeshScene�� ��Ʈ ��� Ʈ������ ��ȸ�ϸ�,
    // LoadAnimationFBX()�� �ε�� �ִϸ��̼� ä�ε��� ����� ���鿡 ���ؼ���
    // ���ε� ���� ��� �ִϸ��̼� TRS�� �����ؼ� �����մϴ�.
    void UpdateAnimation(float _TimeInSeconds);

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [5] ���̴��� �Ѱ��� ���� ���� ��ĵ�(���ε� ���� ���� �� �ִϸ��̼� ��� ��� �� Offset) ��ȯ
    const std::vector<aiMatrix4x4>& GetFinalBoneMatrices() const { return FinalBoneMatrices; }

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [6] ���� �ܰ迡�� �ٷ� CB_SkinnedMatrix ���·� ���
    //     (��� ���ۿ� �����ؼ� �ٷ� GPU�� ���� �� �ֵ��� �մϴ�)
    CB_SkinnedMatrix RenderSkinnedMesh();

private:
    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // T-Pose FBX�� Assimp
    Assimp::Importer MeshImporter;
    const aiScene* MeshScene = nullptr;

    // �ִϸ��̼� FBX�� Assimp
    Assimp::Importer AnimImporter;
    const aiScene* AnimScene = nullptr;

    // ���� ���õ� �ִϸ��̼�(AnimScene->mAnimations[CurrentAnimIndex])
    unsigned int      CurrentAnimIndex = 0;
    const aiAnimation* CurrentAnimation = nullptr;

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [A] T-Pose FBX���� ������ �� ����
    struct BoneInfo
    {
        int          ID;            // 0���� �����ϴ� �� �ε���
        aiMatrix4x4  OffsetMatrix;  // aiBone->mOffsetMatrix: ���� �����̽� �� �� ���� �����̽���
    };
    // (1) �� �̸� �� BoneInfo (ID, OffsetMatrix) ����
    std::unordered_map<std::string, BoneInfo> BoneNameToInfo;

    // (2) �� ����(Unique Bone Count)
    unsigned int BoneCount = 0;

    // (3) ���� ��Ų ���(���ε� ���� �� �ִϸ��̼� ���� �� Offset ��� ���)�� �����ϴ� �迭
    std::vector<aiMatrix4x4> FinalBoneMatrices;

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [B] �ִϸ��̼ǿ� ä�� ����: �� �̸� �� aiNodeAnim*
    //
    // LoadAnimationFBX()�� ȣ���� �� ä������, 
    // UpdateAnimation()���� ���� �̸� �������� aiNodeAnim* �ش� ��� ������ ã�� TRS �������� ����մϴ�.
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [C] T-Pose FBX�� ����Ʈ ��塱 ��ȯ�� �����
    //     ReadNodeHierarchy()���� ���� �� ����� ���� �� ���(���ε� ���� ����)
    aiMatrix4x4 GlobalInverseTransform;

private:
    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [D] T-Pose FBX���� �� ������ ����(BoneCount, BoneNameToInfo)�ϴ� �Լ�
    //     - �� ��� �޽�(mesh) ���� aiBone �̸��� ��Ƽ� �ߺ� ����
    //     - �� �� aiBone�� OffsetMatrix�� ã�Ƽ� BoneInfo �迭 ����
    //     - �� BoneCount�� ����
    void ExtractBonesFromMesh();

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [E] ��������� ���ִϸ��̼� Ű�������� ������ TRS�� + parentTransform�� ����
    //     �� ���(��)�� ���� ��ȯ ����� ���ϰ�, 
    //     ���� ��� �̸��� BoneNameToInfo�� ������(=���� ���̶��) FinalBoneMatrices[boneID]�� ����
    aiMatrix4x4 ReadNodeHierarchy(float _AnimTimeInSeconds, const aiNode* _Node, const aiMatrix4x4& _ParentTransform);

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [F] �ִϸ��̼� ä�ο��� Ư�� �ð�(_AnimTimeInSeconds)�� �ش��ϴ�
    //     Position / Rotation / Scaling Ű�� �ε��� �� ������ ó���ϴ� �Լ���
    void CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [G] �� ����(_TimeInSeconds) �����͸� Assimp ���� ƽ ������ �ٲ��ִ� �Լ�
    float TimeInTicks(float _TimeInSeconds) const;

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // ����׿�: aiMatrix4x4�� ���� ���� ���
    void PrintAiMatrix(const aiMatrix4x4& m, const char* name = "");

    // ������: DirectX::XMMATRIX�� ���� ���� ���
    void PrintXMMATRIX(const DirectX::XMMATRIX& xm, const char* name = "");

    
};