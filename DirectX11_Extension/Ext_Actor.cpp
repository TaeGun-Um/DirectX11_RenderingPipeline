#include "PrecompileHeader.h"
#include "Ext_Actor.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"

Ext_Actor::Ext_Actor()
{
	Transform = std::make_shared<Ext_Transform>();
}

Ext_Actor::~Ext_Actor()
{
}

// 컴포넌트 생성 시 자동호출(오너 액터 설정, 오너 신 설정, 이름 저장, 오더 저장)
void Ext_Actor::ComponentInitialize(std::shared_ptr<Ext_Component> _Component, std::weak_ptr<Ext_Actor> _Actor, std::string_view _Name, int _Order /*=0*/, bool __IsTransformShare /*= false*/)
{
	_Component->SetOwnerActor(_Actor);
	_Component->SetOwnerScene(GetOwnerScene());
	_Component->IsTransformShare = __IsTransformShare;
	_Component->SetName(_Name);
	_Component->SetOrder(_Order);
	_Component->Start();
}
