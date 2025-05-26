#include "PrecompileHeader.h"
#include "Ext_Transform.h"

/////////////////////////////////////// TransformData ///////////////////////////////////////

void TransformData::CalculateLocalMatrix()
{
	LocalQuaternion = LocalRotation.DegreeToQuaternion();
	LocalMatrix.Compose(LocalScale, LocalQuaternion, LocalPosition);
}

void TransformData::CalculateWorldMatrix(const float4x4& _ParentMatrix)
{
	CalculateLocalMatrix();
	WorldMatrix = LocalMatrix * _ParentMatrix;
	WorldMatrix.Decompose(WorldScale, WorldQuaternion, WorldPosition);
	WorldRotation = WorldQuaternion.QuaternionToDegree();
}

void TransformData::SetViewProjectionMatrix(const float4x4& _View, const float4x4& _Projection)
{
	ViewMatrix = _View;
	ProjectionMatrix = _Projection;
	WorldViewMatrix = WorldMatrix * ViewMatrix;
	WorldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjectionMatrix;
}

/////////////////////////////////////// Ext_Transform ///////////////////////////////////////

Ext_Transform::Ext_Transform()
{
    TFData = std::make_shared<TransformData>();
	TransformUpdate();
}

void Ext_Transform::Release()
{
	if (TFData)
	{
		TFData.reset();
	}
	if (auto ParentTransform = Parent.lock())
	{
		ParentTransform->RemoveChild(GetSharedFromThis<Ext_Transform>());
	}
	Parent.reset();
	Children.clear();
}

void Ext_Transform::TransformUpdate()
{
	float4x4 ParentMatrix;
	ParentMatrix.Identity();

	if (auto ParentTransform = Parent.lock())
	{
		ParentMatrix = ParentTransform->GetWorldMatrix();
	}

	TFData->CalculateWorldMatrix(ParentMatrix);

	for (auto& Child : Children)
	{
		Child->TransformUpdate();
	}
}