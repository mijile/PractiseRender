# RGS (Render Graphic System)

RGS 是一个使用 C++ 17 编写的高性能软件渲染引擎示例，旨在探索和练习图形学中的各种渲染算法。该项目涵盖了经典的软件渲染管线、基础光栅化以及更高级的光线追踪技术。

## 🌟 核心特性

- **软件光栅化 (Software Rasterization)**: 模拟基础的渲染管线，包括顶点变换、裁剪和片段着色。
- **光线追踪 (Ray Tracing)**: 实现了一个自定义的光线追踪器，支持：
  - **基础求交**: 射线与三角形的精准相交测试。
  - **KD-Tree 加速**: 使用 KD-Tree 结构对复杂模型进行空间划分，极大地提高了光线投射时的性能。
  - **多级反射/折射**: 模拟光线在物体表面的物理行为（如折射率 `refractiveIndex` 为 1.3 的透明效果）。
- **着色模型**: 
  - **Blinn-Phong**: 支持经典的光照模型（环境光、漫反射、高光）。
  - **纹理映射**: 支持读取并应用 Diffuse (漫反射) 和 Specular (高光) 纹理贴图。
- **资源加载**: 支持加载经典的 `.obj` 模型格式。
- **平台支持**: 提供了基于 Windows 的窗口抽象层，方便跨平台扩展。

## 📁 项目结构

- `src/RGS/`: 核心渲染逻辑及系统管理。
  - `Application.cpp/h`: 应用主循环、场景初始化、模型加载。
  - `Renderer.cpp/h`: 包含光栅化和光线追踪的核心绘制逻辑。
  - `KDTree.cpp/h`: KD-Tree 空间加速结构的实现。
  - `Window.h / WindowsWindow.cpp`: 抽象窗口接口及其 Windows 实现。
  - `vector.h`: 向量、矩阵数学运算库。
  - `Framebuffer.h`: 颜色缓冲区管理。
- `src/Shader/`: 着色器定义。
  - `ShaderBase.h`: 着色器基类。
  - `BlinnShader.h`: Blinn-Phong 光照着色器实现。
- `src/stb/`: 用于加载纹理图片的第三方库（stb_image）。
- `Assets/`: 常用的资源文件（.obj 模型、.png 纹理）。

## 🚀 快速开始

### 依赖项
- **CMake** (3.15 或更高版本)
- **C++ 编译器** (支持 C++ 17，如 MSVC, GCC 或 Clang)

### 构建步骤
1. 克隆项目或解压源码。
2. 在项目根目录下创建并进入构建目录：
   ```powershell
   mkdir build
   cd build
   ```
3. 运行 CMake 配置并构建：
   ```powershell
   cmake ..
   cmake --build . --config Release
   ```
4. 运行生成的可执行程序 `RGS.exe`（通常位于 `build/Release/`）。

## 🎮 操作控制

| 按键 | 功能 |
| :--- | :--- |
| **W / S** | 相机前后移动 |
| **A / D** | 相机左右移动 |
| **Space** | 相机向上移动 |
| **V** | 相机向下移动 |
| **Q / E** | 相机水平旋转 |

## 🛠️ 未来规划
- [ ] 增加更多纹理格式支持。
- [ ] 优化光线追踪中的采样算法（如抗锯齿、软阴影）。
- [ ] 引入阴影映射 (Shadow Mapping) 及其他高级光照效果。
- [ ] 实现多线程加速渲染。

---
*本项目仅供学习和研究使用。*
