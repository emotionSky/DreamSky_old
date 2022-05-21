cmake_minimum_required(VERSION 3.0.0)
project(Log VERSION 0.1.0)

# 宏定义参数
add_definitions(-DLOG_EXPORTS)

if (WIN32)
add_definitions(-DWIN32)
endif(WIN32)

include_directories(
    include
)

add_library(${PROJECT_NAME} SHARED
    src/Log/log_core.c
    src/Log/log_printf.c
)

