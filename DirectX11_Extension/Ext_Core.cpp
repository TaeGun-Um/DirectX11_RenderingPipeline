#include "PrecompileHeader.h"
#include "Ext_Core.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Debug.h>

void Ext_Core::Run(HINSTANCE _hInstance)
{
	Base_Debug::LeakCheck();
	 int* TrustLeak = new int;

	// 윈도우 창 생성 후 루프문 시작
	Base_Windows::WindowCreate(_hInstance);
	Base_Windows::WindowLoop(Ext_Core::Start, Ext_Core::Update, Ext_Core::End);
}

void Ext_Core::Start()
{
	// After Create Window, EngineStart

}

void Ext_Core::Update()
{
	// After EngineStart, EngineLoop
}

void Ext_Core::End()
{
	// After Window Destroy, Process ending
	int EndVal = 0;
}
