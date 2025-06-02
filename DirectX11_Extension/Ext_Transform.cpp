#include "PrecompileHeader.h"
#include "Ext_Transform.h"
#include "Ext_CollisionComponent.h"

/////////////////////////////////////// TransformData ///////////////////////////////////////

// 로컬 매트릭스 만들기
void TransformData::CalculateLocalMatrix()
{
	LocalQuaternion = LocalRotation.DegreeToQuaternion();
	LocalMatrix.Compose(LocalScale, LocalQuaternion, LocalPosition);
}

// 로컬 매트릭스로 월드 매트릭스 만들기
// 부모가 없다면 단위행렬을 곱하여 자기 자신의 로컬이 곧 월드 행렬이 되도록함
// 부모가 있다면 부모의 월드 행렬을 받아서 자신의 월드 행렬 계산 -> 부모 영향을 받는 월드 행렬 구성
void TransformData::CalculateWorldMatrix(const float4x4& _ParentMatrix)
{
	CalculateLocalMatrix();
	WorldMatrix = LocalMatrix * _ParentMatrix;
	WorldMatrix.Decompose(WorldScale, WorldQuaternion, WorldPosition);
	WorldRotation = WorldQuaternion.QuaternionToDegree();
}

// 뷰, 프로젝션 생성
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
	
	// 부모가 없으면 단위 행렬이 들어가고, 부모가 있으면 부모 행렬이 들어감
	TFData->CalculateWorldMatrix(ParentMatrix);

	//// 월드 위치, 회전 강제 덮어쓰기
	//if (bForceWorldPosition || bForceWorldRotation)
	//{
	//	// ✅ 강제 월드 위치 및 회전 정보 준비
	//	float4 WorldPosition = TFData->WorldPosition;
	//	float4 WorldRotation = TFData->WorldRotation;
	//	float4 WorldScale = TFData->WorldScale;
	//	float4 WorldQuaternion = TFData->WorldQuaternion;

	//	if (bForceWorldPosition)
	//	{
	//		WorldPosition = ForcedWorldPosition;
	//		TFData->WorldPosition = WorldPosition;
	//	}

	//	if (bForceWorldRotation)
	//	{
	//		WorldRotation = ForcedWorldRotation;
	//		WorldQuaternion = ForcedWorldRotation.DegreeToQuaternion();

	//		TFData->WorldRotation = WorldRotation;
	//		TFData->WorldQuaternion = WorldQuaternion;
	//	}

	//	// ✅ 새로 조합
	//	TFData->WorldMatrix.Compose(WorldScale, WorldQuaternion, WorldPosition);

	//	// ✅ 역산으로 LocalMatrix 계산
	//	ParentMatrix.Inverse();
	//	TFData->LocalMatrix = TFData->WorldMatrix * ParentMatrix;
	//	TFData->LocalMatrix.Decompose(TFData->LocalScale,	TFData->LocalQuaternion, TFData->LocalPosition);
	//	TFData->LocalRotation = TFData->LocalQuaternion.QuaternionToDegree();
	//}

	for (auto& Child : Children)
	{
		Child->TransformUpdate();
	}
}