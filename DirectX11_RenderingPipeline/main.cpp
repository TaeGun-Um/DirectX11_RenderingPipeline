#pragma once

#include "PrecompileHeader.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Math.h>
#include <DirectX11_Extension/Ext_Core.h>

float4 StartScreenSize = { 1280.f, 720.f }; // ���ϴ� ȭ�� ũ�� �Է�
bool IsStartFullScreen = false;				       // Ǯ��ũ��

int APIENTRY wWinMain
(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow
)
{
	Ext_Core::Run(hInstance, StartScreenSize, IsStartFullScreen);
	return 0;
}