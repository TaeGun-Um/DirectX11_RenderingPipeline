#include "PrecompileHeader.h"
#include "Ext_Transform.h"

/////////////////////////////////////// Ext_Transform ///////////////////////////////////////

Ext_Transform::Ext_Transform()
{
    TFData = std::make_shared<TransformData>();
    TFData->CalculateWorldMatrix();
}

Ext_Transform::~Ext_Transform()
{
}

void Ext_Transform::TransformUpdate()
{
    TFData->CalculateWorldMatrix();
}

/////////////////////////////////////// TransformData ///////////////////////////////////////

void TransformData::CalculateWorldMatrix()
{
    Quaternion = Rotation.DegreeToQuaternion();
    WorldMatrix.Compose(Scale, Quaternion, Position);
}

// ī�޶� �������� ����, ��, �������� ��� ����
void TransformData::SetViewProjectionMatrix(const float4x4& _View, const float4x4& _Projection)
{
    ViewMatrix = _View;
    ProjectionMatrix = _Projection;
    WorldViewMatrix = WorldMatrix * ViewMatrix;
    WorldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjectionMatrix;
}
