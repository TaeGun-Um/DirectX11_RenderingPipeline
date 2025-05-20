#include "PrecompileHeader.h"
#include "Ext_Scene.h"
#include "Ext_Actor.h"

Ext_Scene::Ext_Scene()
{
}

Ext_Scene::~Ext_Scene()
{
}

void Ext_Scene::SetActorName(std::shared_ptr<Ext_Actor> _Actor, std::string_view _Name)
{
	_Actor->SetName(_Name);
}

void Ext_Scene::ActorInitialize(std::shared_ptr<Ext_Actor> _Actor, std::shared_ptr<Ext_Scene> _Level, int _Order)
{
	_Actor->SetOwnerScene(_Level);
	_Actor->Start();
}
