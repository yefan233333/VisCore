#include <iostream>

#include <opencv2/opencv.hpp>
#include "vis_core/feature/standard_rect/standard_rect.h"


using namespace std;
using namespace cv;

int main()
{
    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        cerr << "❌ 无法打开摄像头\n";
        return 1;
    }

    auto detector = StandardRect::getDetector();
    auto camera_ptr = CameraWrapper::create();
    while(true)
    {
        Mat frame;
        cap.read(frame);
        imshow("Camera Feed", frame);
        auto img_ptr = ImageWrapper::create(frame);
        auto feature_list = detector->detect(img_ptr, camera_ptr);
        for (const auto& feature : feature_list)
        {
            // 处理检测到的特征
        }

        // 获取 hsv 图像
        const Mat& hsv_image = img_ptr->getImg("hsv");
        imshow("HSV Image", hsv_image);

        // 获取 binary 图像
        const Mat& binary_image = img_ptr->getImg("binary");
        imshow("Binary Image", binary_image);

        if (waitKey(30) >= 0) break;
    }
}