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
float4              Base_Windows::WindowPosition = { 0.f, 0.f, 0.f, 0.f };

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
    // ������ Ŭ������ ������ ���� ��� ���
    /*1*/wcex.cbSize = sizeof(WNDCLASSEX);
    /*2*/wcex.style = CS_HREDRAW | CS_VREDRAW;
    /*3*/wcex.lpfnWndProc = &Base_Windows::MessageFunction;
    /*4*/wcex.cbClsExtra = 0;
    /*5*/wcex.cbWndExtra = 0;
    /*6*/wcex.hInstance = _hInstance;
    /*7*/wcex.hIcon = nullptr;
    /*8*/wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    /*9*/wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    /*10*/wcex.lpszMenuName = nullptr;
    /*11*/wcex.lpszClassName = "WindowDefault";
    /*12*/wcex.hIconSm = nullptr;
    // <<����>>
    // [1] ����ü�� ũ�� ����, �׻� sizeof(WNDCLASSEX)�� �����ؾ���
    // [2] 
    // [3] ������ ���ν��� �Լ� ������ ����(�޽���ó�� �Լ�)
    // [4] Ŭ���� ���� �޸�, 0�� ��� �����̶�� ��
    // [5] �ν��Ͻ� ���� �޸�, 0�� ��� �����̶�� ��
    // [6] ���� ���ø����̼��� �ڵ�
    // [7] ū ������ �ڵ�, nullptr ���� �� �⺻ �ɼ� ���
    // [8] ���콺 Ŀ�� ����, IDC_ARROW�� �⺻ ȭ��ǥ�̴�.
    // [9] ��� �귯�� ����, COLOR_WINDOW�� �⺻ ����̴�.
    // [10] ����� �޴� ���ҽ��� �ִ���? ���⼭�� �������� ����
    // [11] ������ ������ Ŭ���� �̸��� ����, �̸��� �����ϰ� ����
    // [12] ���� ������ �ڵ�, nullptr ���� �� �⺻ �ɼ� ���

    // ���� ������ wcex Ŭ������ �ý��ۿ� ���
    if (!RegisterClassEx(&wcex))
    {
        MsgAssert("������ Ŭ���� ��Ͽ� �����߽��ϴ�.");
        return;
    }

    // ���� Ŭ���� ������� ���� ������ â ����
    HWnd = CreateWindow
    (
        /*1*/"WindowDefault", 
        /*2*/"DirectX11", 
        /*3*/_IsFullScreen ? (WS_VISIBLE | WS_POPUP) : WS_OVERLAPPEDWINDOW,
        /*4*/CW_USEDEFAULT, 
        /*5*/0, 
        /*6*/CW_USEDEFAULT, 
        /*7*/0, 
        /*8*/nullptr, 
        /*9*/nullptr, 
        /*10*/_hInstance, 
        /*11*/nullptr
    );
    // <<����>>
    // [1] ����� Ŭ���� �̸��� �־���, ������ WindowDefault�� �����
    // [2] ������ ���� ǥ���ٿ� ��� �ؽ�Ʈ, â�� �̸��� �ȴ�.
    // [3] 
    // [4] ������ X ��ǥ, CW_USEDEFAULT�� �⺻ �ɼ�
    // [5] ������ Y ��ǥ, CW_USEDEFAULT�� �⺻ �ɼ�, �ɼ� ������ ���õ�(�⺻���� ������)
    // [6] �ʺ� ����, CW_USEDEFAULT�� �ڵ� ����
    // [7] ���� ����, CW_USEDEFAULT�� �ڵ� ����, �ɼ� ������ ���õ�(�⺻���� ������)
    // [8] �θ� ������ �ڵ� ����, ������ nullptr�� �����Ͽ� �ֻ��� ������� ������
    // [9] �޴� �ڵ� ����, ������ nullptr
    // [10] ���ø����̼� �ڵ� ����, ���⿡ �ش� ���μ����� �ڵ��� �������
    // [11] �߰� ������ ����, ������ nullptr�� ����

    if (true == _IsFullScreen)
    {
        // GWL_STYLE�� ������ �Ϲ� ��Ÿ���̸�, 0�� �����ϸ� ��� ��Ÿ���� �����Ѵ�. -> ������ Ÿ��Ʋ��, �ݱ��ư ���ŵǰ� ũ������ �Ұ��� ����
        SetWindowLong(HWnd, GWL_STYLE, 0);
    }

    WindowBackBufferHdc = GetDC(HWnd);

    ShowWindow(HWnd, SW_SHOW); // â ǥ�� ��� ����, SW_SHOW�� â�� Ȱ��ȭ�ϰ� ���� ũ��� ��ġ�� ǥ��
    UpdateWindow(HWnd);                 // â�� ������Ʈ ������ ��� ���� ���� ��� â�� WM_PAINT �޽����� ���� ������ â�� Ŭ���̾�Ʈ ������ ������Ʈ

    RECT Rc = { 0, 0, _ScreenSize.ix(), _ScreenSize .iy() }; // ������ â ������ Ÿ��Ʋ��, �������� ũ�⸦ ����Ͽ� �����Ѵ�.
    AdjustWindowRect(&Rc, WS_OVERLAPPEDWINDOW, FALSE); // ���� ���ϴ� ũ�⸦ ������ Ÿ��Ʋ�ٱ��� ����� ũ�⸦ �����ִ� �Լ�.
    
    ScreenSize = _ScreenSize;
    WindowSize = { static_cast<float>(Rc.right - Rc.left), static_cast<float>(Rc.bottom - Rc.top) };
    WindowPosition = { 0.f, 0.f, 0.f, 0.f };
    
    // 0�� �־��ָ� ������ ũ�⸦ �����Ѵ�.
    SetWindowPos(HWnd, nullptr, WindowPosition.ix(), WindowPosition.iy(), WindowSize.ix(), WindowSize.iy(), SWP_NOZORDER);

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
