#include "PrecompileHeader.h"
#include "RectActor.h"

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
	std::shared_ptr<class Ext_Transform> Trans = GetTransform();
	if (nullptr == Trans)
	{
		int a = 0;
	}

	int b = 0;
}

void RectActor::Destroy()
{

}