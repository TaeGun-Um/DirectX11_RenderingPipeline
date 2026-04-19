#include "PrecompileHeader.h"
#include "Ext_SpringArm.h"
#include "Ext_Actor.h"
#include "Ext_Transform.h"

void Ext_SpringArm::Start()
{
	// 초기 오너 위치 + pitch/yaw 기반 초기 소켓 위치/회전 계산
	bInitialized = false;
}

void Ext_SpringArm::AddRotationInput(float _dPitch, float _dYaw, float _dRoll /*= 0.0f*/)
{
	Pitch += _dPitch;
	Yaw   += _dYaw;
	Roll  += _dRoll;
}

void Ext_SpringArm::SetRotation(float _Pitch, float _Yaw, float _Roll /*= 0.0f*/)
{
	Pitch = _Pitch;
	Yaw   = _Yaw;
	Roll  = _Roll;
}

void Ext_SpringArm::Update(float _DeltaTime)
{
	Ext_Component::Update(_DeltaTime);

	// Pitch clamp
	Pitch = std::clamp(Pitch, PitchMin, PitchMax);

	// 오너 위치 획득
	std::shared_ptr<Ext_Actor> Owner = GetOwnerActor().lock();
	if (nullptr == Owner) return;

	const float4 PivotWorldPos = Owner->GetTransform()->GetWorldPosition() + TargetOffset;

	// 제어 회전에서 forward/right/up 유도
	const float4 ArmRot = { Pitch, Yaw, Roll, 0.0f };
	float4x4 RotMat;
	RotMat.RotationDegree(ArmRot);

	// 행렬 규약상 ArrVector[0]=Right, [1]=Up, [2]=Forward
	const float4 Right   = RotMat.ArrVector[0];
	const float4 Up      = RotMat.ArrVector[1];
	const float4 Forward = RotMat.ArrVector[2];

	// SocketOffset은 팔 끝 로컬 공간 → 월드로 회전 적용
	const float4 WorldOffset = Right * SocketOffset.x + Up * SocketOffset.y + Forward * SocketOffset.z;

	// 목표 소켓 위치 = 피벗 - Forward * ArmLength + 회전 적용된 오프셋
	const float4 DesiredPos = PivotWorldPos - Forward * TargetArmLength + WorldOffset;

	if (false == bInitialized)
	{
		SocketWorldPos = DesiredPos;
		SocketWorldRot = ArmRot;
		bInitialized = true;
		return;
	}

	if (true == bEnableLag)
	{
		float PosAlpha = std::min<float>(1.0f, _DeltaTime * CameraLagSpeed);
		float RotAlpha = std::min<float>(1.0f, _DeltaTime * RotationLagSpeed);

		// lerp
		SocketWorldPos = SocketWorldPos + (DesiredPos - SocketWorldPos) * PosAlpha;
		SocketWorldRot.x = SocketWorldRot.x + (ArmRot.x - SocketWorldRot.x) * RotAlpha;
		SocketWorldRot.y = SocketWorldRot.y + (ArmRot.y - SocketWorldRot.y) * RotAlpha;
		SocketWorldRot.z = SocketWorldRot.z + (ArmRot.z - SocketWorldRot.z) * RotAlpha;
	}
	else
	{
		SocketWorldPos = DesiredPos;
		SocketWorldRot = ArmRot;
	}
}
