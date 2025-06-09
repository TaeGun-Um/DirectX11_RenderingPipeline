#include "PrecompileHeader.h"
#include "StoneWallActor.h"
#include <DirectX11_Extension/Ext_CollisionComponent.h>
#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void StoneWallActor::Start()
{
	WallBody = CreateComponent<Ext_CollisionComponent>("WallBody", static_cast<int>(CollisionGroup::Wall));
	WallBody->SetCollsionType(CollsionType::OBB3D);

	WallBodyMesh = CreateComponent<Ext_MeshComponent>("WallBodyMesh");
	//WallBodyMesh->CreateMeshComponentUnit("Rect", MaterialType::PBR);
	//WallBodyMesh->SetTexture("StoneWall_BaseColor.jpg", TextureType::BaseColor);
	//WallBodyMesh->SetTexture("StoneWall_Normal.jpg", TextureType::Normal);
	//WallBodyMesh->ShadowOn();
	WallBodyMesh->CreateMeshComponentUnit("Rect", MaterialType::Static);
	WallBodyMesh->SetTexture("Gray.png");
	WallBodyMesh->ShadowOn();

	GetTransform()->SetLocalScale({ 300.f, 300.f, 1.f });
}

void StoneWallActor::Update(float _DletaTime)
{
	__super::Update(_DletaTime);
}
