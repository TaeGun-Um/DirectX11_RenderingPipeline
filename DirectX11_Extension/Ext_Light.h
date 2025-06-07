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
	//float4 LightColor = { 1.0f, 1.0f, 1.0f, 0.25f }; // RGB(��), w(����)
	//
	//float4 LightWorldPosition; // ����Ʈ ���� ���� ��ġ
	//float4 LightViewPosition; // ����Ʈ �� ���� ��ġ
	//float4 LightForwardVector; // ���� �Ի� ����
	//float4 LightBackwordVector; // ���� �Ի� ���� �ݴ� ����
	//float4 LightViewForwardVector; // �� �Ի� ����

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
};

constexpr unsigned int MAX_LIGHTS = 64;
struct LightDatas
{
	int LightCount = 0;
	LightData Lights[MAX_LIGHTS];
};

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

	void LightUpdate(std::shared_ptr<class Ext_Camera> _Camera, float _DeltaTime);

	LightType GetLightType() { return Type; }
	void SetLightType(LightType _Type) { LTData->LightType = static_cast<int>(_Type); }
	void SetLightColor(float4 _Color) { LTData->LightColor = _Color; }
	void SetAttenuationValue(float _Value) { LTData->AttenuationValue = _Value; }
	void SetLightRange(float _Range)
	{
		LTData->NearDistance = 1.0f;
		LTData->FarDistance = _Range;
	}

	std::shared_ptr<LightData> GetLightData() { return LTData; }
	std::shared_ptr<class Ext_DirectXRenderTarget> GetShadowTarget() { return ShadowTarget; }
	float4 GetShadowTextureScale() { float4(LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY, 0.f, 0.f); }

protected:
	void Start() override;
	
private:
	std::shared_ptr<LightData> LTData = nullptr;
	LightType Type = LightType::Unknown;

	std::shared_ptr<class Ext_DirectXRenderTarget> ShadowRenderTarget = nullptr;

	void CreateShadowTarget(std::shared_ptr<class Ext_DirectXRenderTarget> _Target, LightType _Type);
};