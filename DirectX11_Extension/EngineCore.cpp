#include "PrecompileHeader.h"
#include "EngineCore.h"
#include <DirectX11_Base/EngineWindows.h>

void EngineCore::Run(HINSTANCE _hInstance)
{
	// GameEngineDebug::LeakCheck();
	//_CrtSetBreakAlloc(1887917);

	// GameEngineActor::ScreenRatio.x = _Size.x / 1600;
	// GameEngineActor::ScreenRatio.y = _Size.y / 900;
	// 윈도우 창 생성 후 루프문 시작
	EngineWindows::WindowCreate(_hInstance);
	EngineWindows::WindowLoop(EngineCore::Start, EngineCore::Update, EngineCore::End);
}

void EngineCore::Start()
{
	int StartVal = 0;
}

int UpdateVal = 0;

void EngineCore::Update()
{
	//UpdateVal++;

	//if (100000 == UpdateVal)
	//{
	//	EngineWindows::SetWindowsEnd();
	//}
}

void EngineCore::End()
{
	int EndVal = 0;
}
