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
    Quaternion = Rotation.EulerDegreeToQuaternion();
    WorldMatrix.Compose(Scale, Quaternion, Position);
}
