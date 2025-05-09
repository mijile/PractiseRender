 #pragma once
#include<cmath>
#include "vector.h"
#include<type_traits>
#include "Framebuffer.h"
#include "Base.h"
#include "Ray.h"
#include <vector>


namespace RGS {

	template<typename vertex_t>
	struct Triangle {
		//static_assert(std::is_base_of<VertexBase, vertex_t>::value, "vertex_t must be derived from VertexBase");
		
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
		static void FinalRasterize(Framebuffer& framebuffer, const DepthFuncType& depthFunc) {

			//MSAA
			int width = framebuffer.GetWidth();
			int height = framebuffer.GetHeight();
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					Vec3 color1 = framebuffer.MSAAGetColor(2*x, 2*y);
					float zdepth = framebuffer.MSAAGetDepth(2 * x, 2 * y);
					float depth = framebuffer.MSAAGetDepth(2 * x, 2 * y + 1);
					if (PassDepthTest(depth, zdepth, depthFunc)){
						color1 = color1 + framebuffer.MSAAGetColor(2 * x, 2 * y + 1);
					}
					else {
						color1 = framebuffer.MSAAGetColor(2 * x, 2 * y + 1);
						zdepth = depth;
					}
					depth = framebuffer.MSAAGetDepth(2 * x + 1, 2 * y);
					if (PassDepthTest(depth, zdepth, depthFunc)) {
						color1 = color1 + framebuffer.MSAAGetColor(2 * x, 2 * y + 1);
					}
					else {
						color1 = framebuffer.MSAAGetColor(2 * x, 2 * y + 1);
					}
					depth = framebuffer.MSAAGetDepth(2 * x + 1, 2 * y + 1);
					if (PassDepthTest(depth, zdepth, depthFunc)) {
						color1 = color1 + framebuffer.MSAAGetColor(2 * x, 2 * y + 1);
					}
					else {
						color1 = framebuffer.MSAAGetColor(2 * x, 2 * y + 1);
					}
					framebuffer.SetColor(x, y, color1 / 4.0f);
					
				}
			}
		}

		template<typename uniforms_t, typename varyings_t>
		static void CalculateMipmapLevel(const varyings_t(&varyings)[3],  uniforms_t& uniforms) {
			float du1 = varyings[1].TexCoord.X - varyings[0].TexCoord.X;
			float dv1 = varyings[1].TexCoord.Y - varyings[0].TexCoord.Y;
			float du2 = varyings[2].TexCoord.X - varyings[0].TexCoord.X;
			float dv2 = varyings[2].TexCoord.Y - varyings[0].TexCoord.Y;
			float dx1 = varyings[1].FragPos.X - varyings[0].FragPos.X;
			float dy1 = varyings[1].FragPos.Y - varyings[0].FragPos.Y;
			float dx2 = varyings[2].FragPos.X - varyings[0].FragPos.X;
			float dy2 = varyings[2].FragPos.Y - varyings[0].FragPos.Y;
			float det = (dx1 * dy2 - dx2 * dy1);
			float dudx = (du1 * dy2 - du2 * dy1) / det;
			float dvdx = (dv1 * dy2 - dv2 * dy1) / det;
			float dudy = (du2 * dx1 - du1 * dx2) / det;
			float dvdy = (dv2 * dx1 - dv1 * dx2) / det;
			uniforms.Lx = std::log2(sqrt(dudx * dudx + dvdx * dvdx));
			uniforms.Ly = std::log2(sqrt(dudy * dudy + dvdy * dvdy));
			//std::cout << uniforms.Lx << "  " << uniforms.Ly <<std::endl;
		}

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
			/*if (program.EnableBlend) {
				Vec3 dstColor = framebuffer.MSAAGetColor(x, y);
				Vec3 srcColor = color;
				float alpha = color.W;
				color = { Lerp(dstColor , srcColor , alpha) , 1.0f };
			}*/
			framebuffer.MSAASetColor(x, y, color);
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
			
			out.WorldPos = varyings[0].WorldPos * weights[0] +
				varyings[1].WorldPos * weights[1] +
				varyings[2].WorldPos * weights[2];

			out.WorldNormal = Normalize(varyings[0].WorldNormal * weights[0] +
				varyings[1].WorldNormal * weights[1] +
				varyings[2].WorldNormal * weights[2]);

			out.TexCoord = varyings[0].TexCoord * weights[0] +
				varyings[1].TexCoord * weights[1] +
				varyings[2].TexCoord * weights[2];

			out.CamPos = varyings[0].CamPos * weights[0] +
				varyings[1].CamPos * weights[1] +
				varyings[2].CamPos * weights[2];

		}

		template<typename varyings_t>
		static void RayLerpVaryings(varyings_t& out, const Triangle<varyings_t>& varyings, float(&weights)[3], const int width, const int height) {
			out.ClipPos = varyings[0].ClipPos * weights[0] + varyings[1].ClipPos * weights[1] + varyings[2].ClipPos * weights[2];
			out.NdcPos = out.ClipPos / out.ClipPos.W;
			out.NdcPos.W = 1.0f / out.ClipPos.W;

			out.FragPos.X = (out.NdcPos.X + 1.0f) * 0.5f * width;
			out.FragPos.Y = (out.NdcPos.Y + 1.0f) * 0.5f * height;
			out.FragPos.Z = (out.NdcPos.Z + 1.0f) * 0.5f;
			out.FragPos.W = out.NdcPos.W;

			out.WorldPos = varyings[0].WorldPos * weights[0] +
				varyings[1].WorldPos * weights[1] +
				varyings[2].WorldPos * weights[2];

			out.WorldNormal = Normalize(varyings[0].WorldNormal * weights[0] +
				varyings[1].WorldNormal * weights[1] +
				varyings[2].WorldNormal * weights[2]);

			out.TexCoord = varyings[0].TexCoord * weights[0] +
				varyings[1].TexCoord * weights[1] +
				varyings[2].TexCoord * weights[2];

			out.CamPos = varyings[0].CamPos * weights[0] +
				varyings[1].CamPos * weights[1] +
				varyings[2].CamPos * weights[2];

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
		static bool Intersect(const Ray& ray, double& t, float(&weights)[3], varyings_t& object) {

			// 计算三角形的两条边
			Vec3 E1 = object.Vertex[1].CamPos - object.Vertex[0].CamPos;
			Vec3 E2 = object.Vertex[2].CamPos - object.Vertex[0].CamPos;

			// 计算行列式
			Vec3 P = Cross(ray.Direction(), E2);
			double det = Dot(E1, P);

			// 检查光线是否与三角形平面平行
			if (fabs(det) < EPSILON) {
				return false;
			}

			// 计算逆行列式
			double invDet = 1.0 / det;

			// 计算 T 向量
			Vec3 T = ray.Origin() - object.Vertex[0].CamPos;

			// 计算 barycentric weights[0]
			weights[1] = Dot(T, P) * invDet;
			if (weights[1] < 0.0 || weights[1] > 1.0) {
				return false;
			}

			// 计算 Q 向量和 weights[1]
			Vec3 Q = Cross(T, E1);
			weights[2] = Dot(ray.Direction(), Q) * invDet;
			if (weights[2] < 0.0 || weights[2] + weights[1] > 1.0) {
				return false;
			}

			// 计算 weights[2] 和 t
			weights[0] = 1.0 - weights[2] - weights[1];
			t = Dot(E2, Q) * invDet;
			//std::cout << "det: " << det << ", t: " << t << std::endl;
			//std::cout << "weights[0]: " << weights[0] << ", weights[1]: " << weights[1] << ", weights[2]: " << weights[2] << std::endl;
			// 返回相交结果


			return t > EPSILON;
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
			for (int y = bBox.MinY * 2; y <= std::min(bBox.MaxY * 2 + 1, framebuffer.GetHeight() * 2 - 1); y+=1) {
				for (int x = bBox.MinX * 2; x <= std::min(bBox.MaxX * 2 + 1, framebuffer.GetWidth() * 2 - 1); x+=1) {
					float screenWeights[3];
					float weights[3];
					Vec2 screenPoint ={ ((float)x)*0.5f + 0.25f, ((float)y) * 0.5f + 0.25f };
				
					CalculateWeights(screenWeights, weights, fragCoords, screenPoint);

					if (!IsInsideTriangle(weights))continue;
					

					varyings_t pixVaryings;
					LerpVaryings(pixVaryings, varyings, weights, framebuffer.GetWidth(), framebuffer.GetWidth());

					//zbuffer
					if (program.EnableDepthTest) {
						float depth = pixVaryings.FragPos.Z;
						float zdepth = framebuffer.MSAAGetDepth(x, y);
						DepthFuncType depthFunc = program.DepthFunc;
						if (!PassDepthTest(depth,zdepth , depthFunc)) {
							continue;
						}
						framebuffer.MSAASetDepth(x, y, depth);
					}
					ProcessPixel(framebuffer, x, y, program, pixVaryings, uniforms);

				}
			}
			
		}

		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static Vec4 rayTrace(const Ray &ray,
			uniforms_t& uniforms,
			const Program<vertex_t, uniforms_t, varyings_t>& program,
			Framebuffer& framebuffer,
			std::vector< Triangle<varyings_t> > &v,
			int depth)
		{
			if (depth > uniforms.MaxDepth) {
				return Vec4(0.0f, 0.2f, 0.0f, 0.8f);
			}
			//检测碰撞
				//遍历对象池
			int ElementNum = -1;
			double tLast = 10000;
			double t = 0;
			float lastweights[3];
			float weights[3];
			for (int i = 0; i < v.size(); ++i) {
				if (Intersect(ray, t, weights,v[i])) {
					if (tLast > t) {
						tLast = t;
						lastweights[0] = weights[0];
						lastweights[1] = weights[1];
						lastweights[2] = weights[2];
						ElementNum = i;
					}
				}
			}
				//遍历光源
			/*for (int i = 0; i < v.size(); ++i) {
				if (Intersect(ray, t, weights,v[i])) {
					if (tLast > t) {
						tLast = t;
						ElementNum = i;
						return { 1.0f,1.0f,1.0f,0.8f };
					}
				}
			}*/
			//未检测到返回背景色
			if (ElementNum == -1) {
				return (0.0f, 0.2f, 0.0f, 0.8f);
			}
			varyings_t out;

			bool discard = false;
			RayLerpVaryings(out, v[ElementNum], lastweights, framebuffer.GetWidth(), framebuffer.GetHeight());
			Vec4 color = program.FragmentShader(discard, out, uniforms); //初始物体颜色
			Vec3 hitPoint = ray.Origin() + ray.Direction() * tLast;//碰撞点
			Vec3 normal = Vec3(uniforms.MV* Vec4(out.WorldNormal,0) );

			//反射		
			Vec3 reflectDir = ray.Direction() - normal *Dot(ray.Direction(), normal) * 2;
			Ray reflectRay = Ray(hitPoint, reflectDir);
			Vec4 reflectColor = rayTrace(reflectRay, uniforms, program, framebuffer, v,depth + 1) * uniforms.ReflectRatio;//设置反射系数

			//折射...
			//std::cout<< (color + reflectColor).X<<std::endl;
			return color + reflectColor;
		}
	public:
		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void Draw(Framebuffer& framebuffer,
			const Program<vertex_t, uniforms_t, varyings_t>& program, 
			const Triangle<vertex_t>& triangle, 
			uniforms_t& uniforms)
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

			//按三角形拆分图片
			for (int i = 0; i < vertexNum - 2; ++i) {
				varyings_t Triangle[3];
				Triangle[0] = varyings[0];
				Triangle[1] = varyings[i+1];
				Triangle[2] = varyings[i+2];
				//计算梯度
				//CalculateMipmapLevel(Triangle,uniforms);

				RasterizeTriangle(framebuffer, program, Triangle, uniforms);
			}
			FinalRasterize(framebuffer, program.DepthFunc);

		}

		template<typename vertex_t, typename uniforms_t, typename varyings_t>
		static void RayTracingDraw(Framebuffer& framebuffer,
			const Program<vertex_t, uniforms_t, varyings_t>& program,
			const Triangle<vertex_t>& triangle,
			uniforms_t& uniforms,
			std::vector< Triangle<varyings_t> > &v)
		{
			//检查是否继承自基础渲染类
			static_assert(std::is_base_of<VaryingBase, varyings_t>::value, "varyings_t must be derived from VaryingBase");
			static_assert(std::is_base_of<VertexBase, vertex_t>::value, "uniforms_t must be derived from VertexBase");

			varyings_t varyings[RGS_MAX_VARYINGS];
			for (int i = 0; i < 3; i++) {
				program.VertexShader(varyings[i], triangle[i], uniforms);
			}
			Triangle<varyings_t> tempTri;
			tempTri.Vertex[0] = varyings[0];
			tempTri.Vertex[1] = varyings[1];
			tempTri.Vertex[2] = varyings[2];
			v.push_back(tempTri);
			
			for (int x = 0;  x <= framebuffer.GetWidth() ;x++ ){
				float i = (float)x / framebuffer.GetWidth() - 0.5f;
				for (int y = 0; y <= framebuffer.GetHeight(); y++) {
					
					float j =  (float)y / framebuffer.GetHeight() - 0.5f;
					Vec3 screenDirection = {i , j , -0.4f };
					Ray ray = Ray({0.0f,0.0f,0.0f}, screenDirection);
					Vec4 color = rayTrace(ray, uniforms , program , framebuffer,v, 0 );
					framebuffer.SetColor(x, y, color);
				}
			}
			//std::cout << "111" << std::endl;
		}
	};
}