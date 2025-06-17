#include "vis_core/feature/standard_rect/standard_rect_detector.h"
#include "vis_core/utils/param_manager/param_manager.h"

using namespace std;
using namespace cv;

/**
 * @brief 识别参数
 */
struct DetectorParams
{
    //! 白色区域的HSV阈值
    cv::Scalar lower_hsv = cv::Scalar(0, 0, 200);
    cv::Scalar upper_hsv = cv::Scalar(180, 25, 255);

    //! 是否启用颜色阈值调试模式
    bool color_threshold_debug = false;

    PARAM_MANAGER_INIT(DetectorParams,
                       PARAM_MANAGER_ADD_PARAM(lower_hsv);
                       PARAM_MANAGER_ADD_PARAM(upper_hsv);
                       PARAM_MANAGER_ADD_PARAM(color_threshold_debug););
};
inline DetectorParams detector_params;

StandardRectDetector::Ptr StandardRectDetector::create()
{
    return make_shared<StandardRectDetector>();
}

auto StandardRectDetector::detect(Img_ptr &img_ptr, const Camera_ptr &camera_ptr) -> std::vector<StandardRect_ptr>
{
    if (!img_ptr)
    {
        VISCORE_THROW_ERROR("输入图像不能为空");
    }
    return detectImpl(img_ptr, camera_ptr);
}

auto StandardRectDetector::detectImpl(Img_ptr &img_ptr, const Camera_ptr &camera_ptr) -> std::vector<StandardRect_ptr>
{
    // 数据存储
    setSourceImage(img_ptr->img());
    setCamera(camera_ptr);
    if (!img_ptr->hasImg("binary"))
        binarize(img_ptr);
    setBinaryImage(img_ptr->getImg("binary"));
    return {};
}



void StandardRectDetector::binarize(Img_ptr &img_ptr)
{
    Mat src = img_ptr->img();
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    Mat binary;
    inRange(hsv, detector_params.lower_hsv, detector_params.upper_hsv, binary);

    // 颜色阈值调试模式
    if(detector_params.color_threshold_debug)
    {
        static bool gui_ready = false;

        // 用 int 数组承接 GUI 数值
        static int lower_val[3] = {static_cast<int>(detector_params.lower_hsv[0]),
                                   static_cast<int>(detector_params.lower_hsv[1]),
                                   static_cast<int>(detector_params.lower_hsv[2])};

        static int upper_val[3] = {static_cast<int>(detector_params.upper_hsv[0]),
                                   static_cast<int>(detector_params.upper_hsv[1]),
                                   static_cast<int>(detector_params.upper_hsv[2])};

        if (!gui_ready)
        {
            constexpr int SCR_W = 1920, SCR_H = 1080;
            cv::namedWindow("HSV", cv::WINDOW_NORMAL);
            cv::namedWindow("Binary", cv::WINDOW_NORMAL);
            cv::resizeWindow("HSV", SCR_W / 2, SCR_H);
            cv::resizeWindow("Binary", SCR_W / 2, SCR_H);
            cv::moveWindow("HSV", 0, 0);
            cv::moveWindow("Binary", SCR_W / 2, 0);

            auto addTrack = [&](const std::string &n, int *v, int max)
            { cv::createTrackbar(n, "HSV", v, max, nullptr); };

            addTrack("Lower H", &lower_val[0], 180);
            addTrack("Lower S", &lower_val[1], 255);
            addTrack("Lower V", &lower_val[2], 255);
            addTrack("Upper H", &upper_val[0], 180);
            addTrack("Upper S", &upper_val[1], 255);
            addTrack("Upper V", &upper_val[2], 255);

            gui_ready = true;
        }

        // 打印当前阈值
        std::cout << "Lower HSV: " << detector_params.lower_hsv << std::endl;
        std::cout << "Upper HSV: " << detector_params.upper_hsv << std::endl;

        /* 3-2. 每帧把 GUI → Scalar，再 inRange、显示 */
        detector_params.lower_hsv = cv::Scalar(lower_val[0], lower_val[1], lower_val[2]);
        detector_params.upper_hsv = cv::Scalar(upper_val[0], upper_val[1], upper_val[2]);

        cv::inRange(hsv, detector_params.lower_hsv, detector_params.upper_hsv, binary);
        cv::imshow("HSV", hsv);
        cv::imshow("Binary", binary);
    }

    // 对 binary 图像做腐蚀处理
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    erode(binary, binary, kernel, Point(-1, -1), 3);
    img_ptr->setImg("binary", binary);
    img_ptr->setImg("hsv", hsv);
}