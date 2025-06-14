﻿#include "PrecompileHeader.h"
#include "Ext_MeshComponent.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"
#include "Ext_MeshComponentUnit.h"
#include "Ext_Transform.h"
#include "Ext_Camera.h"

void Ext_MeshComponent::Release()
{
	// [1] 유닛 모두 정리
	Unit.reset();

	// [2] 카메라 연결 해제
	OwnerCamera.reset();

	// [3] 부모 클래스 Release 호출
	Ext_Component::Release();
}

void Ext_MeshComponent::Start()
{
	//GetTransform()->SetOwnerComponent(_Component);
	PushMeshToCamera("MainCamera");
}

// 각 메시들의 Rendering() 함수를 매틱 실행
void Ext_MeshComponent::Rendering(float _Deltatime, const float4x4& _ViewMatrix, const float4x4& _ProjectionMatrix)
{
	GetTransform()->SetCameraMatrix(_ViewMatrix, _ProjectionMatrix); // 뷰, 프로젝션 세팅
}

// 생성하면 카메라에도 저장하기 위해 실행
void Ext_MeshComponent::PushMeshToCamera(std::string_view _CameraName)
{
	GetOwnerScene().lock()->PushMeshToCamera(GetSharedFromThis<Ext_MeshComponent>(), _CameraName);
}

// 메시 컴포넌트에 필요한 유닛 생성 및 저장, 따로 설정하지 않으면 Basic Material로 고정
std::shared_ptr<Ext_MeshComponentUnit> Ext_MeshComponent::CreateMeshComponentUnit(std::string_view _MeshName, MaterialType _SettingValue)
{
	std::shared_ptr<Ext_MeshComponentUnit> NewUnit = std::make_shared<Ext_MeshComponentUnit>();
	NewUnit->SetOwnerMeshComponent(GetSharedFromThis<Ext_MeshComponent>());
	NewUnit->MeshComponentUnitInitialize(_MeshName, _SettingValue);
	Unit = NewUnit;

	return NewUnit;
}

// 메시 컴포넌트에 필요한 유닛 생성 및 저장, 따로 설정하지 않으면 Basic Material로 고정
std::shared_ptr<Ext_MeshComponentUnit> Ext_MeshComponent::CreateMeshComponentUnit(std::string_view _MeshName, std::string_view _MaterialName)
{
	std::shared_ptr<Ext_MeshComponentUnit> NewUnit = std::make_shared<Ext_MeshComponentUnit>();
	NewUnit->SetOwnerMeshComponent(GetSharedFromThis<Ext_MeshComponent>());
	NewUnit->MeshComponentUnitInitialize(_MeshName, _MaterialName);
	Unit = NewUnit;

	return NewUnit;
}

// 텍스쳐 변경하기
void Ext_MeshComponent::SetTexture(std::string_view _TextureName, TextureType _TypeValue /*= TextureSlot::BaseColor*/)
{
	if (nullptr == Unit)
	{
		MsgAssert("유닛이 없어 텍스쳐 세팅이 불가능합니다.");
	}

	Unit->SetTexture(_TextureName, _TypeValue);
}

// 텍스쳐 변경하기
void Ext_MeshComponent::SetTexture(std::string_view _TextureName, std::string_view _SlotName)
{
	if (nullptr == Unit)
	{
		MsgAssert("유닛이 없어 텍스쳐 세팅이 불가능합니다.");
	}

	Unit->SetTexture(_TextureName, _SlotName);
}

void Ext_MeshComponent::SetSampler(SamplerType _TypeValue)
{
	Unit->SetSampler(_TypeValue);
}

void Ext_MeshComponent::ShadowOn(ShadowType _Type)
{
	switch (_Type)
	{
	case ShadowType::Static: Unit->StaticShadowOn(); break;
	case ShadowType::Dynamic: Unit->DynamicShadowOn(); break;
	case ShadowType::Unknown: MsgAssert("그람자가 뭔가 잘못됨"); break;
	}
}

void Ext_MeshComponent::ShadowOff()
{
	Unit->ShadowOff();
}

