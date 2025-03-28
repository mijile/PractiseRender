#include "WindowsWindow.h"
#include <assert.h>
#include "Base.h"
#define RGS_WINDOW_CLASS_NAME "Class"
#define RGS_WINDOW_ENTRY_NAME "Entry"

namespace RGS {
	bool WindowsWindow::s_Inited = false;

	WindowsWindow::WindowsWindow(const std::string title, int width, int height)
		:Window(title, width, height)
	{
		ASSERT(s_Inited ,"未初始化");

		DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = (long)width;
		rect.bottom = (long)height;
		AdjustWindowRect(&rect, style, FALSE);
		m_Handle = CreateWindow(RGS_WINDOW_CLASS_NAME, title.c_str(), style, 
								CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top,
								NULL, NULL, GetModuleHandle(NULL), NULL);
		ASSERT(m_Handle != nullptr);
		m_Closed = false;
	    
		SetProp(m_Handle, RGS_WINDOW_ENTRY_NAME, this);

		HDC windowDC = GetDC(m_Handle);
		m_MemoryDC = CreateCompatibleDC(windowDC);

		//设置并使用内存
		//初始化位图
		BITMAPINFOHEADER biHeader = {};
		HBITMAP newBitmap;
		HBITMAP oldBitmap;

		biHeader.biSize = sizeof(BITMAPINFOHEADER);
		biHeader.biWidth = (long(width));
		biHeader.biHeight = -(long(height));	//为了让坐标系与窗口一致，高度取负
		biHeader.biPlanes = 1;
		biHeader.biBitCount = 24;//每个色占8bit
		biHeader.biCompression = BI_RGB;

		//分配空间
		newBitmap = CreateDIBSection(m_MemoryDC, (BITMAPINFO*)&biHeader, DIB_RGB_COLORS, (void**)&m_Buffer, NULL, 0);
		ASSERT(newBitmap != nullptr);
		constexpr int channelCount = 3;
		int size = m_Width * m_Height * sizeof(unsigned char);
		memset(m_Buffer, 0, size);//默认黑色
		oldBitmap = (HBITMAP)SelectObject(m_MemoryDC, newBitmap);

		DeleteObject(oldBitmap);
		ReleaseDC(m_Handle, windowDC);

		Show();
	}

	WindowsWindow::~WindowsWindow()
	{
		ShowWindow(m_Handle, SW_HIDE);//隐藏窗口
		RemoveProp(m_Handle, RGS_WINDOW_ENTRY_NAME);//取消绑定
		DeleteDC(m_MemoryDC);
		DestroyWindow(m_Handle);
	}
	void WindowsWindow::Show() const
	{
		HDC windowDC = GetDC(m_Handle);//获取屏幕
		BitBlt(windowDC, 0, 0, m_Width, m_Height, m_MemoryDC, 0, 0, SRCCOPY); //将内存中的图像拷贝到窗口
		ShowWindow(m_Handle, SW_SHOW);
		ReleaseDC(m_Handle, windowDC);
	}
	void WindowsWindow::Init()
	{
		ASSERT(!s_Inited);
		Register();
		s_Inited = true;
	}
	void WindowsWindow::Terminate()
	{	
		ASSERT(s_Inited);
		Unregister();
		s_Inited = false;
	}
	void WindowsWindow::Register()
	{
		ATOM atom;
		WNDCLASS wc = {0};
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = (HBRUSH)(WHITE_BRUSH); //设置背景颜色
		wc.hCursor = NULL;		//使用默认光标
		wc.hIcon = NULL;		//使用默认图标
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpfnWndProc = WindowsWindow::WndProc;	//窗口处理函数
		wc.lpszClassName = RGS_WINDOW_CLASS_NAME;	//窗口类名字
		wc.style = CS_HREDRAW | CS_VREDRAW;			//设置拉伸时重绘窗口
		wc.lpszMenuName = NULL;		//设置为无菜单	
		atom = RegisterClass(&wc);	//注册窗口
	}
	void WindowsWindow::Unregister()
	{
		UnregisterClass(RGS_WINDOW_CLASS_NAME, GetModuleHandle(NULL));
	}
	void WindowsWindow::PollInputEvents()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	void WindowsWindow::DrawFramebuffer(const Framebuffer& framebuffer)
	{
		//调整视口
		const int fWidth = framebuffer.GetWidth();
		const int fHeight = framebuffer.GetHeight();
		const int width = min(m_Width, fWidth);
		const int height = min(m_Height, fHeight);
		for (int y = 0; y < height; y++) 
		{
			for (int x = 0; x < width; x++)
			{
				constexpr int channelCount = 3;
				constexpr int rChannel = 2;
				constexpr int gChannel = 1;
				constexpr int bChannel = 0;



				Vec3 color = framebuffer.GetColor(x, fHeight - 1 - y);
				const int pixStart = (y * m_Width + x) * channelCount;
				const int rIndex = pixStart + rChannel;
				const int gIndex = pixStart + gChannel;
				const int bIndex = pixStart + bChannel;

				m_Buffer[rIndex] = Float2UChar(color.X);
				m_Buffer[gIndex] = Float2UChar(color.Y);
				m_Buffer[bIndex] = Float2UChar(color.Z);
			}
		}
		InvalidateRect(m_Handle, NULL, FALSE);
	}
	void WindowsWindow::KeyPressImpl(WindowsWindow* window, const WPARAM wParam, const char state)
	{
		if (wParam >= '0' && wParam <= '9') {
			window->m_Keys[wParam] = state;
			return;
		}
		
		if (wParam >= 'A' && wParam <= 'Z') {
			window->m_Keys[wParam] = state;
			return;
		}
		switch (wParam)
		{
		case VK_SPACE:
			window->m_Keys[RGS_KEY_SPACE] = state;
			break;
		case VK_SHIFT:
			window->m_Keys[VK_SHIFT] = state;
			break;
		default:
			break;
		}
	}
	LRESULT CALLBACK WindowsWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{	
		//静态方法无法直接锁定对象，需要先获取
		WindowsWindow* window = (WindowsWindow*)GetProp(hwnd, RGS_WINDOW_ENTRY_NAME);
		if (window == nullptr)
		{
			return DefWindowProc(hwnd, msg, wParam, lParam);//官方默认处理函数
		}

		switch (msg)
		{
		case WM_DESTROY:
			window->m_Closed = true;
			return 0;
		case WM_KEYDOWN:
			KeyPressImpl(window, wParam, RGS_PRESS);
			return 0;
		case WM_KEYUP:
			KeyPressImpl(window, wParam, RGS_RELEASE);
			return 0;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			BitBlt(hdc, 0, 0, window->m_Width, window->m_Height, window->m_MemoryDC, 0, 0, SRCCOPY);
			EndPaint(hwnd, &ps);
			return 0;
		}
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

