#pragma once

#include "PrecompileHeader.h"
#include <DirectX11_Base/Base_Windows.h>
#include <DirectX11_Base/Base_Math.h>
#include <DirectX11_Extension/Ext_Core.h>
#include <DirectX11_Contents/Contents_Core.h>

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
	// Contents_Core�� Ext_Cor�� ����� ���� ������, �̷� ������ �����ؼ� ������ ������ ����� �� �ֵ��� ��
	Ext_Core::Run(hInstance, Contents_Core::Start, Contents_Core::End, StartScreenSize, IsStartFullScreen);
	return 0;
}