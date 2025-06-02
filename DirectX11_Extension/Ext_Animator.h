#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// GPU에 넘길 상수 버퍼 구조체
constexpr unsigned int MAX_BONES = 100;
struct CB_SkinnedMatrix
{
    float4x4 Bones[MAX_BONES];
};

// 애니메이션 재생 데이터
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

    void SetSkeltalMesh(std::string_view _MeshName); // 스켈레탈 메시 적용
    bool LoadAnimation(std::string_view _FilePath); // 애니메이션 생성

    bool SetAnimation(std::string_view _AnimName); // 재생할 애니메이션 선택
    void UpdateAnimation(float _AccumulatedTime); // 선택된 애니메이션 재생
    
    // Getter
    const CB_SkinnedMatrix& GetCBMat() { return CBMat; }

private:
    aiMatrix4x4 ReadNodeHierarchy(float _AccumulatedTime, const aiNode* _Node, const aiMatrix4x4& _ParentTransform); // 재귀적으로 노드 트리를 순회하여 FinalBoneMatrices에 값 적용
    void RenderSkinnedMesh(); // CB_SkinnedMatrix 값 반환
    float TimeInTicks(float _AccumulatedTime) const; // 델타 타임을 assimp 틱 단위로 변환
    std::string FindRootMotionNode(const aiNode* sceneRoot, const std::vector<std::string>& candidates); // 루트모션 노드 찾기

    // 애니메이션 채널에서 특정 시간(_AnimTimeInSeconds)에 해당하는
    // Position / Rotation / Scaling 키의 인덱스 및 보간을 처리하는 함수들
    void CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);

    std::shared_ptr<class Ext_SkeltalMesh> SkeletalMesh; // 스켈레톤 정보(assimp 정보 포함)
    std::map<std::string, std::shared_ptr<AnimationData>> AnimationDatas; // 애니메이션들
    const aiAnimation* CurrentAnimation = nullptr; // 현재 선택한 애니메이션
    std::string RootMotionBoneName; // 애니메이션 루트모션을 담은 노드 이름
    float AccumulatedTime = 0.f;

    // LoadAnimationFBX()를 호출할 때 채워지며, UpdateAnimation()에서 “본 이름 기준으로 aiNodeAnim* 해당 노드 정보를 찾아 TRS 보간”에 사용
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;
    std::vector<aiMatrix4x4> FinalBoneMatrices; // 애니메이션 적용 → Offset 계산 결과 저장 최종 스킨 행렬
    CB_SkinnedMatrix CBMat; // 상수버퍼에 사용될 값
    
};