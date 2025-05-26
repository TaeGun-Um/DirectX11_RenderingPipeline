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
#include <list>
#include <unordered_set>
#include <vector>
#include <string>
#include <string_view>

#include "Base_BaseEnum.h"

// DirectX11
#include <d3d11.h>				// Direct3D 11의 핵심 API 선언이 담긴 헤더(디바이스, 디바이스 컨텍스트, 렌더 타겟, 셰이더 리소스 등)
#include <dxgi.h>					// DirectX Graphics Infrastructure. 화면 출력(출력 모드), 스왑 체인, 어댑터 관리 등 시스템 관련 기능의 선언이 담긴 헤더(어댑터, 스왑체인 등)
#include <d3dcompiler.h>		// HLSL 셰이더 컴파일 관련 API 선언이 담긴 헤더(셰이더 컴파일 API 인터페이스)
#include <D3D11Shader.h>	// 셰이더 리플렉션(Shader Reflection) 관련 인터페이스 선언이 담긴 헤더(셰이더 리플렉션 API 인터페이스)

#pragma comment(lib, "d3d11.lib")				// d3d11.h 기능 링크
#pragma comment(lib, "dxgi.lib")				// dxgi.h 기능 링크
#pragma comment(lib, "d3dcompiler.lib")		// d3dcompiler.h 기능 링크
#pragma comment(lib, "dxguid")					// DirectX 관련 GUID(전역 고유 식별자) 상수 정의를 포함하며, COM 기반 인터페이스 사용에 필요(IID_ID3D11ShaderReflection : 셰이더 리플렉션에 활용)