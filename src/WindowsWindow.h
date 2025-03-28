#include<string>
#include "Window.h"
#include <windows.h>

namespace RGS {
	class WindowsWindow : public Window {
	public:
		WindowsWindow(const std::string title, int width, int height);
		~WindowsWindow();
		virtual void Show()const override;
	public:
		//public����ע�ắ����ʵ�ְ�ȫ�ӿ�
		static void Init();
		static void Terminate();//��ֹ
		static void PollInputEvents();
		virtual void DrawFramebuffer(const Framebuffer& framebuffer) override;


	private:
		static void Register();
		static void Unregister();

		static void KeyPressImpl(WindowsWindow* window, const WPARAM wParam , const char state);
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_Handle;
		HDC m_MemoryDC;//���ڻ��Ƶ��ڴ�DC
		unsigned char* m_Buffer;//���ڻ��Ƶ��ڴ滺���� 

		static bool s_Inited;
	};
}