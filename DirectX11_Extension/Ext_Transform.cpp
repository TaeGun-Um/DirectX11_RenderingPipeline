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
    Quaternion = Rotation.EulerDegreeToQuaternion();
    WorldMatrix.Compose(Scale, Quaternion, Position);
}

void TransformData::SetViewProjectionMatrix(const float4x4& _View, const float4x4& _Projection)
{
    ViewMatirx = _View;
    ProjectionMatirx = _Projection;
    WorldViewMatirx = WorldMatrix * ViewMatirx;
    WorldViewProjectionMatrix = WorldMatrix * ViewMatirx * ProjectionMatirx;
}
