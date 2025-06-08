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

	float ShadowTargetSizeX;
	float ShadowTargetSizeY;

	float NearDistance = 1.0f;
	float FarDistance = 100.0f;
	float AttenuationValue = 1.0f; // 거리감쇠값에 사용
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

	void LightUpdate(std::shared_ptr<class Ext_Camera> _Camera, float _DeltaTime); // 라이트 업데이트 진행(연산정보 업데이트)
	void LightViewSetting(size_t _Index); // 라이트 데이터에 모두 전달

	// Getter, Setter
	LightType GetLightType() { return Type; } // 라이트 타입 가져오기
	std::shared_ptr<LightData> GetLightData() { return LTData; } // 라이트 연산 정보 가져오기
	std::shared_ptr<class Ext_DirectXRenderTarget> GetShadowRenderTarget() { return ShadowRenderTarget; } // 그림자 렌더타겟 가져오기
	float4 GetShadowTextureScale() { float4(LTData->ShadowTargetSizeX, LTData->ShadowTargetSizeY, 0.f, 0.f); } // 그림자 렌더타겟을 위한 사이즈 가져오기
	void SetLightType(LightType _Type) { LTData->LightType = static_cast<int>(_Type); } // 라이트 타입 세팅
	void SetLightColor(float4 _Color) { LTData->LightColor = _Color; } // 라이트 색깔 세팅
	void SetAttenuationValue(float _Value) { LTData->AttenuationValue = _Value; } // 라이트 감쇠 계수 설정(포인트 라이트용)
	void SetLightRange(float _Range) { LTData->FarDistance = _Range; }  // 라이트 범위 설정, Near는 1.0 고정이고 Far가 설정됨

protected:
	void Start() override;
	
private:
	LightType Type = LightType::Unknown; // 해당 라이트는 무슨 종류인지 설정
	std::shared_ptr<LightData> LTData = nullptr; // 라이트 연산에 필요한 데이터들

	std::shared_ptr<class Ext_DirectXRenderTarget> ShadowRenderTarget = nullptr; // 라이터 영향받은 그림자 그릴곳
	float4 ShadowRange = { 0.0f, 0.0f, 0.0f, 0.0f }; // 그림자 길이

};