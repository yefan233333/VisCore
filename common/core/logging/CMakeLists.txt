# Logging CMakeLists.txt
# 无依赖
# 用于日志的输入输出，提供调试时的信息打印

find_package(OpenCV REQUIRED)
VisCore_add_module(logging
EXTERNAL ${OpenCV_LIBS})