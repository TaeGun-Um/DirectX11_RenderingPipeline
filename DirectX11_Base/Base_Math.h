#pragma once

#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

class Base_Math final
{
public:
	static const float PIE, PIE2, DegreeToRadiation, RadiationToDegree;

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

// float 4개를 활용할 수 있는 클래스
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

	float4 QuaternionToEulerDegree();
	float4 QuaternionToEulerRadiation();
	float4 EulerDegreeToQuaternion();

	void Normalize()
	{
		DirectVector = DirectX::XMVector3Normalize(*this);
	}

	// 자기가 길이 1로 줄어든 애를 리턴해주는것.
	float4 NormalizeReturn()
	{
		float4 Result = *this;
		Result.Normalize();
		return Result;
	}

private:

};

typedef float4 Quaternion;

// float 16개를 2차원 배열 형태로 활용할 수 있는 클래스
class float4x4 final
{
public:
	static const float4x4 Zero;

	static const int YCount = 4;
	static const int XCount = 4;

public:
	union
	{
		float Arr1D[16];
		float Arr2D[4][4];
		float4 ArrVector[4];
		DirectX::XMMATRIX DirectMatrix;

		struct
		{
			float _00;
			float _01;
			float _02;
			float _03;
			float _10;
			float _11;
			float _12;
			float _13;
			float _20;
			float _21;
			float _22;
			float _23;
			float _30;
			float _31;
			float _32;
			float _33;
		};
	};

	void Identity()
	{

		DirectMatrix = DirectX::XMMatrixIdentity();

		/*memset(Arr1D, 0, sizeof(float) * 16);
		Arr2D[0][0] = 1.0f;
		Arr2D[1][1] = 1.0f;
		Arr2D[2][2] = 1.0f;
		Arr2D[3][3] = 1.0f;*/
	}

	// 크기
	void Scale(const float4& _Value)
	{
		//100, 0 , 0 , 0
		// 0 ,100, 0 , 0
		// 0 , 0 ,100, 0
		// 0 , 0 , 0 , 1
		Identity();

		DirectMatrix = DirectX::XMMatrixScalingFromVector(_Value);

		//Arr2D[0][0] = _Value.x;
		//Arr2D[1][1] = _Value.y;
		//Arr2D[2][2] = _Value.z;
	}

	// 이동
	void Position(const float4& _Value)
	{
		//  0   1   2   3
		//0 0,  0 , 0 , 0
		//1 0 , 0,  0 , 0
		//2 0 , 0 , 0 , 0
		//3 200, 200 , 200 , 1

		Identity();

		DirectMatrix = DirectX::XMMatrixTranslationFromVector(_Value);

		//Arr2D[3][0] = _Value.x;
		//Arr2D[3][1] = _Value.y;
		//Arr2D[3][2] = _Value.z;
	}

	// 전치
	void Transpose()
	{
		// 0   , 0, -1
		// 100 , 1,  0
		// 1   , 0,  0

		// 0 , 100,  1
		// 0,    1,  0
		// -1 ,  0,  0

		DirectMatrix = DirectX::XMMatrixTranspose(*this);

		//float4x4 This = *this;
		//Identity();
		//for (size_t y = 0; y < YCount; y++)
		//{
		//	for (size_t x = 0; x < XCount; x++)
		//	{
		//		Arr2D[x][y] = This.Arr2D[y][x];
		//	}
		//}

	}

	void Compose(const float4& _Scale, const float4& _Quaternion, const float4& _Position)
	{
		float4 _Rot = _Quaternion;
		_Rot.QuaternionToEulerDegree();
		*this = DirectX::XMMatrixAffineTransformation(_Scale.DirectVector, _Rot.DirectVector, _Quaternion.DirectVector, _Position.DirectVector);
	}

	void Decompose(float4& _Scale, float4& _RotQuaternion, float4& _Pos) const
	{
		DirectX::XMMatrixDecompose(&_Scale.DirectVector, &_RotQuaternion.DirectVector, &_Pos.DirectVector, DirectMatrix);
	}

	void Inverse()
	{
		DirectMatrix = DirectX::XMMatrixInverse(nullptr, *this);
	}

	float4x4 InverseReturn() const
	{
		float4x4 Return = *this;
		Return.Inverse();
		return Return;
	}

	//                                           1280 / 720
	//                         수직시야각        화면의 종횡비   근평면      원평면
	// 3D를 2D로 투영시키기 위한 퍼스펙티브 행렬 구성
	void PerspectiveFovLH(float _FovAngle, float _AspectRatio, float _NearZ = 10.0f, float _FarZ = 10000.0f)
	{
		Identity();

		DirectMatrix = DirectX::XMMatrixPerspectiveFovLH(_FovAngle * Base_Math::DegreeToRadiation, _AspectRatio, _NearZ, _FarZ);

		//// 수직시야각이라면
		//float FOV = _FovAngle * GameEngineMath::DegToRad;
		////  _FovAngle * _AspectRatio;// 수평시야각 구하는법

		//// [0] [] [] []
		//// [] [0] [] []
		//// [] [] [0][1]
		//// [] [] [] [0]
		//Arr2D[2][3] = 1.0f;
		//Arr2D[3][3] = 0.0f;

		//Arr2D[0][0] = 1 / (tanf(FOV / 2.0f) * _AspectRatio);

		//// y 300
		//// z 5000
		//Arr2D[1][1] = 1 / tanf(FOV / 2.0f); // y / z

		//Arr2D[2][2] = _FarZ / (_FarZ - _NearZ);

		//Arr2D[3][2] = -( _NearZ * _FarZ) / (_FarZ - _NearZ);
	}

	//            화면의 너비
	void ViewPort(float _Width, float _Height, float _Left, float _Right, float _ZMin = 0.0f, float _ZMax = 1.0f)
	{
		Identity();

		// 모니터의 공간으로 변환시키는 행렬
		Arr2D[0][0] = _Width * 0.5f;
		Arr2D[1][1] = -_Height * 0.5f;
		Arr2D[2][2] = _ZMax != 0.0f ? 1.0f : _ZMin / _ZMax;

		Arr2D[3][0] = Arr2D[0][0] + _Left;
		Arr2D[3][1] = _Height * 0.5f + _Right;
		Arr2D[3][2] = _ZMax != 0.0f ? 0.0f : _ZMin / _ZMax;
		Arr2D[3][3] = 1.0f;
	}

	void LookToLH(const float4& _EyePos, const float4& _EyeDir, const float4& _EyeUp)
	{
		Identity();

		DirectMatrix = DirectX::XMMatrixLookToLH(_EyePos, _EyeDir, _EyeUp);


		//float4 EyePos = _EyePos;

		//// 합쳐져서 회전행렬이 된다.
		//float4 EyeDir = _EyeDir.NormalizeReturn();
		//float4 EyeUp = _EyeUp;
		//float4 Right = float4::Cross3DReturn(EyeUp, EyeDir);
		//Right.Normalize();

		//float4 UpVector = float4::Cross3DReturn(_EyeDir, Right);
		//Right.Normalize();

		//float4 NegEyePos = -_EyePos;

		//float D0Value = float4::DotProduct3D(Right, NegEyePos);
		//float D1Value = float4::DotProduct3D(UpVector, NegEyePos);
		//float D2Value = float4::DotProduct3D(EyeDir, NegEyePos);

		//// 여기서 내적을 사용합니다.
		//// x + 1;
		//// 역함수 혹은 역행렬이라고 부릅니다.
		//// x - 1;

		//ArrVector[0] = Right;
		//ArrVector[1] = UpVector;
		//ArrVector[2] = EyeDir;

		//Transpose();

		//ArrVector[3] = { D0Value, D1Value, D2Value, 0 };
	}



	//static float4x4 Transformation(float4 _Scale, float4 _RotQ, float4 _Pos)
	//{
	//	return DirectX::XMMatrixTransformation(float4::ZERO.DirectVector, float4::ZERO.DirectVector, _Scale.DirectVector, float4::ZERO.DirectVector, _RotQ.DirectVector, _Pos.DirectVector);
	//}



	//float4 ExtractScaling()
	//{
	//	float4 ret = float4::ZERO;

	//	float Tolerance = 1.e-8f;

	//	const float SquareSum0 = (Arr2D[0][0] * Arr2D[0][0]) + (Arr2D[0][1] * Arr2D[0][1]) + (Arr2D[0][2] * Arr2D[0][2]);
	//	const float SquareSum1 = (Arr2D[1][0] * Arr2D[1][0]) + (Arr2D[1][1] * Arr2D[1][1]) + (Arr2D[1][2] * Arr2D[1][2]);
	//	const float SquareSum2 = (Arr2D[2][0] * Arr2D[2][0]) + (Arr2D[2][1] * Arr2D[2][1]) + (Arr2D[2][2] * Arr2D[2][2]);

	//	if (SquareSum0 > Tolerance)
	//	{
	//		float Scale0 = sqrtf(SquareSum0);
	//		ret.x = Scale0;
	//		float InvScale0 = 1.f / Scale0;
	//		Arr2D[0][0] *= InvScale0;
	//		Arr2D[0][1] *= InvScale0;
	//		Arr2D[0][2] *= InvScale0;
	//	}
	//	else
	//	{
	//		ret.x = 0;
	//	}

	//	if (SquareSum1 > Tolerance)
	//	{
	//		float Scale1 = sqrtf(SquareSum1);
	//		ret.y = Scale1;
	//		float InvScale1 = 1.f / Scale1;
	//		Arr2D[1][0] *= InvScale1;
	//		Arr2D[1][1] *= InvScale1;
	//		Arr2D[1][2] *= InvScale1;
	//	}
	//	else
	//	{
	//		ret.y = 0;
	//	}

	//	if (SquareSum2 > Tolerance)
	//	{
	//		float Scale2 = sqrtf(SquareSum2);
	//		ret.z = Scale2;
	//		float InvScale2 = 1.f / Scale2;
	//		Arr2D[2][0] *= InvScale2;
	//		Arr2D[2][1] *= InvScale2;
	//		Arr2D[2][2] *= InvScale2;
	//	}
	//	else
	//	{
	//		ret.z = 0;
	//	}

	//	return ret;
	//}



	//inline float Determinant() const
	//{
	//	return	Arr2D[0][0] * (
	//		Arr2D[1][1] * (Arr2D[2][2] * Arr2D[3][3] - Arr2D[2][3] * Arr2D[3][2]) -
	//		Arr2D[2][1] * (Arr2D[1][2] * Arr2D[3][3] - Arr2D[1][3] * Arr2D[3][2]) +
	//		Arr2D[3][1] * (Arr2D[1][2] * Arr2D[2][3] - Arr2D[1][3] * Arr2D[2][2])
	//		) -
	//		Arr2D[1][0] * (
	//			Arr2D[0][1] * (Arr2D[2][2] * Arr2D[3][3] - Arr2D[2][3] * Arr2D[3][2]) -
	//			Arr2D[2][1] * (Arr2D[0][2] * Arr2D[3][3] - Arr2D[0][3] * Arr2D[3][2]) +
	//			Arr2D[3][1] * (Arr2D[0][2] * Arr2D[2][3] - Arr2D[0][3] * Arr2D[2][2])
	//			) +
	//		Arr2D[2][0] * (
	//			Arr2D[0][1] * (Arr2D[1][2] * Arr2D[3][3] - Arr2D[1][3] * Arr2D[3][2]) -
	//			Arr2D[1][1] * (Arr2D[0][2] * Arr2D[3][3] - Arr2D[0][3] * Arr2D[3][2]) +
	//			Arr2D[3][1] * (Arr2D[0][2] * Arr2D[1][3] - Arr2D[0][3] * Arr2D[1][2])
	//			) -
	//		Arr2D[3][0] * (
	//			Arr2D[0][1] * (Arr2D[1][2] * Arr2D[2][3] - Arr2D[1][3] * Arr2D[2][2]) -
	//			Arr2D[1][1] * (Arr2D[0][2] * Arr2D[2][3] - Arr2D[0][3] * Arr2D[2][2]) +
	//			Arr2D[2][1] * (Arr2D[0][2] * Arr2D[1][3] - Arr2D[0][3] * Arr2D[1][2])
	//			);
	//}



	//static float4x4 Affine(float4 _Scale, float4 _Rot, float4 _Pos)
	//{
	//	// _Rot.DirectVector 쿼터니온 입니다.
	//	return DirectX::XMMatrixAffineTransformation(_Scale.DirectVector, float4::ZERO.DirectVector, _Rot.DirectVector, _Pos.DirectVector);
	//}

	// z -x _EyeDir


	//void OrthographicLH(float _ScreenWidth, float _ScreenHeight, float _NearZ = 0.1f, float _FarZ = 10000.0f)
	//{
	//	Identity();

	//	DirectMatrix = DirectX::XMMatrixOrthographicLH(_ScreenWidth, _ScreenHeight, _NearZ, _FarZ);
	//}



	////            화면의 너비
	//void ViewPort(float _Width, float _Height, float _Left, float _Right, float _ZMin = 0.0f, float _ZMax = 1.0f)
	//{
	//	Identity();

	//	// 모니터의 공간으로 변환시키는 행렬
	//	Arr2D[0][0] = _Width * 0.5f;
	//	Arr2D[1][1] = -_Height * 0.5f;
	//	Arr2D[2][2] = _ZMax != 0.0f ? 1.0f : _ZMin / _ZMax;

	//	Arr2D[3][0] = Arr2D[0][0] + _Left;
	//	Arr2D[3][1] = _Height * 0.5f + _Right;
	//	Arr2D[3][2] = _ZMax != 0.0f ? 0.0f : _ZMin / _ZMax;
	//	Arr2D[3][3] = 1.0f;
	//}



	//void DecomposeRotQuaternion(float4& _RotQuaternion)
	//{
	//	float4 Temp0;
	//	float4 Temp1;

	//	DirectX::XMMatrixDecompose(&Temp0.DirectVector, &_RotQuaternion.DirectVector, &Temp1.DirectVector, DirectMatrix);
	//}



	//void DecomposePos(float4& _Pos)
	//{
	//	float4 Temp0;
	//	float4 Temp1;

	//	DirectX::XMMatrixDecompose(&Temp0.DirectVector, &Temp1.DirectVector, &_Pos.DirectVector, DirectMatrix);
	//}



	//void DecomposeScale(float4& _Scale)
	//{
	//	float4 Temp0;
	//	float4 Temp1;

	//	DirectX::XMMatrixDecompose(&_Scale.DirectVector, &Temp0.DirectVector, &Temp1.DirectVector, DirectMatrix);
	//}




	void RotationDegToXYZ(const float4& _Deg)
	{
		float4 Rot = _Deg * Base_Math::DegreeToRadiation;

		float4x4 RotX = DirectX::XMMatrixRotationX(Rot.x);
		float4x4 RotY = DirectX::XMMatrixRotationY(Rot.y);
		float4x4 RotZ = DirectX::XMMatrixRotationZ(Rot.z);

		*this = RotX * RotY * RotZ;

		// DirectMatrix = DirectX::XMMatrixRotationRollPitchYaw(Rot.x, Rot.y, Rot.z);
	}



	void RotationDeg(const float4& _Deg)
	{
		// 짐벌락 현상이라는 것이 있습니다.
		// 축이 겹치는 이상한 현상이 있는데 그 현상을 해결하려면
		// 곱하는 순서를 바꿔야 해결이 된다.
		// Rot = RotX * RotY * RotZ;

		// 기본적으로 쿼터니온 회전이라는걸 사용하는데 
		// 짐벌락 해결함.
		//float4x4 RotX = float4x4::Zero;
		//float4x4 RotY = float4x4::Zero;
		//float4x4 RotZ = float4x4::Zero;
		//RotX.RotationXDeg(_Deg.x);
		//RotY.RotationYDeg(_Deg.y);
		//RotZ.RotationZDeg(_Deg.z);

		// *this = RotX * RotY * RotZ;

		float4 Rot = _Deg * Base_Math::DegreeToRadiation;

		// DirectX::XMQuaternionRotationMatrix()

		DirectMatrix = DirectX::XMMatrixRotationRollPitchYaw(Rot.x, Rot.y, Rot.z);
	}

	void RotationXDeg(const float _Deg)
	{
		RotationXRad(_Deg * Base_Math::DegreeToRadiation);
	}

	void RotationXRad(const float _Rad)
	{
		Identity();

		DirectMatrix = DirectX::XMMatrixRotationX(_Rad);


		//Arr2D[1][1] = cosf(_Rad);
		//Arr2D[1][2] = sinf(_Rad);
		//Arr2D[2][1] = -sinf(_Rad);
		//Arr2D[2][2] = cosf(_Rad);
	}


	void RotationYDeg(const float _Deg)
	{
		RotationYRad(_Deg * Base_Math::DegreeToRadiation);
	}

	void RotationYRad(const float _Rad)
	{
		Identity();

		DirectMatrix = DirectX::XMMatrixRotationY(_Rad);

		//Arr2D[0][0] = cosf(_Rad);
		//Arr2D[0][2] = -sinf(_Rad);
		//Arr2D[2][0] = sinf(_Rad);
		//Arr2D[2][2] = cosf(_Rad);
	}

	void RotationZDeg(const float _Deg)
	{
		RotationZRad(_Deg * Base_Math::DegreeToRadiation);
	}

	void RotationZRad(const float _Rad)
	{
		Identity();

		DirectMatrix = DirectX::XMMatrixRotationZ(_Rad);

		//Arr2D[0][0] = cosf(_Rad);
		//Arr2D[0][1] = sinf(_Rad);
		//Arr2D[1][0] = -sinf(_Rad);
		//Arr2D[1][1] = cosf(_Rad);
	}


	float4x4 operator*(const float4x4& _Other)
	{
		//  0   0   0   0			   		  0   0   0   0	    0   0   0   0
		//  0,  0 , 0 , 0			   		  0,  0 , 0 , 0	    0,  0 , 0 , 0
		//  0 , 0,  0 , 0          *   		  0 , 0,  0 , 0  =  0 , 0,  0 , 0
		//  0 , 0 , 0 , 0			   		  0 , 0 , 0 , 0	    0 , 0 , 0 , 0

		//this->Arr2D;
		//_Other.Arr2D;

		//float4x4 Return = Zero;
		//for (size_t y = 0; y < YCount; y++)
		//{
		//	for (size_t x = 0; x < XCount; x++)
		//	{
		//		for (size_t j = 0; j < 4; j++)
		//		{
		//			Return.Arr2D[y][x] += Arr2D[y][j] * _Other.Arr2D[j][x];
		//		}
		//	}
		//}

		float4x4 Return = DirectX::XMMatrixMultiply(*this, _Other);

		return Return;
	}

	float4x4& operator*=(const float4x4& _Other)
	{
		// *this = *this * _Other;

		DirectMatrix = DirectX::XMMatrixMultiply(*this, _Other);

		return *this;
	}

	// w 가 0인 곱하기
	float4 TransformNormal(const float4& _Value)
	{
		return DirectX::XMVector3TransformNormal(_Value, *this);
	}

	// w 가 1인 곱하기
	float4 TransformCoord(const float4& _Value)
	{
		return DirectX::XMVector3TransformCoord(_Value, *this);
	}

	operator DirectX::FXMMATRIX() const
	{
		return DirectMatrix;
	}


	float4x4()
	{
		Identity();
	}

	float4x4(DirectX::FXMMATRIX _DirectMatrix)
		: DirectMatrix(_DirectMatrix)
	{

	}

	float4x4(float4 _x, float4 _y, float4 _z, float4 _w)
	{
		ArrVector[0] = _x;
		ArrVector[1] = _y;
		ArrVector[2] = _z;
		ArrVector[3] = _w;
	}

};
