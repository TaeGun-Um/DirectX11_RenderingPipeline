﻿#pragma once

// 행렬 정보(정점 셰이더랑 동일한 크기로 설정해야함)
struct TransformData
{
	TransformData()
	{
		Scale = float4::ONE;
		Rotation = float4::ZERONULL;
		Quaternion = float4::ZERONULL;
		Position = float4::ZERO;
	}

	float4 Scale;
	float4 Rotation;
	float4 Quaternion;
	float4 Position;

	float4x4 ScaleMatrix;
	float4x4 RotationMatrix;
	float4x4 PositionMatrix;
	float4x4 WorldMatrix;

	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 ViewPortMatrix;

	float4x4 WorldViewMatrix;
	float4x4 WorldViewProjectionMatrix;
	
	void CalculateWorldMatrix();
	void SetViewProjectionMatrix(const float4x4& _View, const float4x4& _Projection);
};

// 기하학 구조를 위한 클래스, World는 Scene 전체 데카르트좌표계 기준, Local은 자기 자신 기준
class Ext_Transform
{
	friend class Ext_Camera;
	friend class Ext_MeshComponent;

public:
	// constrcuter destructer
	Ext_Transform();
	~Ext_Transform();

	// delete Function
	Ext_Transform(const Ext_Transform& _Other) = delete;
	Ext_Transform(Ext_Transform&& _Other) noexcept = delete;
	Ext_Transform& operator=(const Ext_Transform& _Other) = delete;
	Ext_Transform& operator=(Ext_Transform&& _Other) noexcept = delete;

	void SetWorldScale(const float4& _Value) // 월드 크기 지정
	{
		TFData->Scale = _Value;
		TransformUpdate();
	}
	
	void SetWorldRotation(const float4& _Value) // 월드 각도 지정
	{
		TFData->Rotation = _Value;
		TransformUpdate();
	}
	
	void SetWorldPosition(const float4& _Value) // 월드 위치 지정
	{
		TFData->Position = _Value;
		TransformUpdate();
	}

	void AddWorldScale(const float4& _Value) { SetWorldScale(TFData->Scale + _Value); } // 입력값만큼 크기 변경
	void AddWorldRotation(const float4& _Value) { SetWorldRotation(TFData->Rotation + _Value); } // 입력값만큼 회전
	void AddWorldPosition(const float4& _Value) { SetWorldPosition(TFData->Position + _Value); } // 입력값만큼 이동

	std::shared_ptr<TransformData> GetTransformData() { return TFData; }
	float4x4 GetWorldMatrix() { return TFData->WorldMatrix; }
	float4 GetWorldPosition() { return TFData->Position; }
	float4 GetWorldScale() {	return TFData->Scale; }
	float4 GetWorldRotation() { return TFData->Rotation; }
	float4 GetWorldQuaternion() { return TFData->Quaternion; }

	// For Camera
	float4 GetLocalForwardVector() { return TFData->WorldMatrix.ArrVector[2].NormalizeReturn(); }
	float4 GetLocalUpVector() { return TFData->WorldMatrix.ArrVector[1].NormalizeReturn(); }
	float4 GetLocalRightVector() {	return TFData->WorldMatrix.ArrVector[0].NormalizeReturn(); }

protected:
	
private:
	void SetCameraMatrix(const float4x4& _View, const float4x4& _Projection) { TFData->SetViewProjectionMatrix(_View, _Projection); } // 행렬 월드, 뷰, 프로젝션 적용
	void TransformUpdate(); // 행렬 업데이트

	std::shared_ptr<TransformData> TFData = nullptr; // TransformData 정보

	std::shared_ptr<Ext_Transform> Parent = nullptr; // 부모 행렬(아직 안쓸듯)
	std::list<std::shared_ptr<Ext_Transform>> Childs; // 자식 행렬(아직 안쓸듯)

};