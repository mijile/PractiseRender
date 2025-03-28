#include "Window.h"
#include "WindowsWindow.h"
namespace RGS {
	Window::Window(const std::string title, int width, int height)
	{
		m_Title = title;
		m_Width = width;
		m_Height = height;
	}

	Window::Window()
	{
		m_Title = "Default";
		m_Width = 800;
		m_Height = 600;
	}

	void Window::PollInputEvents()
	{
		WindowsWindow::PollInputEvents();
	}

	void Window::Init()
	{
		WindowsWindow::Init();
	}

	void Window::Terminate()
	{
		WindowsWindow::Terminate();
	}

	Window* Window::Create(const std::string title, int width, int height)
	{
//可根据不同的平台选择不同的窗口
//#ifdef RGS_PLATFORM_WINDOWS
		return new WindowsWindow(title, width, height);
//#endif	RGS_PLATFORM_MAC
//		return new MacWindow(title, width, height);
	}


}

