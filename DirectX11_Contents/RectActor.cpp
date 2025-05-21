#include "PrecompileHeader.h"
#include "RectActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>

RectActor::RectActor()
{
}

RectActor::~RectActor()
{
}

void RectActor::Start()
{
	Ext_Actor::Start();
	// CreateComponent<Ext_Component>("TestComp", true);
}

void RectActor::Update(float _DeltaTime)
{
	std::shared_ptr<Ext_Transform> Trans1 = GetTransform();
	
	std::shared_ptr<Ext_MeshComponent> Mesh1 = FindComponent<Ext_MeshComponent>("BasicMesh");
	std::shared_ptr<Ext_Transform> Trans2 = Mesh1->GetTransform();
}

void RectActor::Destroy()
{

}