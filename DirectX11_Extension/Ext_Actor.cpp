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

void Ext_Actor::SetComponentName(std::shared_ptr<Ext_Component> _Component, std::string_view _Name)
{
	_Component->SetName(_Name);
}

void Ext_Actor::ComponentInitialize(std::shared_ptr<Ext_Component> _Component, std::shared_ptr<Ext_Actor> _Actor, int _Order)
{
	_Component->SetOwnerActor(_Actor);
	_Component->Start();
}
