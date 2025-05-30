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
	float4 LightColor = { 1.0f, 1.0f, 1.0f, 0.1f }; // RGB(��), w(����)
	
	float4 LightWorldPosition; // ����Ʈ ���� ��ġ(����)
	float4 LightForward; // ����Ʈ ���� ����

	float4 LightViewSpacePosition; // ����Ʈ �佺���̽�
	float4 LightViewSpaceForward; // ����Ʈ �佺���̽� ���� ����
	float4 LightViewSpaceBack; // ����Ʈ �佺���̽� �Ĺ� ����

	float LightNear = 1.0f;
	float LightFar = 1.0f;
	int LightType = 0;
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
	void SetLightType(LightType _Type) { LTData->LightType = static_cast<int>(_Type); };
	void SetLightRange(float _Range);
	std::shared_ptr<LightData> GetLightData() { return LTData; }

protected:
	
private:
	std::shared_ptr<LightData> LTData = nullptr;
	LightType Type = LightType::Unknown;

};