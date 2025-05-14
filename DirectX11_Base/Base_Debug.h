#pragma once

#include <crtdbg.h>
#include <cassert>

#define MsgAssert(MsgText) {std::string ErrorText = MsgText; MessageBoxA(nullptr, ErrorText.c_str(), "Error", MB_OK); assert(false);}
#define MsgTextBox(MsgText) {std::string ErrorText = MsgText; MessageBoxA(nullptr, ErrorText.c_str(), "Text", MB_OK);}

// ����� ���� Ŭ����
class Base_Debug
{
public:
	// delete Function
	Base_Debug(const Base_Debug& _Other) = delete;
	Base_Debug(Base_Debug&& _Other) noexcept = delete;
	Base_Debug& operator=(const Base_Debug& _Other) = delete;
	Base_Debug& operator=(Base_Debug&& _Other) noexcept = delete;

	// �� üũ �Լ� ����
	static void LeakCheck()
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	}

	// �� �߻� ���� Ȯ�� �Լ�
	static void LeakPointBreak(int _Point)
	{
		_CrtSetBreakAlloc(_Point);
	}

protected:
	
private:
	Base_Debug() {};
	~Base_Debug() {};
	
};