#pragma once
#include "vector.h"

namespace RGS {

	class Texture {
	public:
		Texture(const std::string& path);
		~Texture();
		Vec4 Sample(Vec2 texCoords)const;
	private:
		void Init();

	private:
		Vec4* m_Data;
		std::string m_Path;
		int m_Width, m_Height, m_Channels;
	};
}