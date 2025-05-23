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
//#include <multimap>
#include <list>
#include <vector>
#include <string>
#include <string_view>

#include "Base_BaseEnum.h"

// DirectX11
#include <d3d11.h> // Direct3D 11 ����̽��� ���� ���
#include <dxgi.h>    // DXGI �������̽��� ���� ���(�����, ��� ��)
#include <d3dcompiler.h> // 
#include <D3D11Shader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid")  // ?