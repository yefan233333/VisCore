#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include "vis_core/contourproc/contour_wrapper.hpp"

// 打印轮廓的基本信息
void printContourInfo(const std::string &name, const ContourWrapper<int> &wrapper)
{
    try
    {
        auto center = wrapper.calculateCenterImpl();
        std::cout << "[" << name << "] 点数: " << wrapper.getPoints().size()
                  << ", 质心: (" << center.x << ", " << center.y << ")"
                  << ", 面积: " << wrapper.calculateAreaImpl() << "\n";
    }
    catch (const std::exception &e)
    {
        std::cout << "[" << name << "] 错误: " << e.what() << "\n";
    }
}

int main()
{
    // 创建测试图像（500x500白色背景）
    cv::Mat img(500, 500, CV_8UC3, cv::Scalar(255, 255, 255));

    // 定义测试轮廓点（不规则形状）
    std::vector<cv::Point> contourPoints = {
        {100, 100}, {150, 50}, {200, 70}, {250, 100}, {220, 200}, {180, 220}, {120, 200}, {80, 150}};

    // 1. 创建原始轮廓包装器并计算属性
    std::cout << "\n==== 创建原始轮廓 ====\n";
    ContourWrapper<int> originalWrapper(contourPoints);
    printContourInfo("原始轮廓", originalWrapper);
    auto originalCenter = originalWrapper.calculateCenterImpl();

    // 2. 测试拷贝构造
    std::cout << "\n==== 测试拷贝构造 ====\n";
    {
        ContourWrapper<int> copiedWrapper(originalWrapper); // 拷贝构造
        printContourInfo("拷贝构造副本", copiedWrapper);

        // 验证拷贝后的数据
        if (originalWrapper.getPoints().size() == copiedWrapper.getPoints().size())
        {
            std::cout << "√ 拷贝构造: 点数匹配\n";
        }

        // 计算一些属性
        copiedWrapper.calculateCircularityImpl();
        copiedWrapper.calculateMinAreaRectImpl();

        // 打印地址以验证它们是否为不同的对象
        std::cout << "原始轮廓地址: " << &originalWrapper << "\n";
        std::cout << "拷贝副本地址: " << &copiedWrapper << "\n";
    } // 离开作用域，copiedWrapper析构

    std::cout << "\n离开作用域后: \n";
    printContourInfo("原始轮廓", originalWrapper); // 原始对象应不受影响

    // 3. 测试移动构造
    std::cout << "\n==== 测试移动构造 ====\n";
    ContourWrapper<int> movedWrapper(std::move(originalWrapper)); // 移动构造

    try
    {
        std::cout << "尝试访问已移动的原始轮廓: \n";
        printContourInfo("原始轮廓(已移动)", originalWrapper); // 应抛出异常
    }
    catch (const std::exception &e)
    {
        std::cout << "√ 正确捕获异常: " << e.what() << "\n";
    }

    std::cout << "\n移动后的新轮廓: \n";
    printContourInfo("移动构造的新轮廓", movedWrapper);
    std::cout << "√ 移动构造后新轮廓有效\n";

    // 4. 测试拷贝赋值
    std::cout << "\n==== 测试拷贝赋值 ====\n";
    ContourWrapper<int> assignedWrapper({{300, 300}, {350, 350}, {320, 380}});
    std::cout << "赋值前的轮廓: \n";
    printContourInfo("赋值目标", assignedWrapper);

    assignedWrapper = movedWrapper; // 拷贝赋值

    std::cout << "\n赋值后的轮廓: \n";
    printContourInfo("赋值目标", assignedWrapper);

    // 5. 测试移动赋值
    std::cout << "\n==== 测试移动赋值 ====\n";
    ContourWrapper<int> anotherWrapper({{50, 400}, {100, 450}, {80, 420}});
    std::cout << "移动赋值前的轮廓: \n";
    printContourInfo("移动赋值目标", anotherWrapper);

    anotherWrapper = std::move(assignedWrapper); // 移动赋值

    try
    {
        std::cout << "\n尝试访问源轮廓: \n";
        printContourInfo("移动赋值源", assignedWrapper); // 应抛出异常
    }
    catch (const std::exception &e)
    {
        std::cout << "√ 正确捕获异常: " << e.what() << "\n";
    }

    std::cout << "\n移动赋值后的目标轮廓: \n";
    printContourInfo("移动赋值目标", anotherWrapper);

    // 6. 可视化测试结果
    std::cout << "\n==== 可视化测试结果 ====\n";
    // 绘制原始轮廓点（红色）
    for (const auto &pt : contourPoints)
    {
        cv::circle(img, pt, 4, cv::Scalar(0, 0, 255), -1);
    }

    // 绘制原始轮廓线（蓝色）
    std::vector<std::vector<cv::Point>> contours = {contourPoints};
    cv::drawContours(img, contours, -1, cv::Scalar(255, 0, 0), 2);

    // 绘制原质心（红色X）
    cv::drawMarker(img, originalCenter, cv::Scalar(0, 0, 255), cv::MARKER_CROSS, 15, 2);

    // 绘制当前轮廓的质心（绿色）
    auto currentCenter = anotherWrapper.calculateCenterImpl();
    cv::circle(img, currentCenter, 8, cv::Scalar(0, 255, 0), 2);

    // 添加图例说明
    cv::putText(img, "Original points", cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(img, "Original contour", cv::Point(10, 60),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 0, 0), 2);
    cv::putText(img, "Original centroid (RED X)", cv::Point(10, 90),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
    cv::putText(img, "Current centroid (GREEN)", cv::Point(10, 120),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    cv::putText(img, "Copied/Moved: Centroid should be at same position",
                cv::Point(10, 450), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);

    // 显示结果
    cv::imshow("Contour Copy & Move Test", img);
    cv::waitKey(0);

    return 0;
}