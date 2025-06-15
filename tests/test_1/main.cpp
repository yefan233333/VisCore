#include <opencv2/opencv.hpp>
// #include "transform6D.hpp"
#include "vis_core/pose_proc/transform6D.hpp"
#include <iostream>
#include <cmath>

#include "vis_core/param_manager/param_manager.h"

struct TestParam
{
    double param1 = 0.0;
    int param2 = 0;
    std::vector<std::string> param3 = {"default1", "default2"};
    cv::Mat mat_param;

    PARAM_MANAGER_INIT(TestParam, 
        PARAM_MANAGER_ADD_PARAM(param1);
        PARAM_MANAGER_ADD_PARAM(param2);
        PARAM_MANAGER_ADD_PARAM(param3);
        PARAM_MANAGER_ADD_PARAM(mat_param);
    );
};
inline TestParam g_test_param;


int main()
{
    // 输出测试参数的默认值
    std::cout << "param1: " << g_test_param.param1 << std::endl;
    std::cout << "param2: " << g_test_param.param2 << std::endl;
    std::cout << "param3: ";
    for (const auto &str : g_test_param.param3)
    {
        std::cout << str << " ";
    }
    std::cout << "mat_param: " << g_test_param.mat_param.size() << " " 
              << g_test_param.mat_param.type() << std::endl;

    std::cout << std::endl;

    return 0;
}
