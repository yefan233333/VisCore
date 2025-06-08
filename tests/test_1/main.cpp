#include <iostream>
#include <vector>
#include <random>
#include <opencv2/opencv.hpp>
#include "vis_core/imgproc/image_wrapper.hpp"       // 包含您的ImageWrapper头文件
#include "vis_core/contourproc/contourproc.h"

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap(0); // 打开默认摄像头
    if (!cap.isOpened())
    {
        cerr << "Error: Could not open camera." << endl;
        return -1;
    }
    while(true)
    {
        Mat frame;
        cap >> frame; // 捕获一帧图像
        if (frame.empty())
        {
            cerr << "Error: Could not capture frame." << endl;
            break;
        }

        ImageWrapper imgWrapper = ImageWrapper::create(move(frame)); // 创建ImageWrapper对象
        {
            Mat gray;
            cvtColor(imgWrapper.img(), gray, COLOR_BGR2GRAY); // 转为灰度图
            imgWrapper.setImg("gray", gray); // 设置处理后的图像

            Mat bin;
            threshold(gray, bin, 128, 255, THRESH_BINARY); // 二值化处理
            imgWrapper.setImg("binary", bin); // 设置二值化图像

            // 查找轮廓
            vector<Contour_ptr> contours;
            findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            imgWrapper.setImg("gray", std::move(gray)); // 更新灰度图像
            imgWrapper.setImg("binary", std::move(bin)); // 更新二值化图像
            imgWrapper.setContourGroup("bin_contours", std::move(contours)); // 设置轮廓组
        }


        {
            // 显示处理后的图像
            Mat grayImg = imgWrapper.img("gray");
            Mat binImg = imgWrapper.img("binary");
            imshow("Gray Image", grayImg);
            imshow("Binary Image", binImg);
            // 绘制轮廓
            const auto& contoursGroup = imgWrapper.contour_group("bin_contours");
            Mat contourImg = grayImg.clone(); // 在灰度图上绘制轮廓
            for (const auto& contour : contoursGroup)
            {
                if (contour)
                {
                    cv::polylines(contourImg, contour->points(), true, Scalar(0, 255, 0), 2); // 绘制轮廓
                }
            }
            imshow("Contours", contourImg); // 显示轮廓图像
            // 显示原始图像
            imshow("Original Image", imgWrapper.img());
        }
        



        if (waitKey(30) >= 0) break; // 按任意键退出
    }
}