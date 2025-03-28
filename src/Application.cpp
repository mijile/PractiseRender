#include "Application.h"
#include "Window.h"
#include <iostream>
#include"InputCodes.h"
#include "Shader/ShaderBase.h"
#include "Renderer.h"
#include "Shader/BlinnShader.h"
#include "vector.h"
#include <chrono>

namespace RGS {
	Application::Application(const std::string name, int width, int height)
	{
		m_Name = name;
		m_Width = width;
		m_Height = height;
		Init();
	}
	Application::~Application()
	{
	}
	void Application::Init()
	{
		Window::Init();
		m_Window = Window::Create(m_Name, m_Width, m_Height);
	}
	void Application::Terminate()
	{	
		delete m_Window;
		Window::Terminate();
	}
	void Application::Run()
	{
		while (!m_Window->Closed()) {
			auto nowFrameTime = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(nowFrameTime - m_lastFrameTime);
			float deltaTime = duration.count() * 0.001f * 0.001f;
			m_lastFrameTime = nowFrameTime;

			
			Update(deltaTime);
			Window::PollInputEvents();
		}
	}
	void Application::Update(float time)
	{
		OnCameraUpdate(time);

		Framebuffer framebuffer(m_Width, m_Height);
		framebuffer.Clear({ 0.0f,0.0f,0.0f });
		m_Window->DrawFramebuffer(framebuffer);
		Triangle<BlinnVertex> triangle;
		BlinnUniforms uniforms;
		Program program(BlinnVertexShader, BlinnFragmentShader);

		

		Mat4 view = LookAt(m_Camera.Pos, m_Camera.Pos + m_Camera.Dir, {0.0f,1.0f,0.0f});
		Mat4 proj = Perspective(90.0f / 180.0f * PI, m_Camera.Aspect, 0.1f, 100.0f);
		uniforms.MVP = proj * view;

		program.EnableDoubleSided = true;
		
		
		uniforms.isAnother = true;
		program.EnableBlend = false;
		program.EnableDoubleSided = true;
		program.EnableWriteDepth = true;
		triangle[0].ModelPos = { 10.0f,10.0f,-10.0f,1.0f };
		triangle[1].ModelPos = { -1.0f,-1.0f,-1.0f,1.0f };
		triangle[2].ModelPos = { 10.0f,-10.0f,-10.0f,1.0f };
		Renderer::Draw(framebuffer, program, triangle, uniforms);

		uniforms.isAnother = false;
		program.EnableBlend = true;
		program.EnableDoubleSided = true;
		program.EnableWriteDepth = false;
		triangle[0].ModelPos = { -10.0f,10.0f,-10.0f,1.0f };
		triangle[1].ModelPos = { -10.0f,-10.0f,-10.0f,1.0f };
		triangle[2].ModelPos = { 1.0f,-1.0f,-1.0f,1.0f };
		Renderer::Draw(framebuffer, program, triangle, uniforms);

		
		m_Window->DrawFramebuffer(framebuffer);
	}
	void Application::OnCameraUpdate(float time)
	{	
		constexpr float speed = 200.0f;
		if (m_Window->GetKey(RGS_KEY_SPACE) == RGS_PRESS)
			m_Camera.Pos = m_Camera.Pos + m_Camera.Up * speed * time;
		if (m_Window->GetKey(RGS_KEY_V) == RGS_PRESS)
			m_Camera.Pos = m_Camera.Pos - m_Camera.Up * speed * time;
		if (m_Window->GetKey(RGS_KEY_A) == RGS_PRESS) 
			m_Camera.Pos = m_Camera.Pos - m_Camera.Right * speed * time;
		if (m_Window->GetKey(RGS_KEY_D) == RGS_PRESS)
			m_Camera.Pos = m_Camera.Pos + m_Camera.Right * speed * time;
		if (m_Window->GetKey(RGS_KEY_W) == RGS_PRESS)
			m_Camera.Pos = m_Camera.Pos + m_Camera.Dir * speed * time;
		if (m_Window->GetKey(RGS_KEY_S) == RGS_PRESS)
			m_Camera.Pos = m_Camera.Pos - m_Camera.Dir * speed * time;

		constexpr float rotateSpeed = 300.0f;
		Mat4 rotation = Mat4Identity();
		if (m_Window->GetKey(RGS_KEY_Q) == RGS_PRESS)
			rotation = Mat4RotateY(time * rotateSpeed);
		if (m_Window->GetKey(RGS_KEY_E) == RGS_PRESS)
			rotation = Mat4RotateY(-time * rotateSpeed);
		m_Camera.Dir = rotation * m_Camera.Dir;
		m_Camera.Dir = Vec4(NormalizeToVec3(m_Camera.Dir), 0.0f);
		m_Camera.Right = rotation * m_Camera.Right;
		m_Camera.Right = Vec4(NormalizeToVec3(m_Camera.Right), 0.0f);

	}
}
