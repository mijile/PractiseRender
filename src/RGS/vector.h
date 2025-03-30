#pragma once
#include <string>
#include"Base.h"
#define PI 3.14159265359
#define EPSLION 1e-5f
namespace RGS {
	struct Vec2 {
	public:
		float X;
		float Y;

		Vec2(float x, float y) : X(x), Y(y) {};
		Vec2() : X(0.0f), Y(0.0f) {};

		Vec2 operator - (Vec2 v) {
			return { X - v.X, Y - v.Y };
		}

		Vec2 operator - (const Vec2& v) const {
			return { X - v.X, Y - v.Y };
		}

		Vec2 operator / (float f) {
			ASSERT(f != 0);
			return { X / f, Y / f };
		}

		Vec2 operator / (float f) const {
			ASSERT(f != 0);
			return { X / f, Y / f };
		}

		Vec2 operator + (Vec2 v) {
			return { X + v.X, Y + v.Y };
		}

		Vec2 operator + (const Vec2& v) const {
			return { X + v.X, Y + v.Y };
		}

		Vec2 operator * (float f) {
			return { X * f, Y * f };
		}

		Vec2 operator * (float f) const {
			return { X * f, Y * f };
		}

		Vec2 operator * (const Vec2& v) {
			return { X * v.X, Y * v.Y };
		}
	};

	struct Vec3 {
	public:
		float X;
		float Y;
		float Z;

		Vec3(float x, float y, float z) :X(x),Y(y),Z(z){};
		Vec3() :X(0.0f), Y(0.0f), Z(0.0f) {};
		operator Vec2() const {
			return { X,Y };
		}
		Vec3 operator - (Vec3 v) {
			return { X - v.X,Y - v.Y,Z - v.Z };
		}

		Vec3 operator - (const Vec3& v) const {
			return { X - v.X, Y - v.Y, Z - v.Z };
		}

		Vec3 operator / (float f) {
			ASSERT(f != 0);
			return { X / f,Y / f,Z / f };
		}
		Vec3 operator / (float f) const{
			ASSERT(f != 0);
			return { X / f,Y / f,Z / f };
		}
		Vec3 operator + (Vec3 v) {
			return { X + v.X,Y + v.Y,Z + v.Z };
		}
		Vec3 operator + (Vec3 v) const{
			return { X + v.X,Y + v.Y,Z + v.Z };
		}
		Vec3 operator * (float f) {
			return { X * f,Y * f,Z * f };
		}
		Vec3 operator * ( float f) const {
			return { X * f,Y * f,Z * f };
		}
		Vec3 operator * (const Vec3& v) const {
			return { X * v.X, Y * v.Y, Z * v.Z };
		}

	};
	
	struct Vec4 {
	public:
		float X;
		float Y;
		float Z;
		float W;
		Vec4(float x, float y, float z, float w) :X(x), Y(y), Z(z), W(w) {};
		Vec4() :X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) {};
		Vec4(Vec3 v, float w) :X(v.X), Y(v.Y), Z(v.Z), W(w) {};
		Vec4(float val) :X(val), Y(val), Z(val), W(val) {};
		operator Vec3() const {
			return { X,Y,Z };
		}
		operator Vec2() const {
			return { X,Y };
		}
		operator std::string()const {
			return std::to_string(X) + " " + std::to_string(Y) + " " + std::to_string(Z) + " " + std::to_string(W);
		}
		Vec4 operator * (float f) {
			return { X * f,Y * f,Z * f,W*f };
		}
		Vec4 operator * (float f) const{
			return { X * f,Y * f,Z * f,W * f };
		}
		Vec4 operator * (const Vec4& v) {
			return { X * v.X,Y * v.Y,Z * v.Z ,W*v.W };
		}
		Vec4 operator + (Vec4 v) {
			return { X + v.X,Y + v.Y,Z + v.Z , W+v.W };
		}
		Vec4 operator + (Vec4 v) const {
			return { X + v.X,Y + v.Y,Z + v.Z , W + v.W };
		}
		Vec4 operator - (Vec4 v) {
			return { X - v.X,Y - v.Y,Z - v.Z , W - v.W };
		}
		Vec4 operator - (Vec4 v) const {
			return { X - v.X,Y - v.Y,Z - v.Z , W - v.W };
		}
		Vec4 operator / (float f) {
			ASSERT(f != 0);
			return { X / f,Y / f,Z / f , W/f };
		}
		Vec4 operator / (float f) const {
			ASSERT(f != 0);
			return { X / f,Y / f,Z / f ,W/f};
		}
	};

	struct Mat4 {
		float M[4][4];
		Mat4();
		Mat4(float m[4][4]);
		Mat4(Vec4& v0, Vec4& v1, Vec4& v2, Vec4& v3);

		operator const std::string() const {
			std::string s;
			for (int i = 0; i < 4; i++) {
				s += std::to_string(M[i][0]) + " " + std::to_string(M[i][1]) + " " + std::to_string(M[i][2]) + " " + std::to_string(M[i][3]) + "\n";
			}
			return s;
		}

		Mat4 operator + (const Mat4& other) const {
			Mat4 result;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					result.M[i][j] = M[i][j] + other.M[i][j];
			return result;
		}

		Mat4 operator - (const Mat4& other) const {
			Mat4 result;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					result.M[i][j] = M[i][j] - other.M[i][j];
			return result;
		}

		Mat4 operator * (const Mat4& other) const {
			Mat4 result;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result.M[i][j] = 0.0f;
					for (int k = 0; k < 4; ++k) {
						result.M[i][j] += M[i][k] * other.M[k][j];
					}
				}
			}
			return result;
		}
		Mat4 operator * (const Mat4& other){
			Mat4 result;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result.M[i][j] = 0.0f;
					for (int k = 0; k < 4; ++k) {
						result.M[i][j] += M[i][k] * other.M[k][j];
					}
				}
			}
			return result;
		}

		Mat4 operator * (float scalar) const {
			Mat4 result;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					result.M[i][j] = M[i][j] * scalar;
			return result;
		}

		Mat4 operator / (float scalar) const {
			ASSERT(scalar != 0);
			Mat4 result;
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					result.M[i][j] = M[i][j] / scalar;
			return result;
		}

		Mat4& operator += (const Mat4& other) {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					M[i][j] += other.M[i][j];
			return *this;
		}

		Mat4& operator -= (const Mat4& other) {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					M[i][j] -= other.M[i][j];
			return *this;
		}

		Mat4& operator /= (float scalar) {
			ASSERT(scalar != 0);
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					M[i][j] /= scalar;
			return *this;
		}
	};

	//计算定义
	float Clamp(const float value, const float minN, const float maxN);
	Vec3 Cross(const Vec3& v1, const Vec3& v2);
	Vec4 operator* (const Mat4& m, const Vec4& v);
	Mat4 operator* (const Mat4& left, const Mat4& right);
	Mat4 Mat4Identity();
	float Dot(const Vec3& v1, const Vec3& v2);
	Vec3 Normalize(Vec3 &v);
	Vec3 Normalize(const Vec3& v);
	Vec3 NormalizeToVec3(Vec4& v);
	//空间变换
	Mat4 Mat4Translate(float tx, float ty ,float tz);
	Mat4 Mat4Scale(float sx, float sy, float sz);
	Mat4 Mat4RotateX(float angle);
	Mat4 Mat4RotateY(float angle);
	Mat4 Mat4RotateZ(float angle);
	Mat4 LookAt(const Vec3& eye, const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis);
	Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
	Mat4 Perspective(float fovy, float aspect, float zNear, float zFar);

	float Lerp(const float start, const float end, const float ratio);
	Vec3 Lerp(const Vec3& start, const Vec3& end, const float ratio);

	extern unsigned char Float2UChar(const float f);
	extern float UChar2Float(const unsigned char c);
}