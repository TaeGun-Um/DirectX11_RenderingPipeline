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