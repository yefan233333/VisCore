// main.cpp ---------------------------------------------------------------
// #include "quadrilateral_base.hpp" // 你的头文件路径

#include "vis_core/visual/feature_node/quadrilateral.h"
#include "vis_core/utils/camera/camera_wrapper.h"
// main.cpp ---------------------------------------------------------------
// #include "quadrilateral_base.hpp" // 你的头文件路径
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <numeric>
#include <iostream>

// ---------- 帮助函数：按“左上、右上、右下、左下”顺时针排序 ----------
static std::vector<cv::Point2f> sortClockwise(const std::vector<cv::Point2f> &pts)
{
    std::vector<cv::Point2f> out(4);
    // 质心
    cv::Point2f c = std::accumulate(pts.begin(), pts.end(), cv::Point2f{}) * (1.f / 4);
    // 极角排序
    std::vector<std::pair<double, int>> ang;
    for (int i = 0; i < 4; ++i)
        ang.emplace_back(std::atan2(pts[i].y - c.y, pts[i].x - c.x), i);
    std::sort(ang.begin(), ang.end(), [](auto a, auto b)
              { return a.first < b.first; });
    // 保证 0 是左上
    for (int i = 0; i < 4; ++i)
        out[i] = pts[ang[i].second];
    // 如果第一条边更接近水平而非竖直，可旋转顺序
    if (out[1].y > out[0].y)
        std::rotate(out.begin(), out.begin() + 1, out.end());
    return out;
}

int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "❌ 无法打开摄像头\n";
        return 1;
    }

    const double MIN_AREA = 6e3; // 忽略太小的轮廓
    bool paused = false;

    while (true)
    {
        cv::Mat frame;
        if (!paused)
            cap >> frame;
        if (frame.empty())
            break;

        // ---------- 预处理 ----------
        cv::Mat gray, blur, bw;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blur, {5, 5}, 0);
        cv::threshold(blur, bw, 100, 255, cv::THRESH_OTSU | cv::THRESH_BINARY);

        cv::imshow("binary", bw);

        // ---------- 轮廓检测 ----------
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(bw, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        double bestArea = 0;
        std::vector<cv::Point2f> bestQuad;

        for (auto &c : contours)
        {
            double peri = cv::arcLength(c, true);
            std::vector<cv::Point> approx;
            cv::approxPolyDP(c, approx, 0.02 * peri, true);
            if (approx.size() != 4 || !cv::isContourConvex(approx))
                continue;

            double area = std::fabs(cv::contourArea(approx));
            if (area < MIN_AREA)
                continue;

            if (area > bestArea)
            {
                bestArea = area;
                bestQuad.clear();
                for (auto &p : approx)
                    bestQuad.emplace_back(p);
            }
        }

        // ---------- 若找到矩形，调用 QuadrilateralBase 绘制 ----------
        if (!bestQuad.empty())
        {
            auto ordered = sortClockwise(bestQuad);
            auto quadPtr = QuadrilateralBase::create(ordered);

            quadPtr->drawFeature(
                frame,
                cv::Scalar(0, 255, 0), // 绿色
                2,
                QuadrilateralBase::DrawBorder |
                    QuadrilateralBase::DrawCorners |
                    QuadrilateralBase::DrawContour |
                    QuadrilateralBase::DrawCornerLabels);
        }

        cv::imshow("Quadrilateral Detector", frame);
        int key = cv::waitKey(1);
        if (key == 27 || key == 'q')
            break; // Esc / q 退出
        if (key == 'p')
            paused = !paused; // p 暂停
    }
    return 0;
}
