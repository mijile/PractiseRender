#include "KDTree.h"
#include <iostream>
#define MAXDEPTH 4
namespace RGS {
	KDNode* BuildKDTree(std::vector<Mesh> Geos, int depth)
	{   
		Vec3 minLoc = { 10000,10000,10000 };
		Vec3 maxLoc = { -10000,-10000,-10000 };
        //确认包围盒
        if (Geos.size() >= 1) {
			for (auto& geom : Geos) {
				for (int i = 0; i < 3; ++i) {
					if (geom.minLoc[i] < minLoc[i]) minLoc[i] = geom.minLoc[i];
					if (geom.maxLoc[i] > maxLoc[i]) maxLoc[i] = geom.maxLoc[i];
				}
			}
        }


        if (Geos.size() <= 1 || depth > MAXDEPTH) {
            // 创建叶子节点
            //std::cout << "leaf"<< minLoc[0] << minLoc[1] << minLoc[2] << " " << maxLoc[0] << maxLoc[1] << maxLoc[2] << std::endl;
            return new KDNode{ true, -1, 0.0f, Geos, minLoc, maxLoc, nullptr, nullptr};
        }

        // 选择分割轴和分割平面
        int splitAxis = depth % 3; // 按 x, y, z 循环选择分割轴
        float splitPos = FindSplitPosition(Geos, splitAxis);

        // 划分几何体
        std::vector<Mesh> leftGeometries, rightGeometries;
        for (auto& geom : Geos) {
            float centroid = (geom.minLoc[splitAxis] + geom.maxLoc[splitAxis]) * 0.5f;
            if (centroid <= splitPos)
                leftGeometries.push_back(geom);
            else
                rightGeometries.push_back(geom);
        }

        // 创建内部节点
        KDNode* node = new KDNode{ false, splitAxis, splitPos, {}, minLoc, maxLoc, nullptr, nullptr };
        node->left = BuildKDTree(leftGeometries, depth + 1);
        node->right = BuildKDTree(rightGeometries, depth + 1);
        return node;
	}
    float FindSplitPosition(std::vector<Mesh>& Geos, int splitAxis)
    {
        float sum = 0.0f;
        for (auto& geom : Geos) {
            sum += (geom.minLoc[splitAxis] + geom.maxLoc[splitAxis]) / 2.0f;
        }
        return sum / Geos.size();
    }
    bool HitBox(const Ray& ray, double& t, Mesh& mesh)
    {
        double tmin = -INFINITY; // 初始化为负无穷大
        double tmax = INFINITY;  // 初始化为正无穷大
        Vec3 ori = ray.Origin();
        Vec3 dir = ray.Direction();
        for (int i = 0; i < 3; i++) { // 遍历 x, y, z 三个轴
            if (dir[i] == 0) { // 如果光线平行于该轴
                if (ori[i] < mesh.minLoc[i] || ori[i] >mesh.maxLoc[i]) {
                    return false; // 光线在轴上的投影不在盒子范围内
                }
            }
            else {
                double invDir = 1.0 / dir[i]; // 计算方向的倒数
                double t0 = (mesh.minLoc[i] - ori[i]) * invDir; // 进入时间
                double t1 = (mesh.maxLoc[i] - ori[i]) * invDir; // 离开时间

                if (t0 > t1) std::swap(t0, t1); // 确保 t0 是进入时间，t1 是离开时间

                tmin = std::max(tmin, t0); // 更新全局进入时间
                tmax = std::min(tmax, t1); // 更新全局离开时间
                
                // 如果时间区间无效，则不相交
                if (tmin > tmax) return false;
            }
        }
        /*if (tmin < 0) {
            return false;
        }*/
        t = tmin; // 返回最近的相交时间
        return true;
    }
    bool HitBox(const Ray& ray, Vec3 minLoc, Vec3 maxLoc, double& t)
    {
        double tmin = -INFINITY; // 初始化为负无穷大
        double tmax = INFINITY;  // 初始化为正无穷大
        Vec3 ori = ray.Origin();
        Vec3 dir = ray.Direction();
        for (int i = 0; i < 3; i++) { // 遍历 x, y, z 三个轴
            if (dir[i] == 0) { // 如果光线平行于该轴
                if (ori[i] < minLoc[i] || ori[i] >maxLoc[i]) {
                    return false; // 光线在轴上的投影不在盒子范围内
                }
            }
            else {
                double invDir = 1.0 / dir[i]; // 计算方向的倒数
                double t0 = (minLoc[i] - ori[i]) * invDir; // 进入时间
                double t1 = (maxLoc[i] - ori[i]) * invDir; // 离开时间

                if (t0 > t1) std::swap(t0, t1); // 确保 t0 是进入时间，t1 是离开时间

                tmin = std::max(tmin, t0); // 更新全局进入时间
                tmax = std::min(tmax, t1); // 更新全局离开时间

                
                // 如果时间区间无效，则不相交
                if (tmin > tmax) return false;
            }
        }
       /* if (tmin < 0) {
            return false;
        }*/
		t = tmin; // 返回最近的相交时间
        return true;
    }
    KDNode* HitBox(KDNode* node, const Ray& ray, double& nearestT)
    {
        if (!node) return nullptr;

        double tBox;
        if (!HitBox(ray, node->minLoc, node->maxLoc, tBox)) return nullptr;
        // 只处理最近的交点
        if (tBox > nearestT) return nullptr;

        if (node->isLeaf) {
            nearestT = tBox;
            return node;
        }

        double tLeft = nearestT, tRight = nearestT;
        KDNode* leftHit = HitBox(node->left, ray, tLeft);
        KDNode* rightHit = HitBox(node->right, ray, tRight);

        if (leftHit && rightHit) {
            if (tLeft < tRight) {
                nearestT = tLeft;
                return leftHit;
            }
            else {
                nearestT = tRight;
                return rightHit;
            }
        }
        else if (leftHit) {
            nearestT = tLeft;
            return leftHit;
        }
        else if (rightHit) {
            nearestT = tRight;
            return rightHit;
        }

        return nullptr;
    }
}
