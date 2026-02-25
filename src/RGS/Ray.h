#pragma once
#include "vector.h"

namespace RGS {
	class Ray {
	public:
		Ray() = default;
		Ray(const Vec3& origin, const Vec3& direction) : m_origin(origin), m_direction(direction) {}
		const Vec3& Origin() const { return m_origin; }
		const Vec3& Direction() const { return m_direction; }
		Vec3 at(float t) const { return m_origin + m_direction * t; }
	private:
		Vec3 m_origin;
		Vec3 m_direction;	


	};
    class PointLight {
    public:
        Vec3 position;  // 点光源的位置
        Vec3 color;     // 点光源的颜色
        float intensity; // 点光源的强度
        float attenuation; // 衰减系数

        // 构造函数
        PointLight(const Vec3& pos, const Vec3& col, float inten, float atten = 1.0f)
            : position(pos), color(col), intensity(inten), attenuation(atten) {
        }
    };


}
