#include "PrecompileHeader.h"
#include "Base_Windows.h"
#include "Base_Debug.h"
#include "Base_Math.h"

std::function<LRESULT(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)> Base_Windows::UserMessageFunction;
WNDCLASSEX  Base_Windows::wcex;
HWND             Base_Windows::HWnd = nullptr;
HDC                Base_Windows::WindowBackBufferHdc = nullptr;
bool                Base_Windows::IsWindowUpdate = true;
float4              Base_Windows::ScreenSize = { 0.f, 0.f, 0.f, 0.f };
float4              Base_Windows::WindowSize = { 0.f, 0.f, 0.f, 0.f };

LRESULT CALLBACK Base_Windows::MessageFunction(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
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
        Base_Windows::SetWindowsEnd();
        break;
    }
    default:
        return DefWindowProc(_hWnd, _message, _wParam, _lParam);
    }

    return 0;
}

void Base_Windows::WindowCreate(HINSTANCE _hInstance, const float4& _ScreenSize, bool _IsFullScreen)
{
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &Base_Windows::MessageFunction;
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

    HWnd = CreateWindow
    (
        "WindowDefault", 
        "DirectX11", 
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
    UpdateWindow(HWnd);                 // â�� ������Ʈ ������ ��� ���� ���� ��� â�� WM_PAINT �޽����� ���� ������ â�� Ŭ���̾�Ʈ ������ ������Ʈ

    ScreenSize = _ScreenSize;
    RECT Rc = { 0, 0, _ScreenSize.ix(), _ScreenSize .iy() }; // ������ â ������ Ÿ��Ʋ��, �������� ũ�⸦ ����Ͽ� �����Ѵ�.

    AdjustWindowRect(&Rc, WS_OVERLAPPEDWINDOW, FALSE); // ���� ���ϴ� ũ�⸦ ������ Ÿ��Ʋ�ٱ��� ����� ũ�⸦ �����ִ� �Լ�.

    WindowSize = { static_cast<float>(Rc.right - Rc.left), static_cast<float>(Rc.bottom - Rc.top) };
    
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

int Base_Windows::WindowLoop(std::function<void()> _Start, std::function<void()> _Loop, std::function<void()> _End)
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
