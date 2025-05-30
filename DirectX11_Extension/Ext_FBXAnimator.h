#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

using namespace DirectX;

// GPU縑 剩望 鼻熱 幗ぷ 掘褻羹
constexpr unsigned int MAX_BONES = 100;
struct CB_SkinnedMatrix
{
    XMMATRIX Bones[MAX_BONES];
};

// ゎ模縑 噙衛湍 Ext_DirectXVertexData ④渦蒂 んлж撮蹂.
// (Ext_DirectXVertexData朝 Position, Normal, TexCoord, Color, BONEID(XMUINT4), WEIGHT(XMFLOAT4) 蛔擊 詹幗煎 陛餵棲棻.)
#include "Ext_DirectXVertexData.h"

class Ext_FBXAnimator
{
public:
    Ext_FBXAnimator();
    ~Ext_FBXAnimator();

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [1] T-Pose FBX(詭衛 + 夥檣萄 ん鍔 蝶騷溯驛) 煎萄
    //
    // - _TposeFilename: 詭衛 + 夥檣萄 ん鍔 蝶騷溯驛檜 л眷 菟橫 氈朝 FBX 唳煎
    // - _OutVertices, _OutIndices: 檜 等檜攪蒂 GPU 幗ぷ煎 螢溥憮 溶渦葭й 熱 氈紫煙 瓣錶 鄹棲棻.
    //   翕衛縑 頂睡瞳戲煎 ExtractBonesFromMesh()蒂 ��轎ж罹 BoneNameToInfo, BoneCount蒂 撲薑м棲棻.
    bool LoadMeshFBX(
        const std::string& _TposeFilename,
        std::vector<Ext_DirectXVertexData>& _OutVertices,
        std::vector<unsigned int>& _OutIndices);

    CB_SkinnedMatrix CB;

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [2] 擁棲詭檜暮 等檜攪虜 氬曹 FBX 煎萄
    //
    // - _AnimFilename: 螃霜 擁棲詭檜暮 瓣割(酈Щ溯歜 薑爾)虜 菟橫 氈朝 FBX 唳煎
    //   (詭衛 等檜攪朝 氈橫紫 鼻婦橈戲釭, 蝶騷溯驛/夥檣萄ん鍔 婦溼 薑爾虜 餌辨腎堅 詭衛 薑爾朝 鼠衛腌棲棻)
    // 
    // 頂睡瞳戲煎 AnimScene擊 檗橫憮 CurrentAnimation, BoneNameToAnimChannel擊 掘撩м棲棻.
    bool LoadAnimationFBX(const std::string& _AnimFilename);

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [3] (摹鷗瞳) 擁棲詭檜暮 贗董檜 FBX 寰縑 罹楝 偃 氈擊 唳辦 檣策蝶 雖薑
    //      晦獄高擎 羅 廓簞(anim index=0)煎 撮た腌棲棻.
    bool SetAnimation(unsigned int _AnimIndex);

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [4] 衙 Щ溯歜 ��轎: _TimeInSeconds(蟾 欽嬪) 晦遽戲煎 FinalBoneMatrices蒂 啗骯
    //
    // 頂睡瞳戲煎 ※MeshScene曖 瑞お 喻萄 お葬§蒂 牖�裔牉�,
    // LoadAnimationFBX()煎 煎萄脹 擁棲詭檜暮 瓣割菟檜 翱唸脹 獄菟縑 渠п憮虜
    // 夥檣萄 ん鍔 渠褐 擁棲詭檜暮 TRS蒂 爾除п憮 瞳辨м棲棻.
    void UpdateAnimation(float _TimeInSeconds);

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [5] 膠檜渦縑 剩啖還 譆謙 鄙渠 ч溺菟(夥檣萄 ん鍔 爾薑 ⊥ 擁棲詭檜暮 營敝 啗骯 ⊥ Offset) 奩��
    const std::vector<aiMatrix4x4>& GetFinalBoneMatrices() const { return FinalBoneMatrices; }

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [6] 溶渦 欽啗縑憮 夥煎 CB_SkinnedMatrix ⑽鷓煎 橢晦
    //     (鼻熱 幗ぷ縑 衙ёп憮 夥煎 GPU煎 爾鳥 熱 氈紫煙 м棲棻)
    CB_SkinnedMatrix RenderSkinnedMesh();

private:
    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // T-Pose FBX辨 Assimp
    Assimp::Importer MeshImporter;
    const aiScene* MeshScene = nullptr;

    // 擁棲詭檜暮 FBX辨 Assimp
    Assimp::Importer AnimImporter;
    const aiScene* AnimScene = nullptr;

    // ⑷營 摹鷗脹 擁棲詭檜暮(AnimScene->mAnimations[CurrentAnimIndex])
    unsigned int      CurrentAnimIndex = 0;
    const aiAnimation* CurrentAnimation = nullptr;

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [A] T-Pose FBX縑憮 蹺轎и 獄 薑爾
    struct BoneInfo
    {
        int          ID;            // 0睡攪 衛濛ж朝 獄 檣策蝶
        aiMatrix4x4  OffsetMatrix;  // aiBone->mOffsetMatrix: ※賅筐 蝶む檜蝶 ⊥ 獄 煎鏽 蝶む檜蝶§
    };
    // (1) 獄 檜葷 ⊥ BoneInfo (ID, OffsetMatrix) 衙ё
    std::unordered_map<std::string, BoneInfo> BoneNameToInfo;

    // (2) 獄 偃熱(Unique Bone Count)
    unsigned int BoneCount = 0;

    // (3) 譆謙 蝶鑑 ч溺(夥檣萄 爾薑 ⊥ 擁棲詭檜暮 瞳辨 ⊥ Offset 啗骯 唸婁)擊 盪濰ж朝 寡翮
    std::vector<aiMatrix4x4> FinalBoneMatrices;

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [B] 擁棲詭檜暮辨 瓣割 衙ё: 獄 檜葷 ⊥ aiNodeAnim*
    //
    // LoadAnimationFBX()蒂 ��轎й 陽 瓣錶雖貊, 
    // UpdateAnimation()縑憮 ※獄 檜葷 晦遽戲煎 aiNodeAnim* п渡 喻萄 薑爾蒂 瓊嬴 TRS 爾除§縑 餌辨м棲棻.
    std::unordered_map<std::string, const aiNodeAnim*> BoneNameToAnimChannel;

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [C] T-Pose FBX曖 ※瑞お 喻萄§ 滲�素� 羲ч溺
    //     ReadNodeHierarchy()縑憮 譆謙 獄 ч溺擊 掘й 陽 餌辨(夥檣萄 ん鍔 爾薑)
    aiMatrix4x4 GlobalInverseTransform;

private:
    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [D] T-Pose FBX縑憮 獄 薑爾蒂 熱餵(BoneCount, BoneNameToInfo)ж朝 л熱
    //     - 函 賅萇 詭衛(mesh) 寰曖 aiBone 檜葷擊 賅嬴憮 醞犒 薯剪
    //     - 刻 陝 aiBone曖 OffsetMatrix蒂 瓊嬴憮 BoneInfo 寡翮 掘撩
    //     - 券 BoneCount蒂 撮た
    void ExtractBonesFromMesh();

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [E] 營敝瞳戲煎 ※擁棲詭檜暮 酈Щ溯歜擊 瞳辨и TRS§ + parentTransform擊 鱔п
    //     陝 喻萄(獄)滌 譆謙 滲�� ч溺擊 掘ж堅, 
    //     虜擒 喻萄 檜葷檜 BoneNameToInfo縑 氈戲賊(=褒薯 獄檜塭賊) FinalBoneMatrices[boneID]縑 盪濰
    aiMatrix4x4 ReadNodeHierarchy(float _AnimTimeInSeconds, const aiNode* _Node, const aiMatrix4x4& _ParentTransform);

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [F] 擁棲詭檜暮 瓣割縑憮 か薑 衛除(_AnimTimeInSeconds)縑 п渡ж朝
    //     Position / Rotation / Scaling 酈曖 檣策蝶 塽 爾除擊 籀葬ж朝 л熱菟
    void CalcInterpolatedPosition(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    void CalcInterpolatedScaling(aiVector3D& _Out, float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindPositionIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindRotationIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);
    unsigned int FindScalingIndex(float _AnimTime, const aiNodeAnim* _NodeAnim);

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // [G] 蟾 欽嬪(_TimeInSeconds) 等檜攪蒂 Assimp 頂睡 す 欽嬪煎 夥脯輿朝 л熱
    float TimeInTicks(float _TimeInSeconds) const;

    // 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    // 蛤幗斜辨: aiMatrix4x4蒂 爾晦 謠啪 轎溘
    void PrintAiMatrix(const aiMatrix4x4& m, const char* name = "");

    // 蛤幗梵辨: DirectX::XMMATRIX蒂 爾晦 謠啪 轎溘
    void PrintXMMATRIX(const DirectX::XMMATRIX& xm, const char* name = "");

    
};