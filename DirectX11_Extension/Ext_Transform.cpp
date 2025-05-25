#include "PrecompileHeader.h"
#include "Ext_Transform.h"

void Ext_Transform::Destroy()
{
	// [1] TransformData 제거
	if (TFData)
	{
		TFData.reset();
	}

	// [2] 부모 관계 제거
	if (Parent)
	{
		Parent = nullptr;
	}

	// [3] 자식 리스트 제거
	Childs.clear();
}

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

// 카메라 기준으로 월드, 뷰, 프로젝션 행렬 생성
void TransformData::SetViewProjectionMatrix(const float4x4& _View, const float4x4& _Projection)
{
    ViewMatrix = _View;
    ProjectionMatrix = _Projection;
    WorldViewMatrix = WorldMatrix * ViewMatrix;
    WorldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjectionMatrix;
}
