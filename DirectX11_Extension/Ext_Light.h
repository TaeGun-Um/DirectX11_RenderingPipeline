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
	float4 LightColor = { 1.0f, 1.0f, 1.0f, 0.1f }; // RGB(색), w(강도)
	
	float4 LightWorldPosition; // 라이트 현재 위치(월드)
	float4 LightForward; // 라이트 전방 방향

	float4 LightViewSpacePosition; // 라이트 뷰스페이스
	float4 LightViewSpaceForward; // 라이트 뷰스페이스 전방 방향
	float4 LightViewSpaceBack; // 라이트 뷰스페이스 후방 빙향

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