#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

using namespace DirectX;

// GPU에 넘길 상수 버퍼 구조체
constexpr unsigned int MAX_BONES = 100;
struct CB_SkinnedMatrix
{
    float4x4 Bones[MAX_BONES];
};

// 평소에 쓰시던 Ext_DirectXVertexData 헤더를 포함하세요.
// (Ext_DirectXVertexData는 Position, Normal, TexCoord, Color, BONEID(XMUINT4), WEIGHT(XMFLOAT4) 등을 멤버로 가집니다.)
#include "Ext_DirectXVertexData.h"

class Ext_Animator
{
public:
    Ext_Animator() {}
    ~Ext_Animator() {}

    void SetSkeltalMesh(std::string_view _MeshName); // 스켈레탈 메시 적용
    bool LoadAnimation(std::string_view _FilePath); // 애니메이션 생성
    bool SetAnimation(UINT _AnimIndex); // 재생할 애니메이션 선택
    bool ChangeAnimation(UINT _AnimIndex);
    void UpdateAnimation(float _TimeInSeconds); // 선택된 애니메이션 재생
    void RenderSkinnedMesh(); // CB_SkinnedMatrix 값 반환

    // Getter
    const CB_SkinnedMatrix& GetCBMat() { return CBMat; }

private:
    // 재귀적으로 노드 트리를 순회하여 FinalBoneMatrices에 값 적용
    aiMatrix4x4 ReadNodeHierarchy(float _AnimTimeInSeconds, const aiNode* _Node, const aiMatrix4x4& _ParentTransform);

    // 애니메이션 재생에 필요한 assimp 정보
    Assimp::Importer AnimImporter;
    const aiScene* AIAnimScene = nullptr;

    // 스켈레톤 정보(assimp 정보 포함)
    std::shared_ptr<class Ext_SkeltalMesh> SkeletalMesh;

    CB_SkinnedMatrix CBMat; // 상수버퍼어 사용될 값
    std::vector<aiMatrix4x4> FinalBoneMatrices; // 애니메이션 적용 → Offset 계산 결과 저장 최종 스킨 행렬


    // 현재 선택된 애니메이션(AnimScene->mAnimations[CurrentAnimIndex])
    unsigned int      CurrentAnimIndex = 0;
    const aiAnimation* CurrentAnimation = nullptr;


    // ─────────────────────────────────────────────────────────────────────────────
    // [B] 애니메이션용 채널 매핑: 본 이름 → aiNodeAnim*
    //
    // LoadAnimationFBX()를 호출할 때 채워지며, 
    // UpdateAnimation()에서 “본 이름 기준으로 aiNodeAnim* 해당 노드 정보를 찾아 TRS 보간”에 사용합니다.
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;

private:


    // ─────────────────────────────────────────────────────────────────────────────
    // [F] 애니메이션 채널에서 특정 시간(_AnimTimeInSeconds)에 해당하는
    //     Position / Rotation / Scaling 키의 인덱스 및 보간을 처리하는 함수들
    void CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);

    // ─────────────────────────────────────────────────────────────────────────────
    // [G] 초 단위(_TimeInSeconds) 데이터를 Assimp 내부 틱 단위로 바꿔주는 함수
    float TimeInTicks(float _TimeInSeconds) const;

    // ─────────────────────────────────────────────────────────────────────────────
    // 디버그용: aiMatrix4x4를 보기 좋게 출력
    void PrintAiMatrix(const aiMatrix4x4& m, const char* name = "");

    // 디버깅용: DirectX::XMMATRIX를 보기 좋게 출력
    void PrintXMMATRIX(const DirectX::XMMATRIX& xm, const char* name = "");


    const aiNode* FindNodeByName(const aiNode* node, const std::string& name);


    aiMatrix4x4 GetGlobalTransform(const aiNode* node);


    
};