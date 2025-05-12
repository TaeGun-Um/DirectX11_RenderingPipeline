#pragma once

#include "PrecompileHeader.h"
#include <DirectX11_Base/EngineWindows.h>
#include <DirectX11_Extension/EngineCore.h>

int APIENTRY wWinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow
)
{
	EngineCore::Run(hInstance);
	return 0;
}