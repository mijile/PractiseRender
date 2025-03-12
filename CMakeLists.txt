# 要求最低Cmake版本
cmake_minimum_required(VERSION 3.15)
# 项目名称   xxx + 使用语言
project(RGS LANGUAGES CXX)
# 使用语言标准
set(CMAKE_CXX_STANDARD 17)

#	设置变量方法
#	set ( 变量名
#		 定义
#		....)
set(HEADERS

)

set(SOURCES
	"src/Main.cpp"
	"src/Application.cpp"
)

#	后置附加信息
#	set(变量名 $(变量名) 新信息...)

set(TARGET RGS)
add_executable(${TARGET} ${HEADERS} ${SOURCES})