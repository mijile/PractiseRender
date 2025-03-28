#pragma once
#include "../vector.h"
#include "ShaderBase.h"
namespace RGS {
	struct BlinnVertex : public VertexBase {

	};

	struct BlinnVaryings : public VaryingBase {
		
	};

	struct BlinnUniforms : public UniformBase {
		bool isAnother = false;
	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms);
}