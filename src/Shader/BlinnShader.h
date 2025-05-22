#pragma once
#include "../RGS/Texture.h"
#include "../RGS/Renderer.h"
#include "../RGS/KDTree.h"
namespace RGS {
	struct BlinnVertex : public VertexBase {
		Vec3 ModelNormal;
		Vec2 TexCoord = { 0.0f,0.0f };
	};

	

	struct BlinnVaryings : public VaryingBase {
		Vec3 WorldPos;
		Vec3 WorldNormal; //法线位置	
		Vec2 TexCoord;//材质贴图坐标
	};

	

	struct BlinnUniforms : public UniformBase {
		Mat4 Model;
		Mat4 ModelNormalToWorld;//模型法线到世界法线的变换矩阵
		Vec3 LightPos{ 0.0f,1.0f,2.0f };
		Vec3 LightAmbient{ 0.3f,0.3f,0.3f };//环境光色
		Vec3 LightDiffuse{ 0.5f,0.5f,0.5f };//反射色
		Vec3 LightSpecular{ 1.0f,1.0f,1.0f };//高光色
		Vec3 ObjectColor{ 1.0f,1.0f,1.0f };//物体颜色
		Vec3 CameraPos;//相机位置

		float Lx=1;//水平方向的mipmap层级
		float Ly=1;//垂直方向的mipmap层级
		bool isMipmap = 1;//是否使用mipmap
		int MaxDepth = 3;
		float ReflectRatio = 0.3f;//反射系数

		Texture* Diffuse = nullptr;
		Texture* Specular = nullptr;

		float Shininess = 32.0f;//高光系数
		bool isAnother = false;
	};

	struct RayTUniforms : public UniformBase {
		Mat4 Model;
		Vec3 LightAmbient{ 0.3f,0.3f,0.3f };//环境光色
		Vec3 LightDiffuse{ 0.5f,0.5f,0.5f };//反射色
		Vec3 LightSpecular{ 1.0f,1.0f,1.0f };//高光色
		Vec3 ObjectColor{ 1.0f,1.0f,1.0f };//物体颜色
		Vec3 CameraPos;//相机位置

		float Lx = 1;//水平方向的mipmap层级
		float Ly = 1;//垂直方向的mipmap层级
		bool isMipmap = 1;//是否使用mipmap
		int MaxDepth = 3;
		float ReflectRatio = 0.3f;//反射系数

		Texture* Diffuse = nullptr;
		Texture* Specular = nullptr;

		KDNode* kdTree = nullptr;

		float Shininess = 32.0f;//高光系数
		bool isAnother = false;
	};

	void BlinnVertexShader(BlinnVaryings& varyings, const BlinnVertex& vertex, const BlinnUniforms& uniforms);

	void RayTracingVertexShader(RayTVertex& varyings,  const RayTUniforms& uniforms);

	Vec4 BlinnFragmentShader(bool& discard, const BlinnVaryings& varyings, const BlinnUniforms& uniforms);

	Vec4 RayTracingFragmentShader(bool& discard, const RayTVertex& varyings, const RayTUniforms& uniforms);
}