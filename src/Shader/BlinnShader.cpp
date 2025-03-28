#include "BlinnShader.h"

namespace RGS {
	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms)
	{
		varyings.ClipPos = uniforms.MVP * vertex.ModelPos;
	}

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms)
	{	
		if (uniforms.isAnother) {
			Vec4 color = varyings.NdcPos * 0.5f + 0.5f;
			color.W = 1.0f;
			return color;
		}
		//»æÖÆÌõÎÆ
		else {
			int x = varyings.FragPos.X;
			if((x+10) % 10 >4){
				discard=true;
			}
		}
		
		return Vec4{ 1.0f ,1.0f ,1.0f , 0.5f};
		//²âÊÔ
	}

}

