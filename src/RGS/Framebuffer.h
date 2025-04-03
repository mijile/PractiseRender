#pragma once
#include "vector.h"

namespace RGS {
	class Framebuffer {
	public:
		Framebuffer(const int width, const int height);
		~Framebuffer();

		int GetWidth()const { return m_width; };
		int GetHeight()const { return m_height; };

		Vec3 GetColor(const int x, const int y)const ;
		void SetColor(const int x, const int y ,Vec3 color);
		void SetDepth(const int x, const int y, const float depth);
		float GetDepth(const int x, const int y) const;

		Vec3 MSAAGetColor(const int x, const int y)const;
		void MSAASetColor(const int x, const int y, Vec3 color);
		void MSAASetDepth(const int x, const int y, const float depth);
		float MSAAGetDepth(const int x, const int y) const;

		void Clear(const Vec3& color = { 0.0f, 0.0f, 0.0f });
		void ClearDepth(float depth = 1.0f);


	private:
		int GetPixelIndex(const int x, const int y) const { return y * m_width + x; }
		int MSAAGetPixelIndex(const int x, const int y) const { return y * 2 * m_width + x; }


	private:
		int m_width;
		int m_height;
		int m_PixelSize;
		float* m_DepthBuffer;
		float* MSAA_DepthBuffer;
		Vec3* MSAA_ColorBuffer;
		Vec3* m_ColorBuffer;
	};
}