cmake_minimum_required(VERSION 3.15)
project(RGS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


set(SOURCES
    "src/stb/stb_image.cpp"
    "src/RGS/main.cpp"
    "src/RGS/Application.cpp"
    "src/RGS/Window.cpp"
    "src/RGS/WindowsWindow.cpp"
    "src/RGS/Framebuffer.cpp"
    "src/RGS/vector.cpp"
    "src/RGS/Renderer.cpp"
    "src/Shader/BlinnShader.cpp"
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