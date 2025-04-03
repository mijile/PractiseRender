#include "Framebuffer.h"
#include "vector.h"
#include "Base.h"
namespace RGS {
	Framebuffer::Framebuffer(const int width, const int height)
		:m_width(width), m_height(height)
	{	
		ASSERT(width > 0 && height > 0, "Framebuffer size must be positive");
		m_PixelSize = width * height;
		m_DepthBuffer = new float[m_PixelSize]();
		MSAA_DepthBuffer = new float[4*m_PixelSize]();
		m_ColorBuffer = new Vec3[m_PixelSize]();
		MSAA_ColorBuffer = new Vec3[4*m_PixelSize]();
		Clear({0.0f,0.0f,0.0f});
		ClearDepth();
	}

	Framebuffer::~Framebuffer()
	{
		if (m_ColorBuffer) {
			delete[] m_ColorBuffer;
			m_ColorBuffer = nullptr;
		}
		if (m_DepthBuffer) {
			delete[] m_DepthBuffer;
			m_DepthBuffer = nullptr;
		}
		if (MSAA_ColorBuffer) {
			delete[] MSAA_ColorBuffer;
			MSAA_ColorBuffer = nullptr;
		}
		if (MSAA_DepthBuffer) {
			delete[] MSAA_DepthBuffer;
			MSAA_DepthBuffer = nullptr;
		}
	}


	Vec3 Framebuffer::GetColor(const int x, const int y) const
	{
		if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
			return m_ColorBuffer[GetPixelIndex(x, y)];
		}
		else {
			ASSERT(false, "Out of range");
			return Vec3(0.0f, 0.0f, 0.0f);
		}
	}

	void Framebuffer::SetColor(const int x, const int y, Vec3 color)
	{
		if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
			m_ColorBuffer[GetPixelIndex(x, y)] = color;
		}
		else {
			//ASSERT(false, "Out of range");
		}
	}

	void Framebuffer::SetDepth(const int x, const int y, const float depth)
	{
		if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
			m_DepthBuffer[GetPixelIndex(x, y)] = depth;
		}
		else {
			ASSERT(false, "Out of range");
		}
	}

	float Framebuffer::GetDepth(const int x, const int y) const
	{
		if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
			return m_DepthBuffer[GetPixelIndex(x, y)];
		}
		else {
			ASSERT(false, "Out of range");
			return 0.0f;
		}
	}

	Vec3 Framebuffer::MSAAGetColor(const int x, const int y) const
	{
		if (x >= 0 && x < 2 * m_width && y >= 0 && y < 2 * m_height) {
			return MSAA_ColorBuffer[MSAAGetPixelIndex(x, y)];
		}
		else {
			ASSERT(false, "Out of range");
			return Vec3(0.0f, 0.0f, 0.0f);
		}
	}

	void Framebuffer::MSAASetColor(const int x, const int y, Vec3 color)
	{
		if (x >= 0 && x < 2 * m_width && y >= 0 && y < 2 * m_height) {
			MSAA_ColorBuffer[MSAAGetPixelIndex(x, y)] = color;
		}
		else {
			//ASSERT(false, "Out of range");
		}
	}

	void Framebuffer::MSAASetDepth(const int x, const int y, const float depth)
	{
		if (x >= 0 && x < 2 * m_width && y >= 0 && y < 2 * m_height) {
			MSAA_DepthBuffer[MSAAGetPixelIndex(x, y)] = depth;
		}
		else {
			ASSERT(false, "Out of range");
		}
	}

	float Framebuffer::MSAAGetDepth(const int x, const int y) const
	{
		if (x >= 0 && x < 2 * m_width && y >= 0 && y < 2 * m_height) {
			return MSAA_DepthBuffer[MSAAGetPixelIndex(x, y)];
		}
		else {
			ASSERT(false, "Out of range");
			return 0.0f;
		}
	}

	void Framebuffer::Clear(const Vec3& color)
	{
		for (int i = 0; i < m_PixelSize; i++)
			m_ColorBuffer[i] = color;
		for (int i = 0; i < 4 * m_PixelSize; i++)
			MSAA_ColorBuffer[i] = color;
	}

	void Framebuffer::ClearDepth(float depth)
	{
		for (int i = 0; i < m_PixelSize; i++)
			m_DepthBuffer[i] = depth;
		for (int i = 0; i < 4 * m_PixelSize; i++)
			MSAA_DepthBuffer[i] = depth;
	}

}

