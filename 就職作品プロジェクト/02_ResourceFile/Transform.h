#pragma once
#include <cmath>
#include <SimpleMath.h>

using namespace DirectX;
using namespace SimpleMath;

/// @brief Nは西口の略 DirectXMathの型Vector3が被るのを避ける
/// @brief Vector3は3次元ベクトルを表すクラス
struct NVector3
{
	float x;
	float y;
	float z;

	// コンストラクタ
	NVector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
	
	//====================================
	//			演算子オーバーロード
	//====================================
	
	// float 一括代入
	NVector3& operator=(float value)
	{
		x = y = z = value;
		return *this;
	}

	// 単項スカラー演算子（新しいVector3を返す）
	NVector3 operator+(float value) const { return NVector3(x + value, y + value, z + value); }
	NVector3 operator-(float value) const { return NVector3(x - value, y - value, z - value); }
	NVector3 operator*(float value) const { return NVector3(x * value, y * value, z * value); }
	NVector3 operator/(float value) const { return NVector3(x / value, y / value, z / value); }
	NVector3 operator%(float value) const { return NVector3(fmodf(x, value), fmodf(y, value), fmodf(z, value)); }

	// 単項スカラー演算子（代入）
	NVector3& operator+=(float value) { x += value; y += value; z += value; return *this; }
	NVector3& operator-=(float value) { x -= value; y -= value; z -= value; return *this; }
	NVector3& operator*=(float value) { x *= value; y *= value; z *= value; return *this; }
	NVector3& operator/=(float value) { x /= value; y /= value; z /= value; return *this; }
	NVector3& operator%=(float value) { x = fmodf(x, value); y = fmodf(y, value); z = fmodf(z, value); return *this; }

	// ベクトル同士の演算

	NVector3 operator+(const NVector3& rhs) const { return NVector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	NVector3 operator-(const NVector3& rhs) const { return NVector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	NVector3 operator*(const NVector3& rhs) const { return NVector3(x * rhs.x, y * rhs.y, z * rhs.z); }
	NVector3 operator/(const NVector3& rhs) const { return NVector3(x / rhs.x, y / rhs.y, z / rhs.z); }

	NVector3& operator+=(const NVector3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	NVector3& operator-=(const NVector3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	NVector3& operator*=(const NVector3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	NVector3& operator/=(const NVector3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }

	// ノルム
	float Length() 
	{
		return sqrtf(x * x + y * y + z * z);
	}
	// 正規化
	NVector3 Normalize()
	{
		NVector3 result;
		float length = Length();
		if (length > 0.0f)
		{
			x /= length;	
			y /= length;	
			z /= length;	
		}
		else
		{
			x = y = z = 0.0f; // ゼロベクトルの場合はそのまま
		}

		result.x = x;
		result.y = y;
		result.z = z;

		return result;
	}
	// 内積
	float Dot(const NVector3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}
	// 外積
	NVector3 Cross(const NVector3& rhs) const
	{
		return NVector3(
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x
		);
	}
};

class Transform
{
protected:

	NVector3 m_Position;		// 座標
	NVector3 m_Rotation;		// 角度
	NVector3 m_Scale;			// 大きさ

public:

	/// =================================================
	///                  コンストラクタ
	/// =================================================
	Transform() : m_Position(0.0f, 0.0f, 0.0f),
				  m_Rotation(0.0f, 0.0f, 0.0f),
				  m_Scale   (1.0f, 1.0f, 1.0f) {
	};

	/// =================================================
	///                    セッター
	/// =================================================
	void SetTransform(const Transform _Transform);
	void SetTransform(const NVector3 _Pos, const NVector3 _Rotate, const NVector3 _Scale);

	void SetPos(const NVector3 _Pos);
	void SetPos(const float _PosX, const float _PosY, const float _PosZ);
	void SetPos(const float _Pos);

	void SetRotate(const NVector3 _Rotate);
	void SetRotate(const float _RotateX, const float _RotateY, const float _RotateZ);
	void SetRotate(const float _Rotate);

	void SetScale(const NVector3 _Scale);
	void SetScale(const float _ScaleX, const float _ScaleY, const float _ScaleZ);
	void SetScale(const float _Scale);

	/// =================================================
	///                    ゲッター
	/// =================================================
	Transform GetTransform() const;
	NVector3   GetPos()		const { return m_Position; }
	NVector3   GetRotate()  const { return m_Rotation; }
	NVector3   GetScale()   const { return m_Scale   ; }
};
