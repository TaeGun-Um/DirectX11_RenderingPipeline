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
	//float4 LightColor = { 1.0f, 1.0f, 1.0f, 0.25f }; // RGB(색), w(강도)
	//
	//float4 LightWorldPosition; // 라이트 월드 공간 위치
	//float4 LightViewPosition; // 라이트 뷰 공간 위치
	//float4 LightForwardVector; // 월드 입사 벡터
	//float4 LightBackwordVector; // 월드 입사 벡터 반대 방향
	//float4 LightViewForwardVector; // 뷰 입사 벡터

	//float4 CameraWorldPosition; // 시점 월드 공간 위치
	//float4 CameraViewPosition; // 시점 뷰 공간 위치
	//float4 CameraForwardVector; // 월드 시점 벡터
	//float4 CameraViewForwardVector; // 뷰 시점 벡터

	//float LightNear = 1.0f;
	//float LightFar = 1.0f;
	//int LightType = 0;
	float4 LightColor = { 1.0f, 1.0f, 1.0f, 0.25f };// RGB(색), w(강도)
	float4 LightWorldPosition; // 라이트 월드 공간 위치
	float4 LightForwardVector; // 월드 입사 벡터
	float4 CameraWorldPosition; // 시점 월드 공간 위치

	float NearDistance = 1.0f;
	float FarDistance = 100.0f;
	float AttenuationValue = 1.0f; // 거리감쇠값에 사용
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

	void SetLightType(LightType _Type) { LTData->LightType = static_cast<int>(_Type); };
	void SetLightColor(float4 _Color) { LTData->LightColor = _Color; };
	void SetAttenuationValue(float _Value) { LTData->AttenuationValue = _Value; };
	void SetLightRange(float _Range)
	{
		LTData->NearDistance = 1.0f;
		LTData->FarDistance = _Range;
	}

	std::shared_ptr<LightData> GetLightData() { return LTData; }

protected:
	
private:
	std::shared_ptr<LightData> LTData = nullptr;
	LightType Type = LightType::Unknown;

};