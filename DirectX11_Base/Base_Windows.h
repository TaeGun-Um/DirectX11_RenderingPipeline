#pragma once

#include "Base_Math.h"

// Windows 창 생성용 클래스
class Base_Windows
{
public:
	// delete Function
	Base_Windows(const Base_Windows& _Other) = delete;
	Base_Windows(Base_Windows&& _Other) noexcept = delete;
	Base_Windows& operator=(const Base_Windows& _Other) = delete;
	Base_Windows& operator=(Base_Windows&& _Other) noexcept = delete;

	static LRESULT CALLBACK MessageFunction(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam);
	static void SetUserMessageFunction(std::function<LRESULT(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)> _UserMessageFunction) { UserMessageFunction = _UserMessageFunction; }
	static std::function<LRESULT(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)> UserMessageFunction;
	static void WindowCreate(HINSTANCE _hInstance, const float4& _ScreenSize, bool _IsFullScreen);
	static int WindowLoop(std::function<void()> _Start, std::function<void()> _Loop, std::function<void()> _End);
	
	// Getter, Setter
	static HWND GetHWnd()	 { return HWnd;	}
	static void SetWindowsEnd() { IsWindowUpdate = false; };
	static float4 GetScreenSize() { return ScreenSize; };

protected:
	
private:
	// constrcuter destructer
	Base_Windows() {};
	~Base_Windows() {};
	
	static WNDCLASSEX wcex;
	static HWND HWnd;
	static HDC WindowBackBufferHdc;
	static bool IsWindowUpdate;
	static float4 ScreenSize;
	static float4 WindowSize;
	static float4 WindowPosition;
};