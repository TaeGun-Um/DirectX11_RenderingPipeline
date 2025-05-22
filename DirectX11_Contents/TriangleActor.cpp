#include "PrecompileHeader.h"
#include "TriangleActor.h"

#include <DirectX11_Extension/Ext_Actor.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>

TriangleActor::TriangleActor()
{
}

TriangleActor::~TriangleActor()
{
}

void TriangleActor::Start()
{
	Ext_Actor::Start();
	CreateComponent<Ext_MeshComponent>("BasicMesh", true);
}

void TriangleActor::Update(float _DeltaTime)
{

}

void TriangleActor::Destroy()
{

}