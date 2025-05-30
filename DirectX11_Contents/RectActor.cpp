﻿#include "PrecompileHeader.h"
#include "RectActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

void RectActor::Start()
{
	GetTransform()->SetLocalPosition({ 0, 0, 2 });

	MeshComp = CreateComponent<Ext_MeshComponent>("BasicMesh");
	MeshComp->CreateMeshComponentUnit("Rect", "Basic");
	// MeshComp->SetTexture("Cuphead.png");
	// MeshComp->SetTexture("Cuphead.png");

	MeshComp->GetTransform()->SetLocalPosition({ 0, 0, 0 });
}

void RectActor::Update(float _DeltaTime)
{
	__super::Update(_DeltaTime);

	float MoveSpeed = 100.0f; // 초당 100 단위 회전
	MeshComp->GetTransform()->AddLocalRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
}
