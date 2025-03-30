 #pragma once
#include<cmath>
#include "vector.h"
#include<type_traits>
#include "Framebuffer.h"
#include "Base.h"


namespace RGS {

	template<typename vertex_t>
	struct Triangle {
		static_assert(std::is_base_of<VertexBase, vertex_t>::value, "vertex_t must be derived from VertexBase");
		
		vertex_t Vertex[3];

		vertex_t& operator[](int i) {
			return Vertex[i];
		}
		const vertex_t& operator[](int i) const {
			return Vertex[i];
		}
		
		Triangle() = default;
	
	
	};

	enum class DepthFuncType {
		LESS,
		LEQUAL,
		ALWAYS
	};

	template<typename vertex_t , typename uniforms_t ,typename varyings_t>
	struct Program {
		bool EnableDoubleSided = false;
		bool EnableDepthTest = true;
		bool EnableBlend = true;
		bool EnableWriteDepth = true;
		DepthFuncType DepthFunc = DepthFuncType::LESS;
		//顶点着色器
		using vertex_shader_t = void (*)(varyings_t&, const vertex_t&, const uniforms_t&);
		vertex_shader_t VertexShader;

		//片段着色器
		using fragment_shader_t = Vec4 (*)(bool &discard, const varyings_t&, const uniforms_t&);
		fragment_shader_t FragmentShader;

		Program(const vertex_shader_t vertexShader , const fragment_shader_t fragmentShader) 
			: VertexShader(vertexShader) , FragmentShader(fragmentShader){}
	};

	class Renderer{
	private:
		static constexpr int RGS_MAX_VARYINGS = 9;

	private:
		enum class Plane	 {
			POSITIVE_W,
			POSITIVE_X,
			NEGATIVE_X,
			POSITIVE_Y,
			NEGATIVE_Y,
			POSITIVE_Z,
			NEGATIVE_Z
		};

		struct BoundingBox {
			int MinX;
			int MinY;
			int MaxX;
			int MaxY;
		};

		
		static bool IsVertexVisible(const Vec4& clipPos);
		static bool IsInsidePlane(const Vec4& clipPos, const Plane plane);
		static BoundingBox GetBoundingBox(const Vec4(&fragCoords)[3], const int width, const int height);
		static float GetIntersectRatio(const Vec4& prev, const Vec4& curr, const Plane plane);
		static bool IsInsideTriangle(float(&weights)[3]);
		static bool IsBackFacing(const Vec4& v1, const Vec4& v2, const Vec4& v3);
		static bool PassDepthTest(const float& depth, const float& zdepth, const DepthFuncType& depthFunc);

		static void CalculateWeights(float(&screenWeights)[3], 
									 float(&weights)[3],
									 const Vec4 (&fragCoords)[3], 
									 const Vec2 screenPoint);

		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void ProcessPixel(Framebuffer& framebuffer,
			const int x,
			const int y,
			const Program<vertex_t, uniforms_t, varyings_t>& program,
			const varyings_t& varyings,
			const uniforms_t& uniforms) 
		{
			bool discard = false;
			Vec4 color{ 0.0f,0.0f,0.0f,0.0f };
			color = program.FragmentShader(discard, varyings, uniforms);
			if (discard) {
				return;
			}
			color.X = Clamp(color.X, 0.0f, 1.0f);
			color.Y = Clamp(color.Y, 0.0f, 1.0f);
			color.Z = Clamp(color.Z, 0.0f, 1.0f);
			color.W = Clamp(color.W, 0.0f, 1.0f);

			//Blend
			if (program.EnableBlend) {
				Vec3 dstColor = framebuffer.GetColor(x, y);
				Vec3 srcColor = color;
				float alpha = color.W;
				color = { Lerp(dstColor , srcColor , alpha) , 1.0f };
			}
			framebuffer.SetColor(x, y, color);
		}

		template<typename varyings_t>
		static void LerpVaryings(varyings_t& out, const varyings_t(&varyings)[3], float(&weights)[3], const int width, const int height) {
			out.ClipPos = varyings[0].ClipPos * weights[0] + varyings[1].ClipPos * weights[1] + varyings[2].ClipPos * weights[2];
			out.NdcPos = out.ClipPos / out.ClipPos.W;
			out.NdcPos.W = 1.0f / out.ClipPos.W;
			
			out.FragPos.X = (out.NdcPos.X + 1.0f) * 0.5f * width;
			out.FragPos.Y = (out.NdcPos.Y + 1.0f) * 0.5f * height;
			out.FragPos.Z = (out.NdcPos.Z + 1.0f) * 0.5f;
			out.FragPos.W = out.NdcPos.W;
			
			constexpr uint32_t floatOffset = sizeof(Vec4) * 3 / sizeof(float);//跳过前两NdcPos和FragPos
			constexpr uint32_t floatNum = sizeof(varyings_t) / sizeof(float);

			float* v0 = (float*)&varyings[0];
			float* v1 = (float*)&varyings[1];
			float* v2 = (float*)&varyings[2];
			float* outFloat = (float*)&out;

			//按照比例计算
			for (int i = floatOffset; i < (int)floatNum; i++) {
				outFloat[i] = v0[i] * weights[0] + v1[i] * weights[1] + v2[i] * weights[2];
			}

		}

		template<typename varyings_t>
		static void LerpVaryings(varyings_t& out, const varyings_t& start, const varyings_t& end, const float ratio) {
			constexpr uint32_t floatNum = sizeof(varyings_t) / sizeof(float);
			float* startFloat = (float*)&start;	
			float* endFloat = (float*)&end;
			float* outFloat = (float*)&out;

			//按照比例插值
			for (int i = 0; i < (int)floatNum; i++) {
				outFloat[i] = Lerp(startFloat[i], endFloat[i], ratio);
			}

		}

		template<typename varyings_t>
		static int ClipAgainstPlane(varyings_t(&outVaryings)[RGS_MAX_VARYINGS],
			const varyings_t(&inVaryings)[RGS_MAX_VARYINGS],
			const Plane plane,
			const int inVertexNum) {
			ASSERT(inVertexNum >= 3);

			int outVertexNum = 0;
			for (int i = 0; i < inVertexNum; ++i) {
				int prevIndex = (i + inVertexNum - 1) % inVertexNum;
				int currIndex = i;

				const varyings_t& prevVaryings = inVaryings[prevIndex];
				const varyings_t& currVaryings = inVaryings[currIndex];

				const bool prevInside = IsInsidePlane(prevVaryings.ClipPos, plane);
				const bool currInside = IsInsidePlane(currVaryings.ClipPos, plane);

				if (currInside != prevInside) 
				{
					float ratio = GetIntersectRatio(prevVaryings.ClipPos, currVaryings.ClipPos,  plane);
					LerpVaryings(outVaryings[outVertexNum], prevVaryings, currVaryings, ratio);
					outVertexNum++;
				}

				if (currInside) {
					outVaryings[outVertexNum] = inVaryings[currIndex];
					outVertexNum++;
				}
			}
			_ASSERT(outVertexNum <= RGS_MAX_VARYINGS);
			return outVertexNum;
		}

		template<typename varyings_t>
		static void CalculateNdcPos(varyings_t(&varyings)[RGS_MAX_VARYINGS], const int vertexNum) {
			for (int i = 0; i < vertexNum; ++i) {
				varyings[i].NdcPos.X = varyings[i].ClipPos.X / varyings[i].ClipPos.W;
				varyings[i].NdcPos.Y = varyings[i].ClipPos.Y / varyings[i].ClipPos.W;
				varyings[i].NdcPos.Z = varyings[i].ClipPos.Z / varyings[i].ClipPos.W;

				varyings[i].NdcPos.W = 1.0f / varyings[i].ClipPos.W;
			}
		
		}

		template<typename varyings_t>
		static void CalculateFragPos(varyings_t(&varyings)[RGS_MAX_VARYINGS], const int vertexNum , const float width , const float height) {
			for (int i = 0; i < vertexNum; ++i) {
				varyings[i].FragPos.X = (varyings[i].NdcPos.X + 1.0f) * 0.5f * width;
				varyings[i].FragPos.Y = (varyings[i].NdcPos.Y + 1.0f) * 0.5f * height;
				varyings[i].FragPos.Z = (varyings[i].NdcPos.Y + 1.0f) * 0.5f;//厚度默认为1
				varyings[i].FragPos.W = varyings[i].NdcPos.W;

			}

		}


		template<typename varyings_t>
		static int Clip(varyings_t(&varyings)[RGS_MAX_VARYINGS]) {
			//先判断是否完全在视锥体内
			bool v0_visible = IsVertexVisible(varyings[0].ClipPos);
			bool v1_visible = IsVertexVisible(varyings[1].ClipPos);
			bool v2_visible = IsVertexVisible(varyings[2].ClipPos);
			if (v0_visible && v1_visible && v2_visible) {
				return 3;
			}

			int vertexNum = 3;
			varyings_t varyings_[RGS_MAX_VARYINGS];
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::POSITIVE_W, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::POSITIVE_X, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::NEGATIVE_X, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::POSITIVE_Y, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::NEGATIVE_Y, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings, varyings_, Plane::POSITIVE_Z, vertexNum);
			if (vertexNum == 0) return 0;
			vertexNum = ClipAgainstPlane(varyings_, varyings, Plane::NEGATIVE_Z, vertexNum);
			if (vertexNum == 0) return 0;
			memcpy(varyings, varyings_, sizeof(varyings_));

			return vertexNum;
		}
		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void RasterizeTriangle(Framebuffer& framebuffer,
			const Program<vertex_t, uniforms_t, varyings_t>& program,
			const varyings_t(&varyings)[3],
			const uniforms_t& uniforms) 
		{
			//背面剔除
			if (!program.EnableDoubleSided) {
				bool isBackFacing = false;
				isBackFacing = IsBackFacing(varyings[0].NdcPos, varyings[1].NdcPos, varyings[2].NdcPos );
				if (isBackFacing) {
					return;
				}
			}
			
			Vec4 fragCoords[3];
			fragCoords[0] = varyings[0].FragPos;
			fragCoords[1] = varyings[1].FragPos;
			fragCoords[2] = varyings[2].FragPos;
			BoundingBox bBox = GetBoundingBox(fragCoords, framebuffer.GetWidth(), framebuffer.GetHeight());
			for (int y = bBox.MinY; y <= bBox.MaxY; y++) {
				for (int x = bBox.MinX; x <= bBox.MaxX; x++) {
					float screenWeights[3];
					float weights[3];
					Vec2 screenPoint ={ (float)x + 0.5f, (float)y + 0.5f };
				
					CalculateWeights(screenWeights, weights, fragCoords, screenPoint);

					if (!IsInsideTriangle(weights))continue;
					

					varyings_t pixVaryings;
					LerpVaryings(pixVaryings, varyings, weights, framebuffer.GetWidth(), framebuffer.GetWidth());

					//zbuffer
					if (program.EnableDepthTest) {
						float depth = pixVaryings.FragPos.Z;
						float zdepth = framebuffer.GetDepth(x, y);
						DepthFuncType depthFunc = program.DepthFunc;
						if (!PassDepthTest(depth,zdepth , depthFunc)) {
							continue;
						}
						framebuffer.SetDepth(x, y, depth);
					}
					ProcessPixel(framebuffer, x, y, program, pixVaryings, uniforms);
				
				}
			}
		}
		
	public:
		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void Draw(Framebuffer& framebuffer,
			const Program<vertex_t, uniforms_t, varyings_t>& program, 
			const Triangle<vertex_t>& triangle, 
			const uniforms_t& uniforms) 
		{	
			//检查是否继承自基础渲染类
			static_assert(std::is_base_of<VaryingBase, varyings_t>::value, "varyings_t must be derived from VaryingBase");
			static_assert(std::is_base_of<VertexBase, vertex_t>::value, "uniforms_t must be derived from VertexBase");

			varyings_t varyings[RGS_MAX_VARYINGS];
			for (int i = 0; i < 3; i++) {
				program.VertexShader(varyings[i], triangle[i], uniforms);
			}
			//裁剪
			int vertexNum = Clip(varyings);
			//透视除法标准化＋视口变换
			CalculateNdcPos(varyings, vertexNum);
			int fWidth = framebuffer.GetWidth();
			int fHeight = framebuffer.GetHeight();
			CalculateFragPos(varyings , vertexNum , fWidth, fHeight);

			//按三角形拆分图片渲染下
			for (int i = 0; i < vertexNum - 2; ++i) {
				varyings_t Triangle[3];
				Triangle[0] = varyings[0];
				Triangle[1] = varyings[i+1];
				Triangle[2] = varyings[i+2];
				
				RasterizeTriangle(framebuffer, program, Triangle, uniforms);
			}
		}
	};
}