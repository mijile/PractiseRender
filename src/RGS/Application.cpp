#include "Application.h"
#include "Window.h"
#include <iostream>
#include"InputCodes.h"
#include "../Shader/ShaderBase.h"
#include "Renderer.h"
#include "../Shader/BlinnShader.h"
#include "vector.h"
#include <chrono>
#include <fstream>
#include <vector>
#include <filesystem>

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
		m_lastFrameTime = std::chrono::steady_clock::now();

		LoadMesh("D:\\PractiseRender\\Assets\\box.obj");

		m_Uniforms.Diffuse = new Texture("D:\\PractiseRender\\Assets\\container2.png");
		m_Uniforms.Specular = new Texture("D:\\PractiseRender\\Assets\\container2_specular.png");

	}
	void Application::Terminate()
	{	
		delete m_Uniforms.Diffuse;
		delete m_Uniforms.Specular;


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
		BlinnUniforms uniforms;
		Program program(BlinnVertexShader, BlinnFragmentShader);


		Mat4 model = Mat4Identity();
		Mat4 view = LookAt(m_Camera.Pos, m_Camera.Pos + m_Camera.Dir, { 0.0f,1.0f,0.0f });
		Mat4 proj = Perspective(90.0f / 180.0f * PI, m_Camera.Aspect, 0.1f, 100.0f);
		m_Uniforms.MVP = proj * view * model;
		m_Uniforms.CameraPos = m_Camera.Pos;
		m_Uniforms.Model = model;
		m_Uniforms.ModelNormalToWorld = Mat4Identity();
		m_Uniforms.isAnother = true;

		program.EnableBlend = false;
		program.EnableDoubleSided = true;
		program.EnableWriteDepth = true;

		


		for (auto tri : m_Mesh){
			Renderer::Draw(framebuffer, program, tri, m_Uniforms);
		}

		

		/*uniforms.isAnother = false;
		program.EnableBlend = true;
		program.EnableDoubleSided = true;
		program.EnableWriteDepth = false;
		triangle[0].ModelPos = { -10.0f,10.0f,-10.0f,1.0f };
		triangle[1].ModelPos = { -10.0f,-10.0f,-10.0f,1.0f };
		triangle[2].ModelPos = { 1.0f,-1.0f,-1.0f,1.0f };
		Renderer::Draw(framebuffer, program, triangle, uniforms);*/

		
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
	void Application::LoadMesh(const char* fileName)
	{
		std::ifstream file(fileName);
		std::cout << fileName << std::filesystem::current_path()<< std::endl;
		ASSERT(file);

		std::vector<Vec3> positions;
		std::vector<Vec2> texCoords;
		std::vector<Vec3> normals;
		std::vector<int> posIndices;
		std::vector<int> texIndices;
		std::vector<int> normalIndices;

		std::string line;
		while (!file.eof()) {
			std::getline(file, line);
			int items = -1;
			if (line.find("v ") == 0) {
				Vec3 pos;
				items = sscanf_s(line.c_str(), "v %f %f %f", &pos.X, &pos.Y, &pos.Z);
				ASSERT(items == 3);
				positions.push_back(pos);
			}
			else if (line.find("vt ") == 0) {
				Vec2 texcoord;
				items = sscanf_s(line.c_str(), "vt %f %f", &texcoord.X, &texcoord.Y);
				ASSERT(items == 2);
				texCoords.push_back(texcoord);
			}
			else if (line.find("vn ") == 0) {
				Vec3 normal;
				items = sscanf_s(line.c_str(), "vn %f %f %f", &normal.X, &normal.Y, &normal.Z);
				ASSERT(items == 3);
				normals.push_back(normal);
			}
			else if (line.find("f ") == 0) {
				//存三角形对于的顶点索引
				int posIndex[3], uvIndices[3], normalIndex[3];
				items = sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&posIndex[0], &uvIndices[0], &normalIndex[0],
					&posIndex[1], &uvIndices[1], &normalIndex[1],
					&posIndex[2], &uvIndices[2], &normalIndex[2]);
				ASSERT(items == 9);
				for (int i = 0; i < 3; ++i) {
					posIndices.push_back(posIndex[i] - 1);
					texIndices.push_back(uvIndices[i] - 1);
					normalIndices.push_back(normalIndex[i] - 1);
				}
			}
		}
		file.close();

		int triNum = posIndices.size() / 3;
		for (int i = 0; i < triNum; ++i) {
			Triangle<BlinnVertex> tri;
			for (int j = 0; j < 3; ++j) {
				int index = 3 * i + j;
				int posIndex = posIndices[index];
				int texIndex = texIndices[index];
				int nIndex = normalIndices[index];
				tri[j].ModelPos = { positions[posIndex],1.0f };
				tri[j].TexCoord = texCoords[texIndex];
				tri[j].ModelNormal = normals[nIndex];
			}
			m_Mesh.emplace_back(tri);
		}
	}

}
