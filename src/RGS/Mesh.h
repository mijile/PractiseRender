#pragma once
#include<vector>
#include "vector.h"
#include "../Shader/ShaderBase.h"
namespace RGS {
	struct RayTVertex : public VertexBase {
		Vec3 ModelNormal;
		Vec2 TexCoord = { 0.0f,0.0f };
		Vec3 WorldPos;
		Vec3 WorldNormal; //法线位置
		Vec3 CamPos;
	};
	
	struct Mesh {
		//三角形面数据
		std::vector<Triangle<RayTVertex> > MeshData;
		//包围盒
		Mat4 Model;
		Vec3 minLoc = { 0,0,0 };
		Vec3 maxLoc = { 0,0,0 };
		void Mesh::setBoundingBox()
		{	
			minLoc = { 10000.0f,10000.0f,10000.0f };
			maxLoc = { -10000.0f,-10000.0f,-10000.0f };
			for (auto& tri : MeshData) {
				for (int i = 0; i < 3; i++) {
					Vec3 temp = (tri[i].WorldPos);
					minLoc.X = std::min(minLoc.X, temp.X);
					minLoc.Y = std::min(minLoc.Y, temp.Y);
					minLoc.Z = std::min(minLoc.Z, temp.Z);
					maxLoc.X = std::max(maxLoc.X, temp.X);
					maxLoc.Y = std::max(maxLoc.Y, temp.Y);
					maxLoc.Z = std::max(maxLoc.Z, temp.Z);
				}
			}
		};
	};
}