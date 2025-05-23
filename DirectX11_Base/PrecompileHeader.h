#pragma once
// DirectX11_Base PrecompileHeader
// Base는 DirectX와는 연관없지만 프로그램에 필요한 기능을 추가하기 위해 만든 프로젝트

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
#include <d3d11.h> // Direct3D 11 디바이스를 위한 헤더
#include <dxgi.h>    // DXGI 인터페이스를 위한 헤더(어댑터, 출력 등)
#include <d3dcompiler.h> // 
#include <D3D11Shader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid")  // ?