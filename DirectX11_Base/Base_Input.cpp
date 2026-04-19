#include "PrecompileHeader.h"
#include "Base_Input.h"
#include "Base_String.h"
#include "Base_Debug.h"
#include "Base_Windows.h"

std::map<std::string, Base_Input::Key>  Base_Input::Keys;
std::map<std::string, Base_Input::Axis> Base_Input::Axes;

int  Base_Input::MouseX       = 0;
int  Base_Input::MouseY       = 0;
int  Base_Input::MouseDeltaX  = 0;
int  Base_Input::MouseDeltaY  = 0;
int  Base_Input::MouseWheel   = 0;
bool Base_Input::bCursorLocked = false;

void Base_Input::Key::Reset()
{
	bIsDown = false;
	bIsPress = false;
}

bool Base_Input::Key::KeyCheck()
{
	return 0 != GetAsyncKeyState(Input);
}

void Base_Input::Key::Update(float _DeltaTime)
{
	if (true == KeyCheck())
	{
		PressTime += _DeltaTime;
		if (bIsFree) // 안눌러져 있었는데, 방금 누름(1틱 눌렀다)
		{
			bIsDown = true;
			bIsPress = true;
			bIsFree = false;
		}
		else if (bIsDown) // 누르고 있었는데, 또 누름(연속 입력중이네..)
		{
			bIsDown = false;
			bIsPress = true;
			bIsFree = false;
		}
	}
	else // 아무것도 안하고있음
	{
		PressTime = 0.f;
		bIsFree = true;
		bIsDown = false;
		bIsPress = false;
	}
}

// ---------------- Key(Action) ----------------
void Base_Input::CreateKey(std::string_view _Name, int _Key)
{
	std::string UpperName = Base_String::ToUpper(_Name);

	if (Keys.end() != Keys.find(UpperName))
	{
		MsgAssert("이미 존재하는 이름의 키를 또 만들려고 했습니다. " + UpperName);
	}

	Keys[UpperName].Input = _Key;
}

bool Base_Input::RebindKey(std::string_view _Name, int _NewKey)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	auto it = Keys.find(UpperName);
	if (it == Keys.end()) return false;
	it->second.Input = _NewKey;
	it->second.Reset();
	return true;
}

bool Base_Input::IsDown(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	auto it = Keys.find(UpperName);
	if (it == Keys.end())
	{
		MsgAssert("존재하지 않는 키를 조회 " + UpperName);
		return false;
	}
	return it->second.bIsDown;
}

bool Base_Input::IsPress(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	auto it = Keys.find(UpperName);
	if (it == Keys.end())
	{
		MsgAssert("존재하지 않는 키를 조회 " + UpperName);
		return false;
	}
	return it->second.bIsPress;
}

bool Base_Input::IsFree(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	auto it = Keys.find(UpperName);
	if (it == Keys.end())
	{
		MsgAssert("존재하지 않는 키를 조회 " + UpperName);
		return false;
	}
	return it->second.bIsFree;
}

// ---------------- Axis ----------------
void Base_Input::CreateAxis(std::string_view _Name, int _PositiveKey, int _NegativeKey)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	Axis& A = Axes[UpperName];
	A.PositiveKey = _PositiveKey;
	A.NegativeKey = _NegativeKey;
}

float Base_Input::GetAxis(std::string_view _Name)
{
	std::string UpperName = Base_String::ToUpper(_Name);
	auto it = Axes.find(UpperName);
	if (it == Axes.end()) return 0.0f;

	float value = 0.0f;
	if (0 != GetAsyncKeyState(it->second.PositiveKey)) value += 1.0f;
	if (0 != GetAsyncKeyState(it->second.NegativeKey)) value -= 1.0f;
	return value;
}

// ---------------- Cursor Lock ----------------
void Base_Input::SetCursorLocked(bool _Locked)
{
	if (_Locked == bCursorLocked) return;

	bCursorLocked = _Locked;
	if (_Locked)
	{
		// 커서 숨기고 화면 중앙으로 이동
		float4 Screen = Base_Windows::GetScreenSize();
		POINT Center = { static_cast<long>(Screen.x / 2), static_cast<long>(Screen.y / 2) };
		ClientToScreen(Base_Windows::GetHWnd(), &Center);
		SetCursorPos(Center.x, Center.y);
		while (ShowCursor(FALSE) >= 0) { /* 내부 카운터가 음수가 될 때까지 */ }
	}
	else
	{
		while (ShowCursor(TRUE) < 0) { /* 내부 카운터가 0 이상이 될 때까지 */ }
	}
}

// ---------------- Update ----------------
void Base_Input::Update(float _DeltaTime)
{
	// 키 상태 갱신
	for (auto& Iter : Keys)
	{
		Iter.second.Update(_DeltaTime);
	}

	// 마우스 위치 / 델타
	POINT CursorPt;
	GetCursorPos(&CursorPt);
	ScreenToClient(Base_Windows::GetHWnd(), &CursorPt);

	if (bCursorLocked)
	{
		// 잠금 상태: 델타는 '현재 위치 - 중앙'으로 계산, 매 프레임 중앙으로 재배치
		float4 Screen = Base_Windows::GetScreenSize();
		int CenterX = static_cast<int>(Screen.x / 2);
		int CenterY = static_cast<int>(Screen.y / 2);

		MouseDeltaX = CursorPt.x - CenterX;
		MouseDeltaY = CursorPt.y - CenterY;

		POINT Recenter = { CenterX, CenterY };
		ClientToScreen(Base_Windows::GetHWnd(), &Recenter);
		SetCursorPos(Recenter.x, Recenter.y);

		MouseX = CenterX;
		MouseY = CenterY;
	}
	else
	{
		// 자유 모드: 이전 프레임 위치와 비교해 델타 산출
		MouseDeltaX = CursorPt.x - MouseX;
		MouseDeltaY = CursorPt.y - MouseY;
		MouseX = CursorPt.x;
		MouseY = CursorPt.y;
	}

	// 휠 (Base_Windows가 WM_MOUSEWHEEL에서 누적한 값 소비)
	MouseWheel = Base_Windows::ConsumeMouseWheelDelta();
}
