#include "PrecompileHeader.h"
#include "ReflectionActor.h"

#include <DirectX11_Extension/Ext_ReflectionComponent.h>

void ReflectionActor::Start()
{
	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Sphere", MaterialType::StaticCUBE);
	MeshComp->SetTexture("Gray.png");
	MeshComp->ShadowOn(ShadowType::Static);

	GetTransform()->SetLocalScale({ 50.f, 50.f, 50.f });
}

void ReflectionActor::SetReflection()
{
	Reflection = Ext_ReflectionComponent::AttachTo(GetSharedFromThis<Ext_Actor>(), MeshComp);
}
