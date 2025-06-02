#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

using namespace DirectX;

// GPU¿¡ ³Ñ±æ »ó¼ö ¹öÆÛ ±¸Á¶Ã¼
constexpr unsigned int MAX_BONES = 100;
struct CB_SkinnedMatrix
{
    float4x4 Bones[MAX_BONES];
};

// Æò¼Ò¿¡ ¾²½Ã´ø Ext_DirectXVertexData Çì´õ¸¦ Æ÷ÇÔÇÏ¼¼¿ä.
// (Ext_DirectXVertexData´Â Position, Normal, TexCoord, Color, BONEID(XMUINT4), WEIGHT(XMFLOAT4) µîÀ» ¸â¹ö·Î °¡Áý´Ï´Ù.)
#include "Ext_DirectXVertexData.h"

class Ext_FBXAnimator
{
public:
    Ext_FBXAnimator();
    ~Ext_FBXAnimator();

    CB_SkinnedMatrix CB;
    // (3) ÃÖÁ¾ ½ºÅ² Çà·Ä(¹ÙÀÎµå º¸Á¤ ¡æ ¾Ö´Ï¸ÞÀÌ¼Ç Àû¿ë ¡æ Offset °è»ê °á°ú)À» ÀúÀåÇÏ´Â ¹è¿­
    std::vector<aiMatrix4x4> FinalBoneMatrices;

    // [1] T-pose ¸Þ½Ã + ¹ÙÀÎµå Æ÷Áî ·Îµå ¡æ ³»ºÎÀûÀ¸·Î BoneNameToInfo, BoneCount ¼¼ÆÃ
    bool LoadMeshFBX(const std::string& _TposeFilename,
        std::vector<Ext_DirectXVertexData>& _OutVertices,
        std::vector<unsigned int>& _OutIndices);

    // [2] ¼ø¼ö ¾Ö´Ï¸ÞÀÌ¼Ç FBX ·Îµå ¡æ BoneNameToAnimChannel ¼¼ÆÃ
    bool LoadAnimationFBX(const std::string& _AnimFilename);

    // [3] (¼±ÅÃ) Àç»ýÇÒ ¾Ö´Ï¸ÞÀÌ¼Ç Å¬¸³ ÀÎµ¦½º º¯°æ
    bool SetAnimation(unsigned int _AnimIndex);

    // [4] ¸Å ÇÁ·¹ÀÓ È£Ãâ: (_TimeInSeconds) ±âÁØÀ¸·Î FinalBoneMatrices °»½Å
    void UpdateAnimation(float _TimeInSeconds);

    // [5] ÃÖÁ¾ »À´ë Çà·ÄÀ» CB_SkinnedMatrix ÇüÅÂ·Î ¹ÝÈ¯
    CB_SkinnedMatrix RenderSkinnedMesh();

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // [5] ½¦ÀÌ´õ¿¡ ³Ñ°ÜÁÙ ÃÖÁ¾ »À´ë Çà·Äµé(¹ÙÀÎµå Æ÷Áî º¸Á¤ ¡æ ¾Ö´Ï¸ÞÀÌ¼Ç Àç±Í °è»ê ¡æ Offset) ¹ÝÈ¯
    const std::vector<aiMatrix4x4>& GetFinalBoneMatrices() const { return FinalBoneMatrices; }

private:
    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // T-Pose FBX¿ë Assimp
    Assimp::Importer MeshImporter;
    const aiScene* MeshScene = nullptr;

    // ¾Ö´Ï¸ÞÀÌ¼Ç FBX¿ë Assimp
    Assimp::Importer AnimImporter;
    const aiScene* AnimScene = nullptr;

    // ÇöÀç ¼±ÅÃµÈ ¾Ö´Ï¸ÞÀÌ¼Ç(AnimScene->mAnimations[CurrentAnimIndex])
    unsigned int      CurrentAnimIndex = 0;
    const aiAnimation* CurrentAnimation = nullptr;

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // [A] T-Pose FBX¿¡¼­ ÃßÃâÇÑ º» Á¤º¸
    struct BoneInfo
    {
        int          ID;            // 0ºÎÅÍ ½ÃÀÛÇÏ´Â º» ÀÎµ¦½º
        aiMatrix4x4  OffsetMatrix;  // aiBone->mOffsetMatrix: ¡°¸ðµ¨ ½ºÆäÀÌ½º ¡æ º» ·ÎÄÃ ½ºÆäÀÌ½º¡±
    };
    // (1) º» ÀÌ¸§ ¡æ BoneInfo (ID, OffsetMatrix) ¸ÅÇÎ
    std::unordered_map<std::string, BoneInfo> BoneNameToInfo;

    // (2) º» °³¼ö(Unique Bone Count)
    unsigned int BoneCount = 0;



    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // [B] ¾Ö´Ï¸ÞÀÌ¼Ç¿ë Ã¤³Î ¸ÅÇÎ: º» ÀÌ¸§ ¡æ aiNodeAnim*
    //
    // LoadAnimationFBX()¸¦ È£ÃâÇÒ ¶§ Ã¤¿öÁö¸ç, 
    // UpdateAnimation()¿¡¼­ ¡°º» ÀÌ¸§ ±âÁØÀ¸·Î aiNodeAnim* ÇØ´ç ³ëµå Á¤º¸¸¦ Ã£¾Æ TRS º¸°£¡±¿¡ »ç¿ëÇÕ´Ï´Ù.
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;

private:
    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // [D] T-Pose FBX¿¡¼­ º» Á¤º¸¸¦ ¼öÁý(BoneCount, BoneNameToInfo)ÇÏ´Â ÇÔ¼ö
    //     - ¨ç ¸ðµç ¸Þ½Ã(mesh) ¾ÈÀÇ aiBone ÀÌ¸§À» ¸ð¾Æ¼­ Áßº¹ Á¦°Å
    //     - ¨è °¢ aiBoneÀÇ OffsetMatrix¸¦ Ã£¾Æ¼­ BoneInfo ¹è¿­ ±¸¼º
    //     - ¨é BoneCount¸¦ ¼¼ÆÃ
    void ExtractBonesFromMesh();

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // [E] Àç±ÍÀûÀ¸·Î ¡°¾Ö´Ï¸ÞÀÌ¼Ç Å°ÇÁ·¹ÀÓÀ» Àû¿ëÇÑ TRS¡± + parentTransformÀ» ÅëÇØ
    //     °¢ ³ëµå(º»)º° ÃÖÁ¾ º¯È¯ Çà·ÄÀ» ±¸ÇÏ°í, 
    //     ¸¸¾à ³ëµå ÀÌ¸§ÀÌ BoneNameToInfo¿¡ ÀÖÀ¸¸é(=½ÇÁ¦ º»ÀÌ¶ó¸é) FinalBoneMatrices[boneID]¿¡ ÀúÀå
    aiMatrix4x4 ReadNodeHierarchy(float _AnimTimeInSeconds, const aiNode* _Node, const aiMatrix4x4& _ParentTransform);

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // [F] ¾Ö´Ï¸ÞÀÌ¼Ç Ã¤³Î¿¡¼­ Æ¯Á¤ ½Ã°£(_AnimTimeInSeconds)¿¡ ÇØ´çÇÏ´Â
    //     Position / Rotation / Scaling Å°ÀÇ ÀÎµ¦½º ¹× º¸°£À» Ã³¸®ÇÏ´Â ÇÔ¼öµé
    void CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // [G] ÃÊ ´ÜÀ§(_TimeInSeconds) µ¥ÀÌÅÍ¸¦ Assimp ³»ºÎ Æ½ ´ÜÀ§·Î ¹Ù²ãÁÖ´Â ÇÔ¼ö
    float TimeInTicks(float _TimeInSeconds) const;

    // ¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡¦¡
    // µð¹ö±×¿ë: aiMatrix4x4¸¦ º¸±â ÁÁ°Ô Ãâ·Â
    void PrintAiMatrix(const aiMatrix4x4& m, const char* name = "");

    // µð¹ö±ë¿ë: DirectX::XMMATRIX¸¦ º¸±â ÁÁ°Ô Ãâ·Â
    void PrintXMMATRIX(const DirectX::XMMATRIX& xm, const char* name = "");

    
    const aiNode* FindNodeByName(const aiNode* node, const std::string& name);


    aiMatrix4x4 GetGlobalTransform(const aiNode* node);
};