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
    float4x4 Bones[MAX_BONES];
};

// ��ҿ� ���ô� Ext_DirectXVertexData ����� �����ϼ���.
// (Ext_DirectXVertexData�� Position, Normal, TexCoord, Color, BONEID(XMUINT4), WEIGHT(XMFLOAT4) ���� ����� �����ϴ�.)
#include "Ext_DirectXVertexData.h"

class Ext_FBXAnimator
{
public:
    Ext_FBXAnimator();
    ~Ext_FBXAnimator();

    CB_SkinnedMatrix CB;
    // (3) ���� ��Ų ���(���ε� ���� �� �ִϸ��̼� ���� �� Offset ��� ���)�� �����ϴ� �迭
    std::vector<aiMatrix4x4> FinalBoneMatrices;

    // [1] T-pose �޽� + ���ε� ���� �ε� �� ���������� BoneNameToInfo, BoneCount ����
    bool LoadMeshFBX(const std::string& _TposeFilename,
        std::vector<Ext_DirectXVertexData>& _OutVertices,
        std::vector<unsigned int>& _OutIndices);

    // [2] ���� �ִϸ��̼� FBX �ε� �� BoneNameToAnimChannel ����
    bool LoadAnimationFBX(const std::string& _AnimFilename);

    // [3] (����) ����� �ִϸ��̼� Ŭ�� �ε��� ����
    bool SetAnimation(unsigned int _AnimIndex);

    // [4] �� ������ ȣ��: (_TimeInSeconds) �������� FinalBoneMatrices ����
    void UpdateAnimation(float _TimeInSeconds);

    // [5] ���� ���� ����� CB_SkinnedMatrix ���·� ��ȯ
    CB_SkinnedMatrix RenderSkinnedMesh();

    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [5] ���̴��� �Ѱ��� ���� ���� ��ĵ�(���ε� ���� ���� �� �ִϸ��̼� ��� ��� �� Offset) ��ȯ
    const std::vector<aiMatrix4x4>& GetFinalBoneMatrices() const { return FinalBoneMatrices; }

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



    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [B] �ִϸ��̼ǿ� ä�� ����: �� �̸� �� aiNodeAnim*
    //
    // LoadAnimationFBX()�� ȣ���� �� ä������, 
    // UpdateAnimation()���� ���� �̸� �������� aiNodeAnim* �ش� ��� ������ ã�� TRS �������� ����մϴ�.
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;

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

    
    const aiNode* FindNodeByName(const aiNode* node, const std::string& name);


    aiMatrix4x4 GetGlobalTransform(const aiNode* node);
};