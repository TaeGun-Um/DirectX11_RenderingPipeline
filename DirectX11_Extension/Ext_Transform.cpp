#include "PrecompileHeader.h"
#include "Ext_Transform.h"

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

void TransformData::CalculateWorldMatrix()
{
    //float4x4 ScaleMatrix = float4x4::Scale(Scale);
    //float4x4 RotMatrix = float4x4::RotationEuler(Rotation); // 또는 Quaternion 방식
    //float4x4 TransMatrix = float4x4::Translation(Position);
    // return ScaleMatrix * RotMatrix * TransMatrix;

    Scale;
    Rotation;
    Position;

    // Euler angle
    Quaternion = Rotation.EulerDegreeToQuaternion();
    WorldMatrix.Compose(Scale, Quaternion, Position);
}
