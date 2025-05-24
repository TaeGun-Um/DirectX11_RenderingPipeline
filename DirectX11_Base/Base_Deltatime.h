#pragma once

// 프로젝트의 Deltatime 산출을 위한 클래스
class Base_Deltatime
{
public:
	// constrcuter destructer
	Base_Deltatime();
	~Base_Deltatime();

	// delete Function
	Base_Deltatime(const Base_Deltatime& _Other) = delete;
	Base_Deltatime(Base_Deltatime&& _Other) noexcept = delete;
	Base_Deltatime& operator=(const Base_Deltatime& _Other) = delete;
	Base_Deltatime& operator=(Base_Deltatime&& _Other) noexcept = delete;

	float TimeCheck(); // 시간 체크
	void TimeReset(); // 시간 리셋

	static void ResetFrameTime() { GlobalTime.FrameTime = 0.0f; }

	// Getter, Setter
	static Base_Deltatime& GetGlobalTime() { return GlobalTime; }
	float& GetFrameTime() { return GlobalTime.FrameTime; }
	float& GetFrameLimit() { return GlobalTime.FrameLimit; }
	float GetDeltaTime() const { return min(GlobalTime.floatDeltaTime, 0.1f); } // 프레임 드랍 시, clamp를 위한 리턴 방식
	void SetDeltaTime(float _DeltaTime) { GlobalTime.floatDeltaTime = _DeltaTime; }
	void AddFrameTime(float _FrameTime) { GlobalTime.FrameTime += _FrameTime; }
	void SetFrameRate(float _FrameRate) { GlobalTime.FrameRate = _FrameRate; }
	void SetFPS(int _FPS) { GlobalTime.FPS = _FPS; }

protected:
	
private:
	static Base_Deltatime GlobalTime; // 프로젝트의 델타 타임을 담는 변수
	static float FrameTime; // 프레임 산출을 위한 변수
	static float FrameLimit; // 프레임 상한을 위한 변수
	static float FrameRate; // 프레임 비율 저장 변수(float)
	static int FPS; // 프레임 비율 저장 변수(int)

	LARGE_INTEGER Current = LARGE_INTEGER();
	LARGE_INTEGER Before = LARGE_INTEGER();
	LARGE_INTEGER Second = LARGE_INTEGER();

	double DoubleDeltaTime = 0.0;
	float floatDeltaTime = 0.0;
	
};