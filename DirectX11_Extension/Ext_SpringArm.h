#pragma once
#include "Ext_Component.h"

// 언리얼 엔진 USpringArmComponent와 유사한 역할의 컴포넌트
// - 오너(Actor)의 위치를 기준 피벗으로 삼고, 지정 방향(pitch/yaw/roll)으로 TargetArmLength만큼 뒤로 카메라를 위치시킴
// - 선택적으로 위치/회전 Lag(감쇠) 지원
class Ext_SpringArm : public Ext_Component
{
public:
	// constrcuter destructer
	Ext_SpringArm() {}
	~Ext_SpringArm() {}

	// delete Function
	Ext_SpringArm(const Ext_SpringArm& _Other) = delete;
	Ext_SpringArm(Ext_SpringArm&& _Other) noexcept = delete;
	Ext_SpringArm& operator=(const Ext_SpringArm& _Other) = delete;
	Ext_SpringArm& operator=(Ext_SpringArm&& _Other) noexcept = delete;

	// ------ Setter ------
	void SetTargetArmLength(float _Length)       { TargetArmLength = _Length; }
	void SetTargetOffset(const float4& _Offset)  { TargetOffset = _Offset; }
	void SetSocketOffset(const float4& _Offset)  { SocketOffset = _Offset; }
	void SetEnableLag(bool _Enable)              { bEnableLag = _Enable; }
	void SetCameraLagSpeed(float _Speed)         { CameraLagSpeed = _Speed; }
	void SetRotationLagSpeed(float _Speed)       { RotationLagSpeed = _Speed; }
	void SetPitchClamp(float _Min, float _Max)   { PitchMin = _Min; PitchMax = _Max; }

	// ------ Rotation Control ------
	void AddRotationInput(float _dPitch, float _dYaw, float _dRoll = 0.0f);
	void SetRotation(float _Pitch, float _Yaw, float _Roll = 0.0f);
	float GetPitch() const { return Pitch; }
	float GetYaw()   const { return Yaw; }
	float GetRoll()  const { return Roll; }

	// ------ 결과 조회 (Update 이후 유효) ------
	const float4& GetSocketWorldPosition() const { return SocketWorldPos; }
	const float4& GetSocketWorldRotation() const { return SocketWorldRot; }

	// 파라미터
	float  TargetArmLength   = 300.0f;
	float4 TargetOffset      = float4::ZERO;   // 피벗 기준 오프셋 (월드 공간)
	float4 SocketOffset      = float4::ZERO;   // 카메라 위치 오프셋 (팔 끝 로컬 공간)
	bool   bEnableLag        = false;
	float  CameraLagSpeed    = 10.0f;
	float  RotationLagSpeed  = 10.0f;
	float  PitchMin          = -85.0f;
	float  PitchMax          = 85.0f;

protected:
	void Start() override;
	void Update(float _DeltaTime) override;

private:
	// 제어 회전 (오너와 독립적으로 카메라가 바라보는 방향)
	float Pitch = 0.0f;
	float Yaw   = 0.0f;
	float Roll  = 0.0f;

	// 계산 결과 (보간 후)
	float4 SocketWorldPos = float4::ZERO;
	float4 SocketWorldRot = float4::ZERO;
	bool   bInitialized   = false;
};
