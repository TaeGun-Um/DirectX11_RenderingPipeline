#pragma once

// ������Ʈ�� Deltatime ������ ���� Ŭ����
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

	float TimeCheck(); // �ð� üũ
	void TimeReset(); // �ð� ����

	static void ResetFrameTime() { GlobalTime.FrameTime = 0.0f; }

	// Getter, Setter
	static Base_Deltatime& GetGlobalTime() { return GlobalTime; }
	float& GetFrameTime() { return GlobalTime.FrameTime; }
	float& GetFrameLimit() { return GlobalTime.FrameLimit; }
	float GetDeltaTime() const { return min(GlobalTime.floatDeltaTime, 0.1f); } // ������ ��� ��, clamp�� ���� ���� ���
	void SetDeltaTime(float _DeltaTime) { GlobalTime.floatDeltaTime = _DeltaTime; }
	void AddFrameTime(float _FrameTime) { GlobalTime.FrameTime += _FrameTime; }
	void SetFrameRate(float _FrameRate) { GlobalTime.FrameRate = _FrameRate; }
	void SetFPS(int _FPS) { GlobalTime.FPS = _FPS; }

protected:
	
private:
	static Base_Deltatime GlobalTime; // ������Ʈ�� ��Ÿ Ÿ���� ��� ����
	static float FrameTime; // ������ ������ ���� ����
	static float FrameLimit; // ������ ������ ���� ����
	static float FrameRate; // ������ ���� ���� ����(float)
	static int FPS; // ������ ���� ���� ����(int)

	LARGE_INTEGER Current = LARGE_INTEGER();
	LARGE_INTEGER Before = LARGE_INTEGER();
	LARGE_INTEGER Second = LARGE_INTEGER();

	double DoubleDeltaTime = 0.0;
	float floatDeltaTime = 0.0;
	
};