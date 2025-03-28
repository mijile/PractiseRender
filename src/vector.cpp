#include "vector.h"
#include "Base.h"
namespace RGS {
	unsigned char Float2UChar(const float f) {
		if (f < 0.0f) return 0;
		if (f > 1.0f) return 255;
		return static_cast<unsigned char>(f * 255.0f);
	}

	float UChar2Float(const unsigned char c) {
		return static_cast<float>(c) / 255.0f;
	}
	Vec3 Normalize(Vec3 &v) {
		float len = sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
		ASSERT(len != 0);
		return v/len;
	}
	Vec3 NormalizeToVec3(Vec4& v)
	{
		float len = sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z );
		ASSERT(len != 0);
		return {v.X/len,v.Y/len,v.Z/len};
	}
	Mat4::Mat4()
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
			{
				M[i][j] = 0.0f;
			}
		}
	}
	Mat4::Mat4(float m[4][4])
	{
		if (m == nullptr) {
			// 处理错误，例如初始化为单位矩阵或抛出异常
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					M[i][j] = (i == j) ? 1.0f : 0.0f;
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					M[i][j] = m[i][j];
				}
			}
		}
	}

	Mat4::Mat4(Vec4& v0, Vec4& v1, Vec4& v2, Vec4& v3)
	{
		M[0][0] = v0.X; M[0][1] = v0.Y; M[0][2] = v0.Z; M[0][3] = v0.W;
		M[1][0] = v1.X; M[1][1] = v1.Y; M[1][2] = v1.Z; M[1][3] = v1.W;
		M[2][0] = v2.X; M[2][1] = v2.Y; M[2][2] = v2.Z; M[2][3] = v2.W;
		M[3][0] = v3.X; M[3][1] = v3.Y; M[3][2] = v3.Z; M[3][3] = v3.W;
	}
	float Clamp(const float value, const float minN, const float maxN)
	{
		if (value < minN) {
			return minN;
		}
		else if (value > maxN) {
			return maxN;
		}
		else {
			return value;
		}
	}
	Vec3 Cross(const Vec3& v1, const Vec3& v2)
	{
		Vec3 result;
		result.X = v1.Y * v2.Z - v1.Z * v2.Y;
		result.Y = v1.Z * v2.X - v1.X * v2.Z;
		result.Z = v1.X * v2.Y - v1.Y * v2.X;
		return result;
	}
	Vec4 operator*(const Mat4& m, const Vec4& v)
	{
		Vec4 result;
		result.X = m.M[0][0] * v.X + m.M[0][1] * v.Y + m.M[0][2] * v.Z + m.M[0][3] * v.W;
		result.Y = m.M[1][0] * v.X + m.M[1][1] * v.Y + m.M[1][2] * v.Z + m.M[1][3] * v.W;
		result.Z = m.M[2][0] * v.X + m.M[2][1] * v.Y + m.M[2][2] * v.Z + m.M[2][3] * v.W;
		result.W = m.M[3][0] * v.X + m.M[3][1] * v.Y + m.M[3][2] * v.Z + m.M[3][3] * v.W;
		return result;
	}
	Mat4 operator*(const Mat4& left, const Mat4& right)
	{
		Mat4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++)
			{
				result.M[i][j] = left.M[i][0] * right.M[0][j] + left.M[i][1] * right.M[1][j] + left.M[i][2] * right.M[2][j] + left.M[i][3] * right.M[3][j];
			}
		}
		return result;
	}
	Mat4 Mat4Identity()
	{	
		Mat4 result;
		for (int i = 0; i < 4; i++) {
			result.M[i][i] = 1.0f;
		}
		return result;
	}
	float Dot(const Vec3& v1, const Vec3& v2)
	{
		return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
	}
	Mat4 Mat4Translate(float tx, float ty, float tz)
	{
		Mat4 translate = Mat4Identity();
		translate.M[0][3] = tx;
		translate.M[1][3] = ty;
		translate.M[2][3] = tz;
		return translate;
	}
	Mat4 Mat4Scale(float sx, float sy, float sz)
	{
		Mat4 scale = Mat4Identity();
		ASSERT(sx != 0.0f && sy != 0.0f && sz != 0.0f);
		scale.M[0][0] = sx;
		scale.M[1][1] = sy;
		scale.M[2][2] = sz;
		return scale;
	}
	Mat4 Mat4RotateX(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);
		Mat4 rotate = Mat4Identity();
		rotate.M[1][1] = c;
		rotate.M[1][2] = -s;
		rotate.M[2][1] = s;
		rotate.M[2][2] = c;
		return rotate;
	}
	Mat4 Mat4RotateY(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);
		Mat4 rotate = Mat4Identity();
		rotate.M[0][0] = c;
		rotate.M[0][2] = s;
		rotate.M[2][0] = -s;
		rotate.M[2][2] = c;
		return rotate;
	}
	Mat4 Mat4RotateZ(float angle)
	{
		float c = cos(angle);
		float s = sin(angle);
		Mat4 rotate = Mat4Identity();
		rotate.M[0][0] = c;
		rotate.M[0][1] = -s;
		rotate.M[1][0] = s;
		rotate.M[1][1] = c;
		return rotate;
	}
	Mat4 LookAt(const Vec3& eye, const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis)
	{
		Mat4 m=Mat4Identity();
		m.M[0][0] = xAxis.X;
		m.M[0][1] = xAxis.Y;
		m.M[0][2] = xAxis.Z;

		m.M[1][0] = yAxis.X;
		m.M[1][1] = yAxis.Y;
		m.M[1][2] = yAxis.Z;

		m.M[2][0] = zAxis.X;
		m.M[2][1] = zAxis.Y;
		m.M[2][2] = zAxis.Z;

		m.M[0][3] = -Dot(xAxis, eye);
		m.M[1][3] = -Dot(yAxis, eye);
		m.M[2][3] = -Dot(zAxis, eye);

		return m;
	}
	Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up)
	{
		Vec3 zAxis = Normalize(eye - target);
		Vec3 xAxis = Normalize(Cross(up, zAxis));
		Vec3 yAxis = Cross(zAxis, xAxis);
		return LookAt(eye, xAxis, yAxis, zAxis);
	}
	Mat4 Perspective(float fovy, float aspect, float zNear, float zFar)
	{
		Mat4 mat =Mat4Identity();
		float f = 1.0f /float(tan(fovy / 2.0f));
		mat.M[0][0] = f / aspect;
		mat.M[1][1] = f;
		mat.M[2][2] = -(zFar+zNear) / (zFar - zNear);
		mat.M[2][3] = 2.0f * (zFar * zNear) / (zNear - zFar);
		mat.M[3][2] = -1.0f;
		mat.M[3][3] = 0;
		return mat;
	}

	float Lerp(const float start, const float end, const float ratio)
	{
		return start * (1.0f - ratio) + end * ratio;
	}

	Vec3 Lerp(const Vec3& start, const Vec3& end, const float ratio)
	{
		return start* (1.0f - ratio) + end * ratio;
	}

}
