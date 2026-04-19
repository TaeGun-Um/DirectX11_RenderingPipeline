#pragma once
#include "Ext_Actor.h"

enum class LightType
{
	Directional,
	Point,
	Spot,
	Unknown
};

struct LightData
{
	//float4 CameraWorldPosition; // ���� ���� ���� ��ġ
	//float4 CameraViewPosition; // ���� �� ���� ��ġ
	//float4 CameraForwardVector; // ���� ���� ����
	//float4 CameraViewForwardVector; // �� ���� ����

	//float LightNear = 1.0f;
	//float LightFar = 1.0f;
	//int LightType = 0;
	float4 LightColor = { 1.0f, 1.0f, 1.0f, 0.25f };// RGB(��), w(����)
	float4 LightWorldPosition; // ����Ʈ ���� ���� ��ġ
	float4 LightForwardVector; // ���� �Ի� ����
	float4 CameraWorldPosition; // ���� ���� ���� ��ġ

	float ShadowTargetSizeX;
	float ShadowTargetSizeY;

	float NearDistance = 1.0f;
	float FarDistance = 100.0f;
	float AttenuationValue = 1.0f; // �Ÿ����谪�� ���
	int LightType = 0;
	bool bIsLightSet = false;

	float4x4 LightViewMatrix;
	float4x4 LightViewInverseMatrix;
	float4x4 LightProjectionMatrix;
	float4x4 LightProjectionInverseMatrix;
	float4x4 LightViewProjectionMatrix;
	float4x4 CameraViewInverseMatrix;
};

constexpr unsigned int MAX_LIGHTS = 64;
struct LightDatas
{
	int LightCount = 0;
	LightData Lights[MAX_LIGHTS];
};

// -------------------------------------------------------------------------
//  HLSL ↔ C++ 패킹 검증 (컴파일 타임)
// -------------------------------------------------------------------------
//  HLSL 레이아웃 (Shader/LightData.fx):
//    LightData {
//      float4   LightColor/WorldPos/Forward/CameraWorldPos     = 4 × 16 = 64
//      float    Shadow{X/Y}, Near, Far, Atten                  = 5 × 4  = 20
//      int      LightType                                      = 4
//      bool     bIsLightSet                                    = 4   (HLSL bool 은 4 bytes)
//      // padding to 16 → next matrix boundary                 = 4
//      float4x4 View/ViewInv/Proj/ProjInv/ViewProj/CamViewInv  = 6 × 64 = 384
//    } = 480
//
//    LightDatas {
//      int LightCount                                          = 4
//      // padding to 16                                        = 12
//      LightData Lights[64]                                    = 64 × 480 = 30720
//    } = 30736
//
//  C++ 측에서는 `bool` 이 1 byte 지만 XMMATRIX 의 16-byte 정렬 요구로 인해
//  구조체 레이아웃이 동일하게 맞춰진다 (컴파일러가 자동 padding 삽입).
static_assert(sizeof(LightData) == 480,
    "LightData size mismatch — HLSL LightData 레이아웃과 일치하지 않음 (예상 480)");
static_assert(sizeof(LightData) % 16 == 0,
    "LightData must be 16-byte aligned (D3D11 cbuffer 규격)");
static_assert(sizeof(LightDatas) == 4 + 12 + 480 * MAX_LIGHTS,
    "LightDatas size mismatch — HLSL cbuffer(b2) 레이아웃과 일치하지 않음");

class Ext_Light : public Ext_Actor
{
public:
	// constrcuter destructer
	Ext_Light();
	~Ext_Light() {};

	// delete Function
	Ext_Light(const Ext_Light& _Other) = delete;
	Ext_Light(Ext_Light&& _Other) noexcept = delete;
	Ext_Light& operator=(const Ext_Light& _Other) = delete;
	Ext_Light& operator=(Ext_Light&& _Other) noexcept = delete;

	void LightUpdate(std::shared_ptr<class Ext_Camera> _Camera, float _DeltaTime); // ����Ʈ ������Ʈ ����(�������� ������Ʈ)

	// Getter, Setter
	LightType GetLightType() { return Type; } // ����Ʈ Ÿ�� ��������
	std::shared_ptr<LightData> GetLightData() { return LTData; } // ����Ʈ ���� ���� ��������
	std::shared_ptr<class Ext_DirectXRenderTarget> GetShadowRenderTarget() { return ShadowRenderTarget; } // �׸��� ����Ÿ�� ��������
	float4 GetShadowTextureScale() { float4(LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY, 0.f, 0.f); } // �׸��� ����Ÿ���� ���� ������ ��������

	void SetLightType(LightType _Type) { LTData->LightType = static_cast<int>(_Type); } // ����Ʈ Ÿ�� ����
	void SetLightColor(float4 _Color) { LTData->LightColor = _Color; } // ����Ʈ ���� ����
	void SetAttenuationValue(float _Value) { LTData->AttenuationValue = _Value; } // ����Ʈ ���� ��� ����(����Ʈ ����Ʈ��)
	void SetLightRange(float _Range) { LTData->FarDistance = _Range; }  // ����Ʈ ���� ����, Near�� 1.0 �����̰� Far�� ������

	void ViewPortUpdate(std::shared_ptr<class Ext_Camera> _Camera);

protected:
	void Start() override;
	
private:
	LightType Type = LightType::Unknown; // �ش� ����Ʈ�� ���� �������� ����
	std::shared_ptr<LightData> LTData = nullptr; // ����Ʈ ���꿡 �ʿ��� �����͵�

	std::shared_ptr<class Ext_DirectXRenderTarget> ShadowRenderTarget = nullptr; // ������ ������� �׸��� �׸���
	float4 ShadowRange = { 0.0f, 0.0f, 0.0f, 0.0f }; // �׸��� ����

};