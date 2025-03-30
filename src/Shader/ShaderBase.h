#pragma once
#include <string>
#include "../RGS/vector.h"
namespace RGS {
	struct VertexBase {
		Vec4 ModelPos = { 0,0,0,1 };
		operator const std::string() const {
			return "ModelPos:" + std::string(ModelPos);
		}
	};

	struct VaryingBase {
		Vec4 ClipPos = { 0,0,0,1 };//裁剪后的坐标
		Vec4 NdcPos = { 0,0,0,1 };//NDS标准空间中的坐标
		Vec4 FragPos = { 0,0,0,1 };//视口投影后的坐标
	};

	struct UniformBase {
		Mat4 MVP;
		operator const std::string() const {
			return "MVP:" + std::string(MVP);
		}
	};
}
