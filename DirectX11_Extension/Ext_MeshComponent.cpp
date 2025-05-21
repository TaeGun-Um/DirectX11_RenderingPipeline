#include "PrecompileHeader.h"
#include "Ext_MeshComponent.h"
#include "Ext_Actor.h"

Ext_MeshComponent::Ext_MeshComponent()
{

}

Ext_MeshComponent::~Ext_MeshComponent()
{
}

void Ext_MeshComponent::Start()
{
	Transform = OwnerActor.lock()->GetTransform();
}
