#pragma once

class EngineWindows
{
public:
	// delete Function
	EngineWindows(const EngineWindows& _Other) = delete;
	EngineWindows(EngineWindows&& _Other) noexcept = delete;
	EngineWindows& operator=(const EngineWindows& _Other) = delete;
	EngineWindows& operator=(EngineWindows&& _Other) noexcept = delete;

	static LRESULT CALLBACK MessageFunction(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam);
	static void SetUserMessageFunction(std::function<LRESULT(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)> _UserMessageFunction) { UserMessageFunction = _UserMessageFunction; }
	static std::function<LRESULT(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)> UserMessageFunction;
	static void WindowCreate(HINSTANCE _hInstance);
	static int WindowLoop(std::function<void()> _Start, std::function<void()> _Loop, std::function<void()> _End);

	static void SetWindowsEnd() { IsWindowUpdate = false; };

protected:
	
private:
	// constrcuter destructer
	EngineWindows() {};
	~EngineWindows() {};
	
	static WNDCLASSEX wcex;
	static HWND HWnd;
	static HDC WindowBackBufferHdc;
	static bool IsWindowUpdate;
};