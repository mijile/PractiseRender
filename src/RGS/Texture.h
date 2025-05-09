#pragma once
#include "vector.h"
//#include "../Shader/BlinnShader.h"
//#include "../Shader/ShaderBase.h"
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
namespace RGS {

	class Texture {
	public:
		Texture(const std::string& path);
		~Texture();
		Vec4 Sample(Vec2 texCoords)const;
		Vec4 MipmapSample(Vec2 texCoords, float Lx , float Ly)const;
	private:
		void Init();

	private:
		bool IsMipMap = 0;
		Vec4* m_Data;
		std::string m_Path;
		int m_Width, m_Height, m_Channels;
		Vec4* Mipmap[4][4];
	};
}