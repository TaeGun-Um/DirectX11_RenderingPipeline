#include "PrecompileHeader.h"
#include "Base_Windows.h"
#include "Base_Debug.h"
#include "Base_Math.h"

#include "Base_String.h"

std::function<LRESULT(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)> Base_Windows::UserMessageFunction;
WNDCLASSEX  Base_Windows::wcex;
HWND             Base_Windows::HWnd = nullptr;
HDC                Base_Windows::WindowBackBufferHdc = nullptr;
float4              Base_Windows::ScreenSize = { 0.f, 0.f, 0.f, 0.f };
float4              Base_Windows::WindowSize = { 0.f, 0.f, 0.f, 0.f };
float4              Base_Windows::WindowPosition = { 0.f, 0.f, 0.f, 0.f };
bool                Base_Windows::IsWindowUpdate = true;
bool                Base_Windows::IsWindowFocus = true;

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
        // 사용자가 Alt+Tab으로 이 창으로 돌아오거나, 마우스로 클릭하여 활성화된 경우
        IsWindowFocus = true;
        break;
    }
    case WM_KILLFOCUS:
    {
        // 다른 창으로 전환되거나 최소화 등으로 비활성화되는 순간
        IsWindowFocus = false;
        break;
    }
    case WM_KEYDOWN:
    {
        // 사용자가 키보드 눌렀을 때 활성화
        break;
    }
    case WM_DESTROY:
    {
        // 윈도우가 파괴될 때 활성화
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
    // 윈도우 클래스의 외형과 동작 방식 등록
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
    // <<설명>>
    // [1] 구조체의 크기 설정, 항상 sizeof(WNDCLASSEX)로 지정해야함
    // [2] 
    // [3] 윈도우 프로시저 함수 포인터 설정(메시지처리 함수)
    // [4] 클래스 여분 메모리, 0은 사용 안함이라는 뜻
    // [5] 인스턴스 여분 메모리, 0은 사용 안함이라는 뜻
    // [6] 현재 어플리케이션의 핸들
    // [7] 큰 아이콘 핸들, nullptr 전달 시 기본 옵션 사용
    // [8] 마우스 커서 설정, IDC_ARROW는 기본 화살표이다.
    // [9] 배경 브러시 설정, COLOR_WINDOW는 기본 흰색이다.
    // [10] 연결된 메뉴 리소스가 있는지? 여기서는 없음으로 설정
    // [11] 생성할 윈도우 클래스 이름을 지정, 이름은 고유하게 사용됨
    // [12] 작은 아이콘 핸들, nullptr 전달 시 기본 옵션 사용

    // 위에 생성한 wcex 클래스를 시스템에 등록
    if (!RegisterClassEx(&wcex))
    {
        MsgAssert("윈도우 클래스 등록에 실패했습니다.");
        return;
    }

    // 만들어낸 클래스 기반으로 실제 윈도우 창 생성
    HWnd = CreateWindow
    (
        /*1*/"WindowDefault", 
        /*2*/"DirectX Framework",
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
    // <<설명>>
    // [1] 등록한 클래스 이름을 넣어줌, 위에서 WindowDefault로 등록함
    // [2] 윈도우 제목 표시줄에 띄울 텍스트, 창의 이름이 된다.
    // [3] 
    // [4] 윈도우 X 좌표, CW_USEDEFAULT는 기본 옵션
    // [5] 윈도우 Y 좌표, CW_USEDEFAULT는 기본 옵션, 옵션 설정이 무시됨(기본으로 설정됨)
    // [6] 너비 설정, CW_USEDEFAULT는 자동 설정
    // [7] 높이 설정, CW_USEDEFAULT는 자동 설정, 옵션 설정이 무시됨(기본으로 설정됨)
    // [8] 부모 윈도우 핸들 여부, 없으면 nullptr을 전달하여 최상위 윈도우로 생성됨
    // [9] 메뉴 핸들 여부, 없으면 nullptr
    // [10] 어플리케이션 핸들 여부, 여기에 해당 프로세스의 핸들을 등록해줌
    // [11] 추가 데이터 여부, 없으면 nullptr을 전달

    if (true == _IsFullScreen)
    {
        // GWL_STYLE은 윈도우 일반 스타일이며, 0을 전달하면 모든 스타일을 제거한다. -> 윈도우 타이틀바, 닫기버튼 제거되고 크기조절 불가로 변경
        SetWindowLong(HWnd, GWL_STYLE, 0);
    }

    WindowBackBufferHdc = GetDC(HWnd);

    ShowWindow(HWnd, SW_SHOW); // 창 표시 방법 제어, SW_SHOW는 창을 활성화하고 현재 크기와 위치에 표시
    UpdateWindow(HWnd);                 // 창의 업데이트 영역이 비어 있지 않은 경우 창에 WM_PAINT 메시지를 보내 지정된 창의 클라이언트 영역을 업데이트

    RECT Rc = { 0, 0, _ScreenSize.ix(), _ScreenSize .iy() }; // 윈도우 창 설정은 타이틀바, 프레임의 크기를 고려하여 설정한다.
    AdjustWindowRect(&Rc, WS_OVERLAPPEDWINDOW, FALSE); // 내가 원하는 크기를 넣으면 타이틀바까지 고려한 크기를 리턴주는 함수.
    
    ScreenSize = _ScreenSize;
    WindowSize = { static_cast<float>(Rc.right - Rc.left), static_cast<float>(Rc.bottom - Rc.top) };
    WindowPosition = { 0.f, 0.f, 0.f, 0.f };
    
    // 0을 넣어주면 기존의 크기를 유지한다.
    SetWindowPos(HWnd, nullptr, WindowPosition.ix(), WindowPosition.iy(), WindowSize.ix(), WindowSize.iy(), SWP_NOZORDER);
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

    return (int)msg.wParam;
}
