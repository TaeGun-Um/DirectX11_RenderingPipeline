#pragma once
// DirectX11_Base PrecompileHeader
// Base�� DirectX�ʹ� ���������� ���α׷��� �ʿ��� ����� �߰��ϱ� ���� ���� ������Ʈ

#include <Windows.h>
#include <cmath>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>
#include <functional>
#include <algorithm>

// std
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <string_view>

#include "Base_BaseEnum.h"

// DirectX11
#include <d3d11.h>				// Direct3D 11�� �ٽ� API ������ ��� ���(����̽�, ����̽� ���ؽ�Ʈ, ���� Ÿ��, ���̴� ���ҽ� ��)
#include <dxgi.h>					// DirectX Graphics Infrastructure. ȭ�� ���(��� ���), ���� ü��, ����� ���� �� �ý��� ���� ����� ������ ��� ���(�����, ����ü�� ��)
#include <d3dcompiler.h>		// HLSL ���̴� ������ ���� API ������ ��� ���(���̴� ������ API �������̽�)
#include <D3D11Shader.h>	// ���̴� ���÷���(Shader Reflection) ���� �������̽� ������ ��� ���(���̴� ���÷��� API �������̽�)

#pragma comment(lib, "d3d11.lib")				// d3d11.h ��� ��ũ
#pragma comment(lib, "dxgi.lib")				// dxgi.h ��� ��ũ
#pragma comment(lib, "d3dcompiler.lib")		// d3dcompiler.h ��� ��ũ
#pragma comment(lib, "dxguid")					// DirectX ���� GUID(���� ���� �ĺ���) ��� ���Ǹ� �����ϸ�, COM ��� �������̽� ��뿡 �ʿ�(IID_ID3D11ShaderReflection : ���̴� ���÷��ǿ� Ȱ��)