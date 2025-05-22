#pragma once
#include "vector.h"


namespace RGS {

    enum class LightType { Point, Directional, Area };


    struct Light {
        LightType type;
        Vec3 position;    // 点光源/面光源中心/方向光位置
        Vec3 direction;   // 方向光/聚光灯方向
        Vec3 u, v;        // 面光源的两条边
        Vec3 color;       // 光源颜色
        float intensity;  // 光强
        float area;       // 面光源面积
        // 可扩展更多属性
    };
}
