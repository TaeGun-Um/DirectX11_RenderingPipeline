#include "PrecompileHeader.h"
#include "Contents_Core.h"

#include <DirectX11_Extension/Ext_Core.h>

#include "TempScene.h"
#include "TestScene.h"

#include "TriangleActor.h"
#include "RectActor.h"

Contents_Core::Contents_Core()
{
}

Contents_Core::~Contents_Core()
{
}

void Contents_Core::Start()
{
	Ext_Core::CreateScene<TestScene>("TestScene");
	Ext_Core::ChangeScene("TestScene");
}

void Contents_Core::End()
{
	int b = 0;
}
