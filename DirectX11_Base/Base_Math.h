#pragma once

#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

class Base_Math final
{
public:
	static const float PIE, PIE2, DegreeToRadian, RadianToDegree;

};

// float 4개를 활용할 수 있는 클래스
class float4 final
{
public:
	static const float4 LEFT, RIGHT, UP, DOWN, FORWARD, BACK, ONE, ZERO, ZERONULL, RED, BLUE, GREEN, WHITE, BLACK;
	
	// 생성자
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

	// 메모리 사용 정의
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

	// 간단한 return 함수
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

	// 연산자 정의
	operator DirectX::FXMVECTOR() const
	{
		// float4의 DirectX::XMVECTOR(DirectVector)에 대한 암시적 형변환 지원
		// 이게 있어야 float4로 DirectX의 Math함수 사용 가능
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

	// float4x4는 아래에 있기 때문에, cpp로 구현부를 옮겨서 활용
	float4 operator*(const class float4x4& _Other);
	float4& operator*=(const class float4x4& _Other);

	// 간단한 수학 관련 함수
	float4 QuaternionToDegree();	// 쿼터니언 값을 오일러 각(Degree)으로 변경
	float4 QuaternionToRadian();	// 쿼터니언 값을 오일러 라디안(Radian)으로 변경
	float4 DegreeToQuaternion();	// 오일러 각(Degree)을 쿼터니언 값으로 변경

	// 벡터(float4)를 단위벡터로 변경
	void Normalize()
	{
		DirectVector = DirectX::XMVector3Normalize(*this);
	}

	// 벡터(float4)를 단위벡터로 변경후 리턴
	float4 NormalizeReturn()
	{
		float4 Result = *this;
		Result.Normalize();
		return Result;
	}

	// Degree 단위로 X축 회전
	float4 RotationXDegeree(float _Degree)
	{
		float4 ReturnValue = *this;
		ReturnValue.RotationXRadian(_Degree * Base_Math::DegreeToRadian);
		return ReturnValue;
	}

	// Degree 단위로 Y축 회전
	float4 RotationYDegeree(float _Degree)
	{
		float4 ReturnValue = *this;
		ReturnValue.RotationYRadian(_Degree * Base_Math::DegreeToRadian);
		return ReturnValue;
	}

	// Degree 단위로 Z축 회전
	float4 RotationZDegeree(float _Degree)
	{
		float4 ReturnValue = *this;
		ReturnValue.RotationZRadian(_Degree * Base_Math::DegreeToRadian);
		return ReturnValue;
	}

	void RotationXRadian(float _Radian); // Radian 단위로 X축 회전
	void RotationYRadian(float _Radian); // Radian 단위로 Y축 회전
	void RotationZRadian(float _Radian); // Radian 단위로 Z축 회전

private:

};
// typedef float4 Quaternion;

// float 16개를 2차원 배열 형태로 활용할 수 있는 클래스
class float4x4 final
{
public:
	static const float4x4 ZEROMATRIX;

	// 생성자
	float4x4() 
	{ 
		Identity(); 
	}

	float4x4(float4 _x, float4 _y, float4 _z, float4 _w)
	{
		ArrVector[0] = _x;
		ArrVector[1] = _y;
		ArrVector[2] = _z;
		ArrVector[3] = _w;
	}

	float4x4(DirectX::FXMMATRIX _DirectMatrix)
		: DirectMatrix(_DirectMatrix) { }

	// 메모리 사용 정의
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

	// 연산자 정의
	operator DirectX::FXMMATRIX() const
	{
		return DirectMatrix;
	}

	float4x4 operator*(const float4x4& _Other)
	{
		float4x4 Return = DirectX::XMMatrixMultiply(*this, _Other);
		return Return;
	}

	float4x4& operator*=(const float4x4& _Other)
	{
		DirectMatrix = DirectX::XMMatrixMultiply(*this, _Other);
		return *this;
	}

	// 4x4 단위행렬(Identity Matrix) 생성
	//	| 1  0  0  0 |
	//	| 0  1  0  0 |
	//	| 0  0  1  0 |
	//	| 0  0  0  1 |
	void Identity()
	{
		DirectMatrix = DirectX::XMMatrixIdentity();
	}

	// 주어진 크기 벡터 기반으로 4x4 크기(Scale) 행렬 생성
	//	| Sx  0   0   0 |
	//	| 0   Sy  0   0 |
	//	| 0   0   Sz  0 |
	//	| 0   0   0   1 |
	void Scale(const float4& _Value)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixScalingFromVector(_Value);
	}

	// 주어진 이동 벡터 기반으로 4x4 이동(Translation) 행렬 생성
	//	| 1   0   0   0 |
	//	| 0   1   0   0 |
	//	| 0   0   1   0 |
	//	| Tx  Ty  Tz  1 |
	void Position(const float4& _Value)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixTranslationFromVector(_Value);
	}

	// X, Y, Z축을 기준으로 회전하는 4x4 회전(Ratation) 행렬 생성
	void RotationXDegeree(const float _Degree)
	{
		RotationXRadian(_Degree * Base_Math::DegreeToRadian);
	}

	void RotationYDegeree(const float _Degree)
	{
		RotationYRadian(_Degree * Base_Math::DegreeToRadian);
	}

	void RotationZDegeree(const float _Degree)
	{
		RotationZRadian(_Degree * Base_Math::DegreeToRadian);
	}

	// 	| 1   0        0			0 | X축을 중심으로 Y, Z가 회전
	// 	| 0  cosθ    sinθ		0 |
	// 	| 0 -sinθ    cosθ		0 |
	// 	| 0   0        0			1 |
	void RotationXRadian(const float _Radian)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixRotationX(_Radian);
	}

	//	| cosθ   0 - sinθ		0 | Y축을 중심으로 X, Z가 회전
	//	|  0     1    0			0 |
	//	| sinθ   0   cosθ		0 |
	//	|  0     0    0			1 |
	void RotationYRadian(const float _Radian)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixRotationY(_Radian);
	}
	
	//	| cosθ   sinθ   0		0 | Z축을 중심으로 X, Y가 회전
	//	| -sinθ  cosθ   0		0 |
	//	|  0      0        1		0 |
	//	|  0      0        0		1 |
	void RotationZRadian(const float _Radian)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixRotationZ(_Radian);
	}

	// X, Y, Z를 받아 회전, 회전 순서는 Z->Y->X
	void RotationDegree(const float4& _Degree)
	{
		float4 Rot = _Degree * Base_Math::DegreeToRadian;
		DirectMatrix = DirectX::XMMatrixRotationRollPitchYaw(Rot.x, Rot.y, Rot.z);
	}

	// 4x4 행렬의 전치(Transpose) 행렬 생성
	//	| a00 a01 a02 a03 |		| a00 a10 a20 a30 |
	//	| a10 a11 a12 a13 |		| a01 a11 a21 a31 |
	//	| a20 a21 a22 a23 |  ->	| a02 a12 a22 a32 |
	//	| a30 a31 a32 a33 |		| a03 a13 a23 a33 |
	void Transpose()
	{
		DirectMatrix = DirectX::XMMatrixTranspose(*this);
	}

	// 4x4 행렬의 역행렬(Inverse Matrix) 생성
	// 어떤 행렬 ??에 대해,  ??^(?1)이 존재하고 ??×??^(?1) = ?? (단위 행렬)이면, ??^(?1)은 ??의 역행렬
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

	// 크기, 회전, 이동 벡터를 조합하여 4x4 아핀 변환 행렬(Affine Transformation Matrix) 생성
	void Compose(const float4& _Scale, const float4& _Quaternion, const float4& _Position)
	{
		float4 _Rot = _Quaternion;
		_Rot.QuaternionToDegree();
		*this = DirectX::XMMatrixAffineTransformation(_Scale.DirectVector, _Rot.DirectVector, _Quaternion.DirectVector, _Position.DirectVector);
	}

	// 4x4 아핀 변환 행렬을 크기, 회전, 이동 벡터로 분해해줌
	void Decompose(float4& _Scale, float4& _Quaternion, float4& _Pos) const
	{
		DirectX::XMMatrixDecompose(&_Scale.DirectVector, &_Quaternion.DirectVector, &_Pos.DirectVector, DirectMatrix);
	}

	// 이동만 분해
	void DecomposePosition(float4& _Position)
	{
		float4 Temp0; float4 Temp1;
		DirectX::XMMatrixDecompose(&Temp0.DirectVector, &Temp1.DirectVector, &_Position.DirectVector, DirectMatrix);
	}

	// 크기만 분해
	void DecomposeScale(float4& _Scale)
	{
		float4 Temp0; float4 Temp1;
		DirectX::XMMatrixDecompose(&_Scale.DirectVector, &Temp0.DirectVector, &Temp1.DirectVector, DirectMatrix);
	}

	// 회전만 분해
	void DecomposeRotQuaternion(float4& _Quaternion)
	{
		float4 Temp0; float4 Temp1;
		DirectX::XMMatrixDecompose(&Temp0.DirectVector, &_Quaternion.DirectVector, &Temp1.DirectVector, DirectMatrix);
	}

	// 카메라의 위치, 바라보는 방향, 위쪽 방향을 기준으로 월드 -> 뷰 변환 행렬(View Matrix) 생성
	void LookToLH(const float4& _EyePos, const float4& _EyeDir, const float4& _EyeUp)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixLookToLH(_EyePos, _EyeDir, _EyeUp);
	}

	// 좌수 좌표계 기반의 직교 투영 행렬 생성
	void OrthographicLH(float _ScreenWidth, float _ScreenHeight, float _NearZ = 0.1f, float _FarZ = 10000.0f)
	{
		// 1. _ScreenWidth : 투영 공간의 가로 폭
		// 2. _ScreenHeight : 투영 공간의 세로 높이
		// 3. _NearZ : 근평면 거리
		// 4. _FarZ : 원평면 거리
		Identity();
		DirectMatrix = DirectX::XMMatrixOrthographicLH(_ScreenWidth, _ScreenHeight, _NearZ, _FarZ);
	}

	// 좌수 좌표계 기반의 원근 투영 행렬 생성
	//	| yScale       0          0			0      | -> yScale = 1 / tan(FovY / 2)
	//	| 0          xScale       0			0      | -> xScale = yScale / AspectRatio
	//	| 0              0         Z압축		1      | -> FarZ / (FarZ - NearZ)
	//	| 0              0         Z보정		0	  	| -> -NearZ * FarZ / (FarZ - NearZ)
	void PerspectiveFovLH(float _FovAngle, float _AspectRatio, float _NearZ = 10.0f, float _FarZ = 10000.0f)
	{
		// 1. _FovAngle : 세로 방향 시야각(radian)
		// 2. _AspectRatio : 종횡비 (가로/세로)
		// 3. _NearZ : 근평면(z최솟값)
		// 4. _FarZ : 원평면(z최댓값)
		Identity();
		DirectMatrix = DirectX::XMMatrixPerspectiveFovLH(_FovAngle * Base_Math::DegreeToRadian, _AspectRatio, _NearZ, _FarZ);
		
		// [Z압축]
		// 원래 뷰 공간에 대해 z값이 NearZ이상, FarZ이하로 존재하는 요소라면, 이것을 NDC 공간의 0이상, 1이하 z로 매핑해야 함
		// 비선형 깊이 변환을 수행하는 부분
		
		// [Z보정]
		// 투영 좌표계에서 z를 w로 나눌 때 깊이값이 역수 비례되도록 설계되어야함
		//  -> 가까운 물체는 z가 작고, w도 작아서 깊이 클리핑 통과
		//  -> 멀리 있는 물체는 z / w 가 매우 작아져 원근감 발생
		// 고로 z축 원근 왜곡을 조정하는 항
		
		// [(2, 3) 요소가 1인 이유]
		// 3D에서 정점은 ( x, y, z, 1 ) 형태로 표현되고, 투영 행렬을 곱하면 ( xp, yp, zp, w )가 됨
		// 이때, zp / w 를 통해 원근감을 적용해야 함, 이때 z값을 적용하여 w에 포함되도록 하기 위해 원근 나눗셈으로 활용
	}

	// 모니터의 공간으로 변환시키는 뷰포트 행렬(ViewPort Matrix) 생성
	void ViewPort(float _Width, float _Height, float _Left, float _Right, float _ZMin = 0.0f, float _ZMax = 1.0f)
	{
		Identity();
		
		Arr2D[0][0] = _Width * 0.5f;
		Arr2D[1][1] = -_Height * 0.5f;
		Arr2D[2][2] = _ZMax != 0.0f ? 1.0f : _ZMin / _ZMax;

		Arr2D[3][0] = Arr2D[0][0] + _Left;
		Arr2D[3][1] = _Height * 0.5f + _Right;
		Arr2D[3][2] = _ZMax != 0.0f ? 0.0f : _ZMin / _ZMax;
		Arr2D[3][3] = 1.0f;
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

};
