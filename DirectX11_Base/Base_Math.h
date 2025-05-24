#pragma once

#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

class Base_Math final
{
public:
	static const float PIE, PIE2, DegreeToRadian, RadianToDegree;

};

// float 4���� Ȱ���� �� �ִ� Ŭ����
class float4 final
{
public:
	static const float4 LEFT, RIGHT, UP, DOWN, FORWARD, BACK, ONE, ZERO, ZERONULL, RED, BLUE, GREEN, WHITE, BLACK;
	
	// ������
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

	// �޸� ��� ����
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

	// ������ return �Լ�
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

	// ������ ����
	operator DirectX::FXMVECTOR() const
	{
		// float4�� DirectX::XMVECTOR(DirectVector)�� ���� �Ͻ��� ����ȯ ����
		// �̰� �־�� float4�� DirectX�� Math�Լ� ��� ����
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

	// float4x4�� �Ʒ��� �ֱ� ������, cpp�� �����θ� �Űܼ� Ȱ��
	float4 operator*(const class float4x4& _Other);
	float4& operator*=(const class float4x4& _Other);

	// ������ ���� ���� �Լ�
	float4 QuaternionToDegree();	// ���ʹϾ� ���� ���Ϸ� ��(Degree)���� ����
	float4 QuaternionToRadian();	// ���ʹϾ� ���� ���Ϸ� ����(Radian)���� ����
	float4 DegreeToQuaternion();	// ���Ϸ� ��(Degree)�� ���ʹϾ� ������ ����

	// ����(float4)�� �������ͷ� ����
	void Normalize()
	{
		DirectVector = DirectX::XMVector3Normalize(*this);
	}

	// ����(float4)�� �������ͷ� ������ ����
	float4 NormalizeReturn()
	{
		float4 Result = *this;
		Result.Normalize();
		return Result;
	}

	// Degree ������ X�� ȸ��
	float4 RotationXDegeree(float _Degree)
	{
		float4 ReturnValue = *this;
		ReturnValue.RotationXRadian(_Degree * Base_Math::DegreeToRadian);
		return ReturnValue;
	}

	// Degree ������ Y�� ȸ��
	float4 RotationYDegeree(float _Degree)
	{
		float4 ReturnValue = *this;
		ReturnValue.RotationYRadian(_Degree * Base_Math::DegreeToRadian);
		return ReturnValue;
	}

	// Degree ������ Z�� ȸ��
	float4 RotationZDegeree(float _Degree)
	{
		float4 ReturnValue = *this;
		ReturnValue.RotationZRadian(_Degree * Base_Math::DegreeToRadian);
		return ReturnValue;
	}

	void RotationXRadian(float _Radian); // Radian ������ X�� ȸ��
	void RotationYRadian(float _Radian); // Radian ������ Y�� ȸ��
	void RotationZRadian(float _Radian); // Radian ������ Z�� ȸ��

private:

};
// typedef float4 Quaternion;

// float 16���� 2���� �迭 ���·� Ȱ���� �� �ִ� Ŭ����
class float4x4 final
{
public:
	static const float4x4 ZEROMATRIX;

	// ������
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

	// �޸� ��� ����
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

	// ������ ����
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

	// 4x4 �������(Identity Matrix) ����
	//	| 1  0  0  0 |
	//	| 0  1  0  0 |
	//	| 0  0  1  0 |
	//	| 0  0  0  1 |
	void Identity()
	{
		DirectMatrix = DirectX::XMMatrixIdentity();
	}

	// �־��� ũ�� ���� ������� 4x4 ũ��(Scale) ��� ����
	//	| Sx  0   0   0 |
	//	| 0   Sy  0   0 |
	//	| 0   0   Sz  0 |
	//	| 0   0   0   1 |
	void Scale(const float4& _Value)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixScalingFromVector(_Value);
	}

	// �־��� �̵� ���� ������� 4x4 �̵�(Translation) ��� ����
	//	| 1   0   0   0 |
	//	| 0   1   0   0 |
	//	| 0   0   1   0 |
	//	| Tx  Ty  Tz  1 |
	void Position(const float4& _Value)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixTranslationFromVector(_Value);
	}

	// X, Y, Z���� �������� ȸ���ϴ� 4x4 ȸ��(Ratation) ��� ����
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

	// 	| 1   0        0			0 | X���� �߽����� Y, Z�� ȸ��
	// 	| 0  cos��    sin��		0 |
	// 	| 0 -sin��    cos��		0 |
	// 	| 0   0        0			1 |
	void RotationXRadian(const float _Radian)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixRotationX(_Radian);
	}

	//	| cos��   0 - sin��		0 | Y���� �߽����� X, Z�� ȸ��
	//	|  0     1    0			0 |
	//	| sin��   0   cos��		0 |
	//	|  0     0    0			1 |
	void RotationYRadian(const float _Radian)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixRotationY(_Radian);
	}
	
	//	| cos��   sin��   0		0 | Z���� �߽����� X, Y�� ȸ��
	//	| -sin��  cos��   0		0 |
	//	|  0      0        1		0 |
	//	|  0      0        0		1 |
	void RotationZRadian(const float _Radian)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixRotationZ(_Radian);
	}

	// X, Y, Z�� �޾� ȸ��, ȸ�� ������ Z->Y->X
	void RotationDegree(const float4& _Degree)
	{
		float4 Rot = _Degree * Base_Math::DegreeToRadian;
		DirectMatrix = DirectX::XMMatrixRotationRollPitchYaw(Rot.x, Rot.y, Rot.z);
	}

	// 4x4 ����� ��ġ(Transpose) ��� ����
	//	| a00 a01 a02 a03 |		| a00 a10 a20 a30 |
	//	| a10 a11 a12 a13 |		| a01 a11 a21 a31 |
	//	| a20 a21 a22 a23 |  ->	| a02 a12 a22 a32 |
	//	| a30 a31 a32 a33 |		| a03 a13 a23 a33 |
	void Transpose()
	{
		DirectMatrix = DirectX::XMMatrixTranspose(*this);
	}

	// 4x4 ����� �����(Inverse Matrix) ����
	// � ��� ??�� ����,  ??^(?1)�� �����ϰ� ??��??^(?1) = ?? (���� ���)�̸�, ??^(?1)�� ??�� �����
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

	// ũ��, ȸ��, �̵� ���͸� �����Ͽ� 4x4 ���� ��ȯ ���(Affine Transformation Matrix) ����
	void Compose(const float4& _Scale, const float4& _Quaternion, const float4& _Position)
	{
		float4 _Rot = _Quaternion;
		_Rot.QuaternionToDegree();
		*this = DirectX::XMMatrixAffineTransformation(_Scale.DirectVector, _Rot.DirectVector, _Quaternion.DirectVector, _Position.DirectVector);
	}

	// 4x4 ���� ��ȯ ����� ũ��, ȸ��, �̵� ���ͷ� ��������
	void Decompose(float4& _Scale, float4& _Quaternion, float4& _Pos) const
	{
		DirectX::XMMatrixDecompose(&_Scale.DirectVector, &_Quaternion.DirectVector, &_Pos.DirectVector, DirectMatrix);
	}

	// �̵��� ����
	void DecomposePosition(float4& _Position)
	{
		float4 Temp0; float4 Temp1;
		DirectX::XMMatrixDecompose(&Temp0.DirectVector, &Temp1.DirectVector, &_Position.DirectVector, DirectMatrix);
	}

	// ũ�⸸ ����
	void DecomposeScale(float4& _Scale)
	{
		float4 Temp0; float4 Temp1;
		DirectX::XMMatrixDecompose(&_Scale.DirectVector, &Temp0.DirectVector, &Temp1.DirectVector, DirectMatrix);
	}

	// ȸ���� ����
	void DecomposeRotQuaternion(float4& _Quaternion)
	{
		float4 Temp0; float4 Temp1;
		DirectX::XMMatrixDecompose(&Temp0.DirectVector, &_Quaternion.DirectVector, &Temp1.DirectVector, DirectMatrix);
	}

	// ī�޶��� ��ġ, �ٶ󺸴� ����, ���� ������ �������� ���� -> �� ��ȯ ���(View Matrix) ����
	void LookToLH(const float4& _EyePos, const float4& _EyeDir, const float4& _EyeUp)
	{
		Identity();
		DirectMatrix = DirectX::XMMatrixLookToLH(_EyePos, _EyeDir, _EyeUp);
	}

	// �¼� ��ǥ�� ����� ���� ���� ��� ����
	void OrthographicLH(float _ScreenWidth, float _ScreenHeight, float _NearZ = 0.1f, float _FarZ = 10000.0f)
	{
		// 1. _ScreenWidth : ���� ������ ���� ��
		// 2. _ScreenHeight : ���� ������ ���� ����
		// 3. _NearZ : ����� �Ÿ�
		// 4. _FarZ : ����� �Ÿ�
		Identity();
		DirectMatrix = DirectX::XMMatrixOrthographicLH(_ScreenWidth, _ScreenHeight, _NearZ, _FarZ);
	}

	// �¼� ��ǥ�� ����� ���� ���� ��� ����
	//	| yScale       0          0			0      | -> yScale = 1 / tan(FovY / 2)
	//	| 0          xScale       0			0      | -> xScale = yScale / AspectRatio
	//	| 0              0         Z����		1      | -> FarZ / (FarZ - NearZ)
	//	| 0              0         Z����		0	  	| -> -NearZ * FarZ / (FarZ - NearZ)
	void PerspectiveFovLH(float _FovAngle, float _AspectRatio, float _NearZ = 10.0f, float _FarZ = 10000.0f)
	{
		// 1. _FovAngle : ���� ���� �þ߰�(radian)
		// 2. _AspectRatio : ��Ⱦ�� (����/����)
		// 3. _NearZ : �����(z�ּڰ�)
		// 4. _FarZ : �����(z�ִ�)
		Identity();
		DirectMatrix = DirectX::XMMatrixPerspectiveFovLH(_FovAngle * Base_Math::DegreeToRadian, _AspectRatio, _NearZ, _FarZ);
		
		// [Z����]
		// ���� �� ������ ���� z���� NearZ�̻�, FarZ���Ϸ� �����ϴ� ��Ҷ��, �̰��� NDC ������ 0�̻�, 1���� z�� �����ؾ� ��
		// ���� ���� ��ȯ�� �����ϴ� �κ�
		
		// [Z����]
		// ���� ��ǥ�迡�� z�� w�� ���� �� ���̰��� ���� ��ʵǵ��� ����Ǿ����
		//  -> ����� ��ü�� z�� �۰�, w�� �۾Ƽ� ���� Ŭ���� ���
		//  -> �ָ� �ִ� ��ü�� z / w �� �ſ� �۾��� ���ٰ� �߻�
		// ��� z�� ���� �ְ��� �����ϴ� ��
		
		// [(2, 3) ��Ұ� 1�� ����]
		// 3D���� ������ ( x, y, z, 1 ) ���·� ǥ���ǰ�, ���� ����� ���ϸ� ( xp, yp, zp, w )�� ��
		// �̶�, zp / w �� ���� ���ٰ��� �����ؾ� ��, �̶� z���� �����Ͽ� w�� ���Եǵ��� �ϱ� ���� ���� ���������� Ȱ��
	}

	// ������� �������� ��ȯ��Ű�� ����Ʈ ���(ViewPort Matrix) ����
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

	// w �� 0�� ���ϱ�
	float4 TransformNormal(const float4& _Value)
	{
		return DirectX::XMVector3TransformNormal(_Value, *this);
	}

	// w �� 1�� ���ϱ�
	float4 TransformCoord(const float4& _Value)
	{
		return DirectX::XMVector3TransformCoord(_Value, *this);
	}

};
