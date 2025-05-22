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
	QueryPerformanceCounter(&Before); // Windows���� ������ �ð� ������ ���� ���Ǵ� �Լ�
	QueryPerformanceFrequency(&Second); // ���ػ� ī������ 1�ʴ� Ƚ��(���ļ�)�� ���ϴ� �Լ�
}

Base_Deltatime::~Base_Deltatime()
{
}

float Base_Deltatime::TimeCheck()
{
	if (false == Base_Windows::GetIsWindowFocus())
	{
		QueryPerformanceCounter(&Before);
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
