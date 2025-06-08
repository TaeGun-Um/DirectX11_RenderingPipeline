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
	void LightViewSetting(size_t _Index); // ����Ʈ �����Ϳ� ��� ����

	// Getter, Setter
	LightType GetLightType() { return Type; } // ����Ʈ Ÿ�� ��������
	std::shared_ptr<LightData> GetLightData() { return LTData; } // ����Ʈ ���� ���� ��������
	std::shared_ptr<class Ext_DirectXRenderTarget> GetShadowRenderTarget() { return ShadowRenderTarget; } // �׸��� ����Ÿ�� ��������
	float4 GetShadowTextureScale() { float4(LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY, 0.f, 0.f); } // �׸��� ����Ÿ���� ���� ������ ��������
	void SetLightType(LightType _Type) { LTData->LightType = static_cast<int>(_Type); } // ����Ʈ Ÿ�� ����
	void SetLightColor(float4 _Color) { LTData->LightColor = _Color; } // ����Ʈ ���� ����
	void SetAttenuationValue(float _Value) { LTData->AttenuationValue = _Value; } // ����Ʈ ���� ��� ����(����Ʈ ����Ʈ��)
	void SetLightRange(float _Range) { LTData->FarDistance = _Range; }  // ����Ʈ ���� ����, Near�� 1.0 �����̰� Far�� ������

protected:
	void Start() override;
	
private:
	LightType Type = LightType::Unknown; // �ش� ����Ʈ�� ���� �������� ����
	std::shared_ptr<LightData> LTData = nullptr; // ����Ʈ ���꿡 �ʿ��� �����͵�

	std::shared_ptr<class Ext_DirectXRenderTarget> ShadowRenderTarget = nullptr; // ������ ������� �׸��� �׸���
	float4 ShadowRange = { 0.0f, 0.0f, 0.0f, 0.0f }; // �׸��� ����

};