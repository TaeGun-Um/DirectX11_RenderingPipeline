#pragma once

#include <crtdbg.h>
#include <cassert>

#define MsgAssert(MsgText) {std::string ErrorText = MsgText; MessageBoxA(nullptr, ErrorText.c_str(), "Error", MB_OK); assert(false);}
#define MsgTextBox(MsgText) {std::string ErrorText = MsgText; MessageBoxA(nullptr, ErrorText.c_str(), "Text", MB_OK);}

// 디버깅 헬퍼 클래스
class Base_Debug
{
public:
	// 릭 체크 함수 실행
	static void LeakCheck() { _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); }

	// 릭 발생 지점 확인 함수
	static void LeakPointBreak(int _Point) { _CrtSetBreakAlloc(_Point); }
	
};