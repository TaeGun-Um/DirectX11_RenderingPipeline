#include "PrecompileHeader.h"
#include "EngineWindows.h"
#include "EngineDebug.h"

std::function<LRESULT(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)> EngineWindows::UserMessageFunction;
WNDCLASSEX EngineWindows::wcex;
HWND EngineWindows::HWnd = nullptr;
HDC EngineWindows::WindowBackBufferHdc = nullptr;
bool EngineWindows::IsWindowUpdate = true;

LRESULT CALLBACK EngineWindows::MessageFunction(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
    if (nullptr != UserMessageFunction)
    {
        if (0 != UserMessageFunction(_hWnd, _message, _wParam, _lParam))
        {
            return true;
        }
    }

    switch (_message)
    {
    case WM_SETFOCUS:
    {
        break;
    }
    case WM_KILLFOCUS:
    {
        break;
    }
    case WM_KEYDOWN:
    {
        break;
    }
    case WM_DESTROY:
    {
        // IsWindowUpdate = false;
        break;
    }
    default:
        return DefWindowProc(_hWnd, _message, _wParam, _lParam);
    }

    return 0;
}

void EngineWindows::WindowCreate(HINSTANCE _hInstance)
{
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &EngineWindows::MessageFunction;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = _hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = "WindowDefault";
    wcex.hIconSm = nullptr;

    if (!RegisterClassEx(&wcex))
    {
        MsgAssert("윈도우 클래스 등록에 실패했습니다.");
        return;
    }

    bool _IsFullScreen = false;

    HWnd = CreateWindow
    (
        "WindowDefault", "DirectX11", 
        _IsFullScreen ? (WS_VISIBLE | WS_POPUP) : WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        0, 
        CW_USEDEFAULT, 
        0, 
        nullptr, 
        nullptr, 
        _hInstance, 
        nullptr
    );

    if (true == _IsFullScreen)
    {
        SetWindowLong(HWnd, GWL_STYLE, 0);
    }

    WindowBackBufferHdc = GetDC(HWnd);

    ShowWindow(HWnd, SW_SHOW); // 창 표시 방법 제어, SW_SHOW는 창을 활성화하고 현재 크기와 위치에 표시
    UpdateWindow(HWnd);        // 창의 업데이트 영역이 비어 있지 않은 경우 창에 WM_PAINT 메시지를 보내 지정된 창의 클라이언트 영역을 업데이트

    // 윈도우 창 설정은 타이틀바, 프레임의 크기를 고려하여 설정한다.
    //          위치      크기
    //RECT Rc = { 0, 0, _Size.ix(), _Size.iy() };
    RECT Rc = { 0, 0, 1280, 720 };

    // ScreenSize = _Size;

    AdjustWindowRect(&Rc, WS_OVERLAPPEDWINDOW, FALSE); // 내가 원하는 크기를 넣으면 타이틀바까지 고려한 크기를 리턴주는 함수.

    // WindowSize = { static_cast<float>(Rc.right - Rc.left), static_cast<float>(Rc.bottom - Rc.top) };
    // 0을 넣어주면 기존의 크기를 유지한다.
    // SetWindowPos(HWnd, nullptr, WindowPos.ix(), WindowPos.iy(), WindowSize.ix(), WindowSize.iy(), SWP_NOZORDER);
    SetWindowPos(HWnd, nullptr, 0, 0, Rc.right - Rc.left, Rc.bottom - Rc.top, SWP_NOZORDER);

    //if (nullptr != DoubleBufferImage)
    //{
    //    delete DoubleBufferImage;
    //    DoubleBufferImage = nullptr;
    //}

    //DoubleBufferImage = new GameEngineImage();
    //DoubleBufferImage->ImageCreate(ScreenSize);
}

int EngineWindows::WindowLoop(std::function<void()> _Start, std::function<void()> _Loop, std::function<void()> _End)
{
    if (nullptr != _Start)
    {
        _Start();
    }

    MSG msg = {};

    while (IsWindowUpdate)
    {
        // 비동기 함수
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {

            if (nullptr != _Loop)
            {
                _Loop();
                // GameEngineInput::IsAnyKeyOff();
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // 데드타임, 이때도 게임을 실행한다.
        if (nullptr != _Loop)
        {
            _Loop();
            // GameEngineInput::IsAnyKeyOff();
        }
    }

    if (nullptr != _End)
    {
        _End();
    }

    //if (nullptr != BackBufferImage)
    //{
    //    delete DoubleBufferImage;
    //    DoubleBufferImage = nullptr;

    //    delete BackBufferImage;
    //    BackBufferImage = nullptr;
    //}

    return (int)msg.wParam;
}
