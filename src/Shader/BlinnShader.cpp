#include "BlinnShader.h"

namespace RGS {
	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms)
	{
		varyings.ClipPos = uniforms.MVP * vertex.ModelPos;
		varyings.TexCoord = vertex.TexCoord;
		varyings.WorldPos = uniforms.Model * vertex.ModelPos;
		varyings.WorldNormal = uniforms.ModelNormalToWorld * Vec4{ vertex.ModelNormal,0.0f };
	}

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms)
	{	
		//if (uniforms.isAnother) {
		//	Vec4 color = varyings.NdcPos * 0.5f + 0.5f;
		//	color.W = 1.0f;
		//	return color;
		//}
		////»æÖÆÌõÎÆ
		//else {
		//	int x = varyings.FragPos.X;
		//	if((x+10) % 10 >4){
		//		discard=true;
		//	}
		//}
		
		//return Vec4{varyings.TexCoord.X ,varyings.TexCoord.Y ,0.0f , 1.0f};
		//²âÊÔ

		discard = false;

		const Vec3& cameraPos = uniforms.CameraPos;
		const Vec3& lightPos = uniforms.LightPos;
		const Vec3& worldPos = varyings.WorldPos;
		Vec3 worldNormal = Normalize(varyings.WorldNormal);
		Vec3 lightDir = Normalize(lightPos - worldPos);
		Vec3 viewDir = Normalize(cameraPos - worldPos);
		Vec3 halfDir = Normalize(lightDir + viewDir);

		Vec3 ambient = uniforms.LightAmbient;
		Vec3 specularStrength{ 1.0f,1.0f,1.0f };
		Vec3 diffColor{ 1.0f,1.0f,1.0f };
		if (uniforms.Diffuse && uniforms.Specular) {
			const Vec2& texCoord = varyings.TexCoord;
			diffColor = uniforms.Diffuse->Sample(varyings.TexCoord);
			ambient = ambient * diffColor;
			specularStrength = uniforms.Specular->Sample(varyings.TexCoord);
		}

		Vec3 diffuse = diffColor*uniforms.LightDiffuse * std::max(0.0f, Dot(worldNormal, lightDir)) ;
		Vec3 specular = uniforms.LightSpecular * specularStrength *(float)pow(std::max(0.0f, Dot(worldNormal, halfDir)), uniforms.Shininess) ;

		Vec3 finalColor = (ambient + diffuse + specular);
		return { finalColor,1.0f };
	}

}

