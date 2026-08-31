cmake_minimum_required(VERSION 3.10)
project(muduo_mine)

#muduo_mine最终编译成so动态库，设置动态库路径，放在根目录的lib文件夹下
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

#设置调试信息以及启动c++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

#定义参与编译的源文件
aux_source_directory(. SRC_LIST)

#编译生成动态库muduo_mine
add_library(muduo_mine SHARED ${SRC_LIST})