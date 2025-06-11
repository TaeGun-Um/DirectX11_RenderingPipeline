#include "PrecompileHeader.h"
#include "Contents_Core.h"

#include <DirectX11_Extension/Ext_Core.h>

#include "MathScene.h"
#include "RenderScene.h"

#include "RectActor.h"

/// <summary>
/// Scene 생성하는 곳
/// </summary>
void Contents_Core::Start()
{
	// Ext_Core::CreateScene<RenderScene>("RenderScene");
	// Ext_Core::ChangeScene("RenderScene");

	Ext_Core::CreateScene<MathScene>("MathScene");
	Ext_Core::ChangeScene("MathScene");
}

void Contents_Core::End()
{
	// 프로세스 종료 시 무언가를 진행하면 됨
}
