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

	float4 LocalScale;
	float4 LocalRotation;
	float4 LocalQuaternion;
	float4 LocalPosition;

	float4 WorldScale;
	float4 WorldRotation;
	float4 WorldQuaternion;
	float4 WorldPosition;

	float4x4 View;
	float4x4 Projection;
	float4x4 ViewPort;

	float4x4 ScaleMatrix;
	float4x4 RotationMatrix;
	float4x4 PositionMatrix;
	float4x4 LocalWorldMatrix;
	float4x4 WorldMatrix;
	float4x4 WorldView;
	float4x4 WorldViewProjectionMatrix;

	//void WorldCalculation(const TransformData& _Parent, bool AbsoluteScale, bool AbsoluteRotation, bool AbsolutePosition);
	//void LocalCalculation();
	//void SetViewAndProjection(const float4x4& _View, const float4x4& _Projection);

};

class Ext_Transform
{
public:
	// constrcuter destructer
	Ext_Transform();
	~Ext_Transform();

	// delete Function
	Ext_Transform(const Ext_Transform& _Other) = delete;
	Ext_Transform(Ext_Transform&& _Other) noexcept = delete;
	Ext_Transform& operator=(const Ext_Transform& _Other) = delete;
	Ext_Transform& operator=(Ext_Transform&& _Other) noexcept = delete;


protected:
	
private:
	void TransformUpdate();

	std::list<std::shared_ptr<Ext_Transform>> Childs;
	std::shared_ptr<Ext_Transform> Parent = nullptr;
	std::shared_ptr<TransformData> TransData;

};