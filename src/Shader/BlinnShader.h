#pragma once
#include "../RGS/vector.h"
#include "ShaderBase.h"
#include "../RGS/Texture.h"
namespace RGS {
	struct BlinnVertex : public VertexBase {
		Vec3 ModelNormal;
		Vec2 TexCoord = { 0.0f,0.0f };

	};

	struct BlinnVaryings : public VaryingBase {
		Vec3 WorldPos;
		Vec3 WorldNormal; //����λ��	
		Vec2 TexCoord;//������ͼ����
	};

	struct BlinnUniforms : public UniformBase {
		Mat4 Model;
		Mat4 ModelNormalToWorld;//ģ�ͷ��ߵ����編�ߵı任����
		Vec3 LightPos{ 0.0f,1.0f,2.0f };
		Vec3 LightAmbient{ 0.3f,0.3f,0.3f };//������ɫ
		Vec3 LightDiffuse{ 0.5f,0.5f,0.5f };//����ɫ
		Vec3 LightSpecular{ 1.0f,1.0f,1.0f };//�߹�ɫ
		Vec3 ObjectColor{ 1.0f,1.0f,1.0f };//������ɫ
		Vec3 CameraPos;//���λ��

		Texture* Diffuse = nullptr;
		Texture* Specular = nullptr;

		float Shininess = 32.0f;//�߹�ϵ��
		bool isAnother = false;
	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms);
}