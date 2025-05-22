#pragma once
#include "Ray.h"
#include "Mesh.h"
namespace RGS {
    struct KDNode {
        bool isLeaf;
        int splitAxis; // 分割轴 (0 = x, 1 = y, 2 = z)
        float splitPos; // 分割平面的位置
        std::vector<RGS::Mesh> meshGroup;// 叶子节点存储的几何体
        //包围盒
        Vec3 minLoc ;
        Vec3 maxLoc ;
        KDNode* left;  // 左子节点
        KDNode* right; // 右子节点
    };

    KDNode* BuildKDTree(std::vector<Mesh> Geos, int depth);
    float FindSplitPosition(std::vector<Mesh>& Geos, int splitAxis);
    //判断光线与子节点碰撞盒相交
    bool HitBox(const Ray& ray, double& t, Mesh& mesh);
    bool HitBox(const Ray& ray, Vec3 minLoc ,Vec3 maxLoc, double& t);
    KDNode* HitBox(KDNode* node, const Ray& ray, double& nearestT);
}