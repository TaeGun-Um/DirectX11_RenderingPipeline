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
        MsgAssert("������ Ŭ���� ��Ͽ� �����߽��ϴ�.");
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

    ShowWindow(HWnd, SW_SHOW); // â ǥ�� ��� ����, SW_SHOW�� â�� Ȱ��ȭ�ϰ� ���� ũ��� ��ġ�� ǥ��
    UpdateWindow(HWnd);        // â�� ������Ʈ ������ ��� ���� ���� ��� â�� WM_PAINT �޽����� ���� ������ â�� Ŭ���̾�Ʈ ������ ������Ʈ

    // ������ â ������ Ÿ��Ʋ��, �������� ũ�⸦ ����Ͽ� �����Ѵ�.
    //          ��ġ      ũ��
    //RECT Rc = { 0, 0, _Size.ix(), _Size.iy() };
    RECT Rc = { 0, 0, 1280, 720 };

    // ScreenSize = _Size;

    AdjustWindowRect(&Rc, WS_OVERLAPPEDWINDOW, FALSE); // ���� ���ϴ� ũ�⸦ ������ Ÿ��Ʋ�ٱ��� ����� ũ�⸦ �����ִ� �Լ�.

    // WindowSize = { static_cast<float>(Rc.right - Rc.left), static_cast<float>(Rc.bottom - Rc.top) };
    // 0�� �־��ָ� ������ ũ�⸦ �����Ѵ�.
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
        // �񵿱� �Լ�
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

        // ����Ÿ��, �̶��� ������ �����Ѵ�.
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
