#pragma once
#include <string>
#include"InputCodes.h"
#include "Framebuffer.h"
namespace RGS {
	class Window {
	public:
		Window(const std::string title, int width, int height);
		Window();
		virtual ~Window() {};

		virtual void Show()const = 0;
		virtual void DrawFramebuffer(const Framebuffer& framebuffer)= 0;
		

		bool Closed()const { return m_Closed; }
		char GetKey(const uint32_t index) const { return m_Keys[index]; }
	public:
		static void Init();
		static void Terminate();
		static Window* Create(const std::string title, int width, int height);

		static void PollInputEvents();
	protected:
		std::string m_Title;
		int m_Width, m_Height;
		bool m_Closed = true;

		char m_Keys[RGS_KEY_MAX_COUNT];


	};
}