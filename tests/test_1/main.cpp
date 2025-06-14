#include <opencv2/opencv.hpp>
// #include "transform6D.hpp"
#include "vis_core/pose_proc/transform6D.hpp"
#include <iostream>
#include <cmath>

int main()
{
    // 1. 定义立方体顶点和边
    std::vector<cv::Point3f> pts = {{-0.5f, -0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f}, {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}};
    std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    // 2. 相机参数
    cv::Mat K = (cv::Mat_<double>(3, 3) << 700, 0, 400, 0, 700, 300, 0, 0, 1);
    cv::Mat dist = cv::Mat::zeros(4, 1, CV_64F);

    // 3. 准备基础变换，并确保都在视野中
    Transform6D baseT0;
    baseT0.tvec(cv::Vec3d(0, 0, 6)); // 移入视野

    Transform6D baseA(cv::Vec3f(0, 0, 0), cv::Vec3f(0, 0, 6));

    Transform6D baseB(cv::Matx33d::eye(), cv::Vec3d(3, 1, 6));

    Transform6D baseC = baseT0;
    baseC.rvec(cv::Vec3d(20 * CV_PI / 180., 30 * CV_PI / 180., 40 * CV_PI / 180.));
    baseC.tvec(cv::Point3d(1.5, 0.5, 6.));

    Transform6D baseD = baseA;
    baseD += baseC;

    Transform6D baseE = baseD;
    baseE -= baseB;

    struct Item
    {
        Transform6D base;
        cv::Scalar color;
        std::string name;
        float phase;
    };
    std::vector<Item> items = {
        {baseT0, cv::Scalar(180, 180, 180), "T0", 0.0f},
        {baseA, cv::Scalar(0, 255, 0), "A", 0.3f},
        {baseB, cv::Scalar(255, 0, 0), "B", 0.6f},
        {baseC, cv::Scalar(0, 0, 255), "C", 0.9f},
        {baseD, cv::Scalar(255, 255, 0), "D", 1.2f},
        {baseE, cv::Scalar(255, 0, 255), "E", 1.5f}};

    cv::namedWindow("Transform6D 修正版动画");
    const int numFrames = 600;
    for (int f = 0; f < numFrames; ++f)
    {
        double t = f * 0.02; // 时间参数
        cv::Mat canvas(720, 1024, CV_8UC3, cv::Scalar(20, 20, 20));

        for (size_t i = 0; i < items.size(); ++i)
        {
            // 动态旋转：绕 X, Y, Z 轴组合
            auto dynR = items[i].base.rvec();
            dynR[0] += std::sin(t + items[i].phase) * 0.5;
            dynR[1] += std::cos(t + items[i].phase) * 0.7;
            dynR[2] += std::sin(2 * t + items[i].phase) * 0.3;

            // 动态平移：sin 波动
            auto dynT = items[i].base.tvec();
            dynT[0] += std::sin(t * 1.5 + items[i].phase) * 1.0;
            dynT[1] += std::cos(t * 1.2 + items[i].phase) * 0.5;

            Transform6D Tdyn = items[i].base;
            Tdyn.rvec(dynR);
            Tdyn.tvec(dynT);

            // 投影并绘制
            std::vector<cv::Point2f> proj;
            cv::projectPoints(pts, Tdyn.rvec(), Tdyn.tvec(), K, dist, proj);
            cv::Point2f offset(150 * (int)i, 0);
            int thickness = 1 + int((std::sin(t + items[i].phase) + 1) * 1.5);
            for (auto &e : edges)
                cv::line(canvas, proj[e.first] + offset, proj[e.second] + offset, items[i].color, thickness);
            cv::putText(canvas, items[i].name, offset + cv::Point2f(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, items[i].color, 2);
        }

        cv::imshow("Transform6D 修正版动画", canvas);
        char key = (char)cv::waitKey(16);
        if (key == 27)
            break;
    }
    cv::destroyAllWindows();
    return 0;
}
