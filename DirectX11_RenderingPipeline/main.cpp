#pragma once

#include "PrecompileHeader.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Math.h>
#include <DirectX11_Extension/Ext_Core.h>
#include <DirectX11_Contents/Contents_Core.h>

float4 StartScreenSize = { 1280.f, 720.f }; // 원하는 화면 크기 입력
bool IsStartFullScreen = false;				       // 풀스크린

int APIENTRY wWinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow
)
{
	// Contents_Core가 Ext_Cor의 상속은 받지 않지만, 이런 식으로 전달해서 상위가 하위를 사용할 수 있도록 함
	Ext_Core::Run(hInstance, Contents_Core::Start, Contents_Core::End, StartScreenSize, IsStartFullScreen);
	return 0;
}