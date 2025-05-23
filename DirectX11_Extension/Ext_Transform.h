#pragma once

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
	
	//float4 LocalScale;
	//float4 LocalRotation;
	//float4 LocalQuaternion;
	//float4 LocalPosition;

	//float4 WorldScale;
	//float4 WorldRotation;
	//float4 WorldQuaternion;
	//float4 WorldPosition;

	//float4x4 LocalWorldMatrix;
	

	//void WorldCalculation(const TransformData& _Parent, bool AbsoluteScale, bool AbsoluteRotation, bool AbsolutePosition);
	//void LocalCalculation();
	//void SetViewAndProjection(const float4x4& _View, const float4x4& _Projection);
	
	void CalculateWorldMatrix();
	void SetViewProjectionMatrix(const float4x4& _View, const float4x4& _Projection);
};

// ������ ������ ���� Ŭ����, World�� Scene ��ü ��ī��Ʈ��ǥ�� ����, Local�� �ڱ� �ڽ� ����
class Ext_Transform
{
	friend class Ext_Camera;

public:
	// constrcuter destructer
	Ext_Transform();
	~Ext_Transform();

	// delete Function
	Ext_Transform(const Ext_Transform& _Other) = delete;
	Ext_Transform(Ext_Transform&& _Other) noexcept = delete;
	Ext_Transform& operator=(const Ext_Transform& _Other) = delete;
	Ext_Transform& operator=(Ext_Transform&& _Other) noexcept = delete;

	void SetWorldScale(const float4& _Value) // ���� ũ�� ����
	{
		TFData->Scale = _Value;
		TransformUpdate();
	}
	
	void SetWorldRotation(const float4& _Value) // ���� ���� ����
	{
		TFData->Rotation = _Value;
		TransformUpdate();
	}
	
	void SetWorldPosition(const float4& _Value) // ���� ��ġ ����
	{
		TFData->Position = _Value;
		TransformUpdate();
	}

	void AddWorldScale(const float4& _Value) { SetWorldScale(TFData->Scale + _Value); } // �Է°���ŭ ũ�� ����
	void AddWorldRotation(const float4& _Value) { SetWorldRotation(TFData->Rotation + _Value); } // �Է°���ŭ ȸ��
	void AddWorldPosition(const float4& _Value) { SetWorldPosition(TFData->Position + _Value); } // �Է°���ŭ �̵�

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

	void SetCameraMatrix(const float4x4& _View, const float4x4& _Projection) { TFData->SetViewProjectionMatrix(_View, _Projection); }

protected:
	
private:
	void TransformUpdate();
	

	std::list<std::shared_ptr<Ext_Transform>> Childs;
	std::shared_ptr<Ext_Transform> Parent = nullptr;
	std::shared_ptr<TransformData> TFData = nullptr;


};