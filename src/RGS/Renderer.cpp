#include "vector.h"
#include "Renderer.h"
namespace RGS {
	bool Renderer::IsVertexVisible(const Vec4& clipPos) {
		//根据-1<=x/w<=1判断是否在视锥体内
		return clipPos.X >= -clipPos.W && clipPos.X <= clipPos.W &&
			clipPos.Y >= -clipPos.W && clipPos.Y <= clipPos.W &&
			clipPos.Z >= -clipPos.W && clipPos.Z <= clipPos.W;
	}
	bool Renderer::IsInsidePlane(const Vec4& clipPos, const Plane plane)
	{
		switch (plane)
		{
		case Plane::POSITIVE_W:
			return clipPos.W >= 0.0f;
		case Plane::POSITIVE_X:
			return clipPos.X <= clipPos.W;
		case Plane::NEGATIVE_X:
			return clipPos.X >= -clipPos.W;
		case Plane::POSITIVE_Y:
			return clipPos.Y <= clipPos.W;
		case Plane::NEGATIVE_Y:
			return clipPos.Y >= -clipPos.W;
		case Plane::POSITIVE_Z:
			return clipPos.Z <= +clipPos.W;
		case Plane::NEGATIVE_Z:
			return clipPos.Z >= -clipPos.W;
		default:
			ASSERT(false);
			return false;
		}
	}
	Renderer::BoundingBox Renderer::GetBoundingBox(const Vec4(&fragCoords)[3], const int width, const int height)
	{
		auto x_list = { fragCoords[0].X,fragCoords[1].X,fragCoords[2].X };
		auto y_list = { fragCoords[0].Y,fragCoords[1].Y,fragCoords[2].Y };
		float minX = std::min<float>(x_list);
		float minY = std::min<float>(y_list);
		float maxX = std::max<float>(x_list);
		float maxY = std::max<float>(y_list);

		minX = Clamp(minX, 0.0f, (float)(width - 1));
		minY = Clamp(minY, 0.0f, (float)(height - 1));
		maxX = Clamp(maxX, 0.0f, (float)(width - 1));
		maxY = Clamp(maxY, 0.0f, (float)(height - 1));

		BoundingBox bBox;
		bBox.MaxX = std::ceil(maxX);
		bBox.MaxY = std::ceil(maxY);
		bBox.MinX = std::floor(minX);
		bBox.MinY = std::floor(minY);

		return bBox;

	}
	float Renderer::GetIntersectRatio(const Vec4& prev, const Vec4& curr, const Plane plane) {
		switch (plane) {
		case Plane::POSITIVE_W:
			return prev.W / (prev.W - curr.W);
		case Plane::NEGATIVE_X:
			return (prev.W + prev.X) / ((prev.W + prev.X) - (curr.W + curr.X));
		case Plane::POSITIVE_X:
			return (prev.W - prev.X) / ((prev.W - prev.X) - (curr.W - curr.X));
		case Plane::NEGATIVE_Y:
			return (prev.W + prev.Y) / ((prev.W + prev.Y) - (curr.W + curr.Y));
		case Plane::POSITIVE_Y:
			return (prev.W - prev.Y) / ((prev.W - prev.Y) - (curr.W - curr.Y));
		case Plane::NEGATIVE_Z:
			return (prev.W + prev.Z) / ((prev.W + prev.Z) - (curr.W + curr.Z));
		case Plane::POSITIVE_Z:
			return (prev.W - prev.Z) / ((prev.W - prev.Z) - (curr.W - curr.Z));
		default:
			ASSERT(false);
			return 0.0f;
		}
	}

	bool Renderer::IsInsideTriangle(float(&weights)[3])
	{
		return weights[0] >= EPSLION && weights[1] >= EPSLION && weights[2] >= EPSLION;
	}

	bool Renderer::IsBackFacing(const Vec4& v1, const Vec4& v2, const Vec4& v3)
	{
		//判断连续的两边方向是顺时针还是逆时针来确定面的正反
		float signedArea = v1.X * v2.Y - v1.Y * v2.X +
						   v2.X * v3.Y - v2.Y * v3.X +
						   v3.X * v1.Y - v3.Y * v1.X;
		return signedArea <= 0.0f;
	}

	bool Renderer::PassDepthTest(const float& depth, const float& zdepth, const DepthFuncType& depthFunc)
	{
		switch (depthFunc) {
		case DepthFuncType::ALWAYS:
			return true;
		case DepthFuncType::LESS:
			return depth < zdepth;
		case DepthFuncType::LEQUAL:
			return depth <= zdepth;
		default:
			return false;
		}
	}

	void Renderer::CalculateWeights(float(&screenWeights)[3], 
									float(&weights)[3], 
									const Vec4(&fragCoords)[3], 
									const Vec2 screenPoint)
	{
		Vec2 ab = fragCoords[1] - fragCoords[0];
		Vec2 ac = fragCoords[2] - fragCoords[0];
		Vec2 ap = screenPoint - Vec2(fragCoords[0]);
		float factor = 1.0f / (ab.X * ac.Y - ab.Y * ac.X);
		float s = (ac.Y * ap.X - ac.X * ap.Y) * factor;
		float t = (ab.X * ap.Y - ab.Y * ap.X) * factor;
		screenWeights[0] = 1 - s - t;
		screenWeights[1] = s;
		screenWeights[2] = t;

		float w0 = fragCoords[0].W * screenWeights[0];
		float w1 = fragCoords[1].W * screenWeights[1];
		float w2 = fragCoords[2].W * screenWeights[2];
		float normalizer = 1.0f / (w0 + w1 + w2);
		weights[0] = w0 * normalizer;
		weights[1] = w1 * normalizer;
		weights[2] = w2 * normalizer;
	}


}