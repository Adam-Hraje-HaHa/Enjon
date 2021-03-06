#include "Math/Vec4.h"

namespace Enjon {
 
	std::ostream& operator<<(std::ostream& stream, Vec4& vector)
	{
		stream << "Vector4f: (" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";
		return stream;
	}

	Vec4& Vec4::Add(const Vec4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	} 
	
	Vec4& Vec4::Subtract(const Vec4& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}
	
	Vec4& Vec4::Multiply(const Vec4& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}
	
	Vec4& Vec4::Divide(const Vec4& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}

	bool operator==(const Vec4& left, const Vec4& right) 
	{
		return (left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w);
	}

	bool operator!=(const Vec4& left, const Vec4& right) 
	{
		return !(left == right);
	}

	Vec4 operator+(Vec4 left, const Vec4& right)
	{
		return left.Add(right);
	}
	
	Vec4 operator-(Vec4 left, const Vec4& right)
	{
		return left.Subtract(right);
	}
	
	Vec4 operator*(Vec4 left, const Vec4& right)
	{
		return left.Multiply(right);
	}
	
	Vec4 operator/(Vec4 left, const Vec4& right)
	{
		return left.Divide(right);
	}

	Vec4& Vec4::operator+=(const Vec4& other)
	{
		return Add(other);
	} 
	
	Vec4& Vec4::operator-=(const Vec4& other)
	{
		return Subtract(other);
	} 
	
	Vec4& Vec4::operator*=(const Vec4& other)
	{
		return Multiply(other);
	} 
	
	Vec4& Vec4::operator/=(const Vec4& other)
	{
		return Divide(other);
	} 

	Vec4 operator*(Vec4 left, const float& scalar)
	{
		return Vec4(left.x * scalar, left.y * scalar, left.z * scalar, left.w * scalar);
	}

	void operator*=(Vec4& left, const float& scalar)
	{
		left.x *= scalar;
		left.y *= scalar;
		left.z *= scalar;
		left.w *= scalar;
	}

	void operator/=(Vec4& left, float scalar)
	{
		left.x /= scalar;
		left.y /= scalar;
		left.z /= scalar;
		left.w /= scalar;
	}

	float Vec4::Length() const 
	{
		return sqrt((x * x) + (y * y) + (z * z) + (w * w));
	}
		
	Vec4 operator/(Vec4 left, float value)
	{
		return Vec4(left.x / value, left.y / value, left.z / value, left.w / value);
	}

	Vec4 operator*(const float& scalar, Vec4 right)
	{
		return right * scalar;
	}

	float Vec4::Dot(const Vec4& other)
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}
			
	Vec4 Vec4::Normalize(const Vec4& vec) 
	{ 
		return vec / vec.Length(); 
	}

	Vec3 Vec4::XYZ()
	{
		return Vec3(this->x, this->y, this->z);
	}

	std::ostream& operator<<(std::ostream& stream, iVec4& vector)
	{
		stream << "Vector3i: (" << vector.x << ", " << vector.y << ", " << vector.z << ")";
		return stream; 
	}
	
	//================================================

	iVec4& iVec4::Add(const iVec4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this; 
	}
	
	//================================================
	
	iVec4& iVec4::Subtract(const iVec4& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this; 
	}
	
	//================================================
	
	iVec4& iVec4::Multiply(const iVec4& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this; 
	}
	
	//================================================
	
	iVec4& iVec4::Divide(const iVec4& other)
	{ 
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this; 
	}
	
	//================================================
	
	iVec4& iVec4::Scale(const s32& scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	
	//================================================

	iVec4 operator+(iVec4 left, const iVec4& right)
	{
		return left.Add(right); 
	}
	
	//================================================
	
	iVec4 operator-(iVec4 left, const iVec4& right)
	{
		return left.Subtract(right); 
	}
	
	//================================================
	
	iVec4 operator*(iVec4 left, const iVec4& right)
	{
		return left.Multiply(right); 
	}
	
	//================================================
	
	iVec4 operator/(iVec4 left, const iVec4& right)
	{
		return left.Divide(right);
	}
	
	//================================================
	
	b8 operator==(iVec4 left, const iVec4& right)
	{ 
		return (left.x == right.x && left.y == right.y && left.z == right.z);
	}
	
	//================================================
	
	b8 operator!=(iVec4 left, const iVec4& right)
	{
		return !(left == right);
	}
	
	//================================================

	iVec4& iVec4::operator+=(const iVec4& other)
	{
		return Add(other); 
	}
	
	//================================================
	
	iVec4& iVec4::operator-=(const iVec4& other)
	{ 
		return Subtract(other); 
	}
	
	//================================================
	
	iVec4& iVec4::operator*=(const iVec4& other)
	{
		return Multiply(other);
	}
	
	//================================================
	
	iVec4& iVec4::operator/=(const iVec4& other)
	{
		return Divide(other); 
	}
	
	//================================================
	
	iVec4& iVec4::operator*=(const s32& scalar)
	{
		return Multiply(iVec4(scalar));
	}

	//================================================
}