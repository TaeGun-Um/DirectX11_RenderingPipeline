#include "PrecompileHeader.h"
#include "RectActor.h"

#include <DirectX11_Extension/Ext_MeshComponent.h>
#include <DirectX11_Extension/Ext_Transform.h>

RectActor::RectActor()
{
}

RectActor::~RectActor()
{
}

void RectActor::Start()
{

}

void RectActor::Update(float _DeltaTime)
{
	std::shared_ptr<Ext_Transform> Transform = GetTransform();

	float MoveSpeed = 100.0f; // 초당 100 단위 회전
	GetTransform()->AddWorldRotation({ 0.f, MoveSpeed * _DeltaTime, 0.f });
}

void RectActor::Destroy()
{

}