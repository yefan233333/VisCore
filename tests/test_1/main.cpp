#include "vis_core/contourproc/extensions.hpp"

using namespace std;
using namespace cv;


void test1()
{
    // 创建相机
    VideoCapture camera(2);
    if (!camera.isOpened())
    {
        cerr << "无法打开相机" << endl;
        return;
    }
    // 设置相机参数
    while(true)
    {
        Mat frame;
        camera >> frame; // 捕获一帧图像
        if (frame.empty())
        {
            cerr << "无法捕获图像" << endl;
            break;
        }

        // 转换为灰度图像
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // 二值化处理
        Mat binary;
        threshold(gray, binary, 128, 255, THRESH_BINARY);

        // 查找轮廓
        vector<Contour_ptr> contours;
        findContours(binary, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);


        for (const auto& contour : contours)
        {
            const auto& points = contour->points();
            polylines(frame, points, true, Scalar(0, 255, 0), 2); // 绘制轮廓
            auto center = contour->center();
            circle(frame, center, 5, Scalar(0, 0, 255), -1); // 绘制质心
        }

        imshow("轮廓检测", frame);
        if (waitKey(30) >= 0) break; // 按任意键退出
    }
}

int main()
{
    test1(); // 调用测试函数
    return 0; // 返回0表示程序正常结束
}