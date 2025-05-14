#pragma once

#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

class Base_Math final
{
public:
	// static std::vector<unsigned int> GetDigits(int _Value);
	// static unsigned int GetLenth(int _Value);
	static const float PIE, PIE2, DegToRad, RadToDeg;

	static float Lerp(float p1, float p2, float Time)
	{
		return (1.0f - Time) * p1 + Time * p2;
	}

	static float LerpLimit(float p1, float p2, float Time)
	{
		if (1.0f <= Time)
		{
			Time = 1.0f;
		}

		return Lerp(p1, p2, Time);
	}

private:
	virtual ~Base_Math() = 0;
};

class float4 final
{
public:
	static const float4 LEFT, RIGHT, UP, DOWN, FORWARD, BACK, ONE, ZERO, ZERONULL, RED, BLUE, GREEN, WHITE, BLACK;
	
	float4()
		: x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
	float4(float _x, float _y)
		: x(_x), y(_y), z(0.0f), w(1.0f) {}
	float4(float _x, float _y, float _z)
		: x(_x), y(_y), z(_z), w(1.0f) {}
	float4(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w) {}
	float4(DirectX::FXMVECTOR _Vector)
		: DirectVector(_Vector) {}

	union
	{
		struct { float x; float y; float z;	 float w; };
		struct { float r; float g; float b; float a; };
		struct { float PositionX; float PositionY;	float SizeX; float SizeY; };

		float Arr1D[4];
		DirectX::XMFLOAT3 DirectFloat3;
		DirectX::XMFLOAT4 DirectFloat4;
		DirectX::XMVECTOR DirectVector;
	};

	float hx() const { return x * 0.5f; }
	float hy() const { return y * 0.5f; }
	float hz() const { return z * 0.5f; }
	float hw() const { return w * 0.5f; }

	int ix() const	{ return static_cast<int>(x); }
	int iy() const { return static_cast<int>(y); }
	int iz() const { return static_cast<int>(z); }
	int iw() const { return static_cast<int>(w); }

	int hix() const { return static_cast<int>(x * 0.5f); }
	int hiy() const { return static_cast<int>(y * 0.5f); }
	int hiz() const { return static_cast<int>(z * 0.5f); }
	int hiw() const { return static_cast<int>(w * 0.5f); }

	unsigned int uix() const { return static_cast<unsigned int>(x); }
	unsigned int uiy() const { return static_cast<unsigned int>(y); }
	unsigned int uiz() const { return static_cast<unsigned int>(z); }
	unsigned int uiw() const { return static_cast<unsigned int>(w); }

	unsigned int huix() const { return static_cast<unsigned int>(x * 0.5f); }
	unsigned int huiy() const { return static_cast<unsigned int>(y * 0.5f); }
	unsigned int huiz() const { return static_cast<unsigned int>(z * 0.5f); }
	unsigned int huiw() const { return static_cast<unsigned int>(w * 0.5f); }

	float4 half() const { return { x * 0.5f,y * 0.5f,z * 0.5f,w }; }
	bool IsZero() const { return x == 0.0f && y == 0.0f && z == 0.0f; }
	float Size() const {	return sqrtf(x * x + y * y + z * z); }

	operator DirectX::FXMVECTOR() const
	{
		return DirectVector;
	}

	bool operator ==(const float4& _Value) const
	{
		return _Value.x == x && _Value.y == y && _Value.z == z;
	}

	float4 operator *(const float _Value) const
	{
		return DirectX::XMVectorMultiply(*this, float4{ _Value , _Value , _Value , 1.0f });
	}

	float4 operator +(const float4& _Value) const
	{
		float PrevW = w;
		float4 Return = DirectX::XMVectorAdd(*this, _Value);
		Return.w = PrevW;
		return Return;
	}

	float4 operator -(const float4& _Value) const
	{
		float PrevW = w;
		float4 Return = DirectX::XMVectorAdd(*this, -_Value);
		Return.w = PrevW;
		return Return;
	}

	float4 operator *(const float4& _Value) const
	{
		float PrevW = w;
		float4 Return = DirectX::XMVectorMultiply(*this, _Value);
		Return.w = PrevW;
		return Return;
	}

	float4 operator /(const float4& _Value) const
	{
		float PrevW = w;
		float4 Return = DirectX::XMVectorDivide(*this, _Value);
		Return.w = PrevW;
		return Return;
	}

	float4 operator /(const float _Value) const
	{
		float PrevW = w;
		float4 Return = DirectX::XMVectorDivide(*this, { _Value , _Value , _Value , _Value });
		Return.w = PrevW;
		return Return;
	}

	float4 operator -() const
	{
		return { -x, -y, -z, w };
	}

	float4& operator +=(const float4& _Other)
	{
		*this = *this + _Other;
		return *this;
	}

	float4& operator *=(const float _Value)
	{
		*this = *this * _Value;
		return *this;
	}

	float4& operator /=(const float _Value)
	{
		*this = *this / _Value;
		return *this;
	}

	float4& operator *=(const float4& _Other)
	{
		*this = *this * _Other;
		return *this;
	}

	float4& operator -=(const float4& _Other)
	{
		*this = *this - _Other;
		return *this;
	}

	float4& operator /=(const float4& _Other)
	{
		*this = *this / _Other;
		return *this;
	}

	//float4 operator*(const class float4x4& _Other);
	//float4& operator*=(const class float4x4& _Other);

private:

};