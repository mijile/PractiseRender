cmake_minimum_required(VERSION 3.15)
project(RGS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


set(SOURCES
    "src/stb/stb_image.cpp"
    "src/RGS/main.cpp"
    "src/RGS/Application.cpp"
    "src/RGS/Application.h"
    "src/RGS/Window.cpp"
    "src/RGS/Window.h"
    "src/RGS/WindowsWindow.cpp"
    "src/RGS/WindowsWindow.h"
    "src/RGS/Framebuffer.cpp"
    "src/RGS/Framebuffer.h"
    "src/RGS/vector.cpp"
    "src/RGS/vector.h"
    "src/RGS/Renderer.cpp"
    "src/RGS/Renderer.h"
    "src/Shader/BlinnShader.cpp"
    "src/Shader/BlinnShader.h"
    "src/Shader/ShaderBase.h"
    "src/RGS/Texture.cpp"
)

add_executable(RGS ${SOURCES})

target_include_directories(RGS PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/RGS
    ${CMAKE_SOURCE_DIR}/src/Shader
    ${CMAKE_SOURCE_DIR}/src/stb
)