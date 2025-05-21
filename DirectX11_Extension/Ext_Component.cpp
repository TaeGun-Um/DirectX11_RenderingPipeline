#include "PrecompileHeader.h"
#include "Ext_Component.h"
#include "Ext_Transform.h"

Ext_Component::Ext_Component()
{
	Transform = std::make_shared<Ext_Transform>();
}

Ext_Component::~Ext_Component()
{
}
