#pragma once

// 입력 처리 시스템
// - Key(Action): 이름 기반 키 바인딩. IsDown / IsPress / IsFree로 순간 / 지속 / 해제 상태 질의
// - Axis: 양/음 두 키를 조합해 -1 ~ +1 float 값으로 반환 (예: "MoveForward" W/S)
// - Mouse: 위치, 프레임 델타, 휠 틱
// - Cursor Lock: 활성 시 커서를 화면 중앙 고정 + 숨김 (FPS / flycam용)
class Base_Input
{
	struct Key
	{
		bool bIsFree = false;
		bool bIsDown = false;
		bool bIsPress = false;

		float PressTime = 0.f;
		int Input = -1;

		void Reset();
		bool KeyCheck();
		void Update(float _DeltaTime);
	};

	struct Axis
	{
		int PositiveKey = 0;
		int NegativeKey = 0;
	};

public:
	// constrcuter destructer
	Base_Input() {}
	~Base_Input() {}

	// delete Function
	Base_Input(const Base_Input& _Other) = delete;
	Base_Input(Base_Input&& _Other) noexcept = delete;
	Base_Input& operator=(const Base_Input& _Other) = delete;
	Base_Input& operator=(Base_Input&& _Other) noexcept = delete;

	// ------ Action Key ------
	static void CreateKey(std::string_view _Name, int _Key);
	static bool RebindKey(std::string_view _Name, int _NewKey); // 키 재할당. 없는 이름이면 false
	static bool IsDown(std::string_view _Name);
	static bool IsPress(std::string_view _Name);
	static bool IsFree(std::string_view _Name);

	// ------ Axis ------
	static void  CreateAxis(std::string_view _Name, int _PositiveKey, int _NegativeKey);
	static float GetAxis(std::string_view _Name); // -1.0 ~ +1.0

	// ------ Mouse ------
	static int GetMouseX()       { return MouseX; }
	static int GetMouseY()       { return MouseY; }
	static int GetMouseDeltaX()  { return MouseDeltaX; }
	static int GetMouseDeltaY()  { return MouseDeltaY; }
	static int GetMouseWheel()   { return MouseWheel; } // 이번 프레임 휠 틱 (WHEEL_DELTA 단위, 120 = 한 칸)

	// ------ Cursor Lock ------
	// true일 때: 커서 숨김 + 매 프레임 화면 중앙으로 재배치 (FPS/flycam 스타일)
	// false일 때: 커서 표시 + 자유 이동
	static void SetCursorLocked(bool _Locked);
	static bool IsCursorLocked() { return bCursorLocked; }

	// ------ Frame Update ------
	static void Update(float _DeltaTime);

protected:

private:
	static std::map<std::string, Key>  Keys;
	static std::map<std::string, Axis> Axes;

	// 마우스 상태
	static int  MouseX, MouseY;
	static int  MouseDeltaX, MouseDeltaY;
	static int  MouseWheel;
	static bool bCursorLocked;
};
