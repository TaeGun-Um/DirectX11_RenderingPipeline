#include "PrecompileHeader.h"
#include "Character.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_FBXMeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>
#include <DirectX11_Extension/Ext_Component.h>
#include <DirectX11_Base/Base_Math.h>
#include <DirectX11_Extension/Ext_FBXAnimator.h>

void Character::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 5 });
	GetTransform()->SetLocalScale({ 0.05f, 0.05f, 0.05f });

	MeshComp = CreateComponent<Ext_FBXMeshComponent>("BodyMesh");
	MeshComp->CreateMeshComponentUnit("Girl", "Animation");
	MeshComp->SetTexture("Ch03_1001_Diffuse.png", "BaseColor");

	//MeshComp2 = CreateComponent<Ext_MeshComponent>("BodyMesh");
	//MeshComp2->CreateMeshComponentUnit("Girl", "Basic");
	//MeshComp2->SetTexture("Ch03_1001_Diffuse.png", "BaseColor");
}

bool Switch = false;

void Character::Update(float _DeltaTime)
{
	AccTime += _DeltaTime;

	if (AccTime > 3.f)
	{
		AccTime = 0.f;
		Switch = !Switch;
		if (Switch)
		{
			MeshComp->GetAnimator()->SetAnimation(1);
		}
		else
		{
			MeshComp->GetAnimator()->SetAnimation(0);
		}
	}


	__super::Update(_DeltaTime);
}