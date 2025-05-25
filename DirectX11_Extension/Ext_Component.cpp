#include "PrecompileHeader.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"
#include "Ext_Actor.h"

Ext_Component::Ext_Component()
{
	
}

Ext_Component::~Ext_Component()
{

}

void Ext_Component::Start()
{
	if (IsTransformShare)
	{
		Transform = OwnerActor.lock()->GetTransform();
	}
	else
	{
		Transform = std::make_shared<Ext_Transform>();
	}
}

void Ext_Component::Destroy()
{
	// [1] Transform 해제
	if (!IsTransformShare && Transform)
	{
		Transform->Destroy();  // 개별 Transform이면 정리
		Transform = nullptr;
	}
	else
	{
		// 공유일 경우는 Actor가 관리하므로 제거하지 않음
		Transform = nullptr;
	}

	// [2] Actor 연결 해제
	OwnerActor.reset();
	OwnerScene.reset();

	// [3] 추가적인 공통 자원 해제 필요 시 여기에
	// 예: 이벤트 핸들러 언바인딩 등
}
