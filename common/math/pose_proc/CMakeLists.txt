# pose CMakeLists.txt
# 需要依赖于 logging


VisCore_add_module(pose_proc
INTERFACE 
DEPENDS logging geom_utils)