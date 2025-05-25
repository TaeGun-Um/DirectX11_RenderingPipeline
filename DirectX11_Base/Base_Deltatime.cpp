#include "PrecompileHeader.h"
#include "Base_Deltatime.h"
#include "Base_Windows.h"

Base_Deltatime Base_Deltatime::GlobalTime;
float Base_Deltatime::FrameTime = 0.f;
float Base_Deltatime::FrameLimit = 1.f / 60.f;
float Base_Deltatime::FrameRate = 0.f;
int Base_Deltatime::FPS = 0;

Base_Deltatime::Base_Deltatime()
{
	QueryPerformanceCounter(&Before); // Windows에서 정밀한 시간 측정을 위해 사용되는 함수
	QueryPerformanceFrequency(&Second); // 고해상도 카운터의 1초당 횟수(주파수)를 구하는 함수
}

Base_Deltatime::~Base_Deltatime()
{
}

float Base_Deltatime::TimeCheck()
{
	// 포커스 잃으면 기준 시간 강제 갱신 (시간 흐름 무시)
	if (false == Base_Windows::GetIsWindowFocus())
	{
		QueryPerformanceCounter(&Before);
		floatDeltaTime = 0.0f;
		return 0.0f;
	}

	QueryPerformanceCounter(&Current);
	DoubleDeltaTime = (static_cast<double>(Current.QuadPart) - static_cast<double>(Before.QuadPart)) / static_cast<double>(Second.QuadPart);
	floatDeltaTime = static_cast<float>(DoubleDeltaTime);
	Before.QuadPart = Current.QuadPart;

	return floatDeltaTime;
}

void Base_Deltatime::TimeReset()
{
	QueryPerformanceCounter(&Before);
}
