#include "PrecompileHeader.h"
#include "Base_Math.h"

// 기본 수학 기호 지정
const float Base_Math::PIE = 3.141592653589793238462643383279502884197169399375105820974944f;
const float Base_Math::PIE2 = PIE * 2.0f;
const float Base_Math::DegreeToRadian = Base_Math::PIE / 180;
const float Base_Math::RadianToDegree = 180 / Base_Math::PIE;
// Degree : 각도 단위
// Radian : 라디안 단위(rad)

const float4 float4::LEFT = { -1.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::RIGHT = { 1.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::UP = { 0.0f, 1.0f, 0.0f, 1.0f };
const float4 float4::DOWN = { 0.0f, -1.0f, 0.0f, 1.0f };
const float4 float4::FORWARD = { 0.0f, 0.0f, 1.0f, 1.0f };
const float4 float4::BACK = { 0.0f, 0.0f, -1.0f, 1.0f };
const float4 float4::ONE = { 1.0f, 1.0f, 1.0f, 1.0f };
const float4 float4::ZERO = { 0.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::ZERONULL = { 0.0f, 0.0f, 0.0f, 0.0f };
const float4 float4::RED = { 1.0f, 0.0f, 0.0f, 1.0f };
const float4 float4::BLUE = { 0.0f, 0.0f, 1.0f, 1.0f };
const float4 float4::GREEN = { 0.0f, 1.0f, 0.0f, 1.0f };
const float4 float4::WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };
const float4 float4::BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };

const float4x4 float4x4::ZEROMATRIX = float4x4(float4::ZERONULL, float4::ZERONULL, float4::ZERONULL, float4::ZERONULL);

// float4랑 float4x4 곱하기
float4 float4::operator*(const class float4x4& _Other)
{
	float4 ReturnValue = DirectX::XMVector4Transform(*this, _Other);

	return ReturnValue;
}

// float4랑 float4x4 곱하기
float4& float4::operator*=(const class float4x4& _Other)
{
	DirectVector = DirectX::XMVector4Transform(*this, _Other);;
	return *this;
}

// 쿼터니언 값을 오일러 각(Degree)으로 변경
float4 float4::QuaternionToDegree()
{
	return QuaternionToRadian() * Base_Math::RadianToDegree;
}

// 쿼터니언 값을 오일러 라디안(Radian)으로 변경
float4 float4::QuaternionToRadian()
{
	float4 result;

	float sinrCosp = 2.0f * (w * z + x * y);
	float cosrCosp = 1.0f - 2.0f * (z * z + x * x);
	result.z = atan2f(sinrCosp, cosrCosp);

	float pitchTest = w * x - y * z;
	float asinThreshold = 0.4999995f;
	float sinp = 2.0f * pitchTest;

	if (pitchTest < -asinThreshold)
	{
		result.x = -(0.5f * Base_Math::PIE);
	}
	else if (pitchTest > asinThreshold)
	{
		result.x = (0.5f * Base_Math::PIE);
	}
	else
	{
		result.x = asinf(sinp);
	}

	float sinyCosp = 2.0f * (w * y + x * z);
	float cosyCosp = 1.0f - 2.0f * (x * x + y * y);
	result.y = atan2f(sinyCosp, cosyCosp);

	return result;
}

// 오일러 각(Degree)을 쿼터니언 값으로 변경
float4 float4::DegreeToQuaternion()
{
	float4 Return = DirectVector;
	Return *= Base_Math::DegreeToRadian;
	Return = DirectX::XMQuaternionRotationRollPitchYawFromVector(Return.DirectVector);
	return Return;
}

void float4::RotationXRadian(float _Radian)
{
	float4x4 Rot;
	Rot.RotationXRadian(_Radian);
	*this = *this * Rot;
}

void float4::RotationYRadian(float _Radian)
{
	float4x4 Rot;
	Rot.RotationYRadian(_Radian);
	*this = *this * Rot;
}

void float4::RotationZRadian(float _Radian)
{
	float4x4 Rot;
	Rot.RotationZRadian(_Radian);
	*this = *this * Rot;
}