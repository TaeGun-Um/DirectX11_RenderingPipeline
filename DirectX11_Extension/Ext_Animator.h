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

class Ext_Animator
{
public:
    Ext_Animator() {}
    ~Ext_Animator() {}

    void SetSkeltalMesh(std::string_view _MeshName); // ���̷�Ż �޽� ����
    bool LoadAnimation(std::string_view _FilePath); // �ִϸ��̼� ����
    bool SetAnimation(UINT _AnimIndex); // ����� �ִϸ��̼� ����
    bool ChangeAnimation(UINT _AnimIndex);
    void UpdateAnimation(float _TimeInSeconds); // ���õ� �ִϸ��̼� ���
    void RenderSkinnedMesh(); // CB_SkinnedMatrix �� ��ȯ

    // Getter
    const CB_SkinnedMatrix& GetCBMat() { return CBMat; }

private:
    // ��������� ��� Ʈ���� ��ȸ�Ͽ� FinalBoneMatrices�� �� ����
    aiMatrix4x4 ReadNodeHierarchy(float _AnimTimeInSeconds, const aiNode* _Node, const aiMatrix4x4& _ParentTransform);

    // �ִϸ��̼� ����� �ʿ��� assimp ����
    Assimp::Importer AnimImporter;
    const aiScene* AIAnimScene = nullptr;

    // ���̷��� ����(assimp ���� ����)
    std::shared_ptr<class Ext_SkeltalMesh> SkeletalMesh;

    CB_SkinnedMatrix CBMat; // ������۾� ���� ��
    std::vector<aiMatrix4x4> FinalBoneMatrices; // �ִϸ��̼� ���� �� Offset ��� ��� ���� ���� ��Ų ���


    // ���� ���õ� �ִϸ��̼�(AnimScene->mAnimations[CurrentAnimIndex])
    unsigned int      CurrentAnimIndex = 0;
    const aiAnimation* CurrentAnimation = nullptr;


    // ����������������������������������������������������������������������������������������������������������������������������������������������������������
    // [B] �ִϸ��̼ǿ� ä�� ����: �� �̸� �� aiNodeAnim*
    //
    // LoadAnimationFBX()�� ȣ���� �� ä������, 
    // UpdateAnimation()���� ���� �̸� �������� aiNodeAnim* �ش� ��� ������ ã�� TRS �������� ����մϴ�.
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;

private:


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