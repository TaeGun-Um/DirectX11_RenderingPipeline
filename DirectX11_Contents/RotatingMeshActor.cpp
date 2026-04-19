#include "PrecompileHeader.h"
#include "RotatingMeshActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void RotatingMeshActor::Update(float _DeltaTime)
{
	Ext_Actor::Update(_DeltaTime);

	if (bIsRotation && nullptr != MeshComp)
	{
		const float Angle = RotationSpeed * _DeltaTime;
		MeshComp->GetTransform()->AddLocalRotation({
			RotationAxis.x * Angle,
			RotationAxis.y * Angle,
			RotationAxis.z * Angle,
			0.0f });
	}
}
