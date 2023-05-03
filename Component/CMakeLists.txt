cmake_minimum_required(VERSION 3.0.0)
project(Component VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

# 宏定义参数
add_definitions(-DDREAMSKY_EXPORTS)
add_definitions(-DLONG_BITSTREAM_READER)
add_definitions(-DARCH_64BITS) #如果想要编译32位平台程序，需要在这里取消定义

if (WIN32)
    message(STATUS "build project on windows.")
elseif (UNIX)
    message(STATUS "build project on UNIX-like os.")
elseif (APPLE)
    message(AUTHOR_WARNING "not test on mac.")
endif(WIN32)

include_directories(
    include
)

file(GLOB_RECURSE component_src src/*.cpp)

if (enable_static)
# 静态库
add_library(${PROJECT_NAME}_static STATIC
    # component
    ${component_src}
)
# 静态库重命名
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
# 防止被清除
set_target_properties(${PROJECT_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif (enable_static)

# 共享库（动态库）
# 这里实际上可以直接使用 ${PROJECT_NAME} 而不是 ${PROJECT_NAME}_shared
# 但是为了区分，做了一下命名调整
add_library(${PROJECT_NAME}_shared SHARED
    # component
    ${component_src}
)
# 共享库重命名
set_target_properties(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
# 防止被清除
set_target_properties(${PROJECT_NAME}_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)

# 同时生成共享库和静态库的参考博客
# https://blog.csdn.net/u011436427/article/details/124618773

# 合并多个库为一个库的参考博客
# https://www.zhihu.com/question/439963507/answer/1688429415