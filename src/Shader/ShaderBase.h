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
		Vec4 ClipPos = { 0,0,0,1 };//�ü��������
		Vec4 NdcPos = { 0,0,0,1 };//NDS��׼�ռ��е�����
		Vec4 FragPos = { 0,0,0,1 };//�ӿ�ͶӰ�������
	};

	struct UniformBase {
		Mat4 MVP;
		operator const std::string() const {
			return "MVP:" + std::string(MVP);
		}
	};
}
