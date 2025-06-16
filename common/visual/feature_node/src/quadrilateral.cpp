#include "vis_core/visual/feature_node/quadrilateral.h"
#include "vis_core/core/logging/logging.h"

using namespace std;
using namespace cv;


QuadrilateralBase::Ptr QuadrilateralBase::create(const std::vector<cv::Point2f> &corners)
{
    if (corners.size() != 4)
    {
        VISCORE_WARNING_INFO("QuadrilateralBase::create , 角点数量不为 4 : %i",static_cast<int>(corners.size()));
    }
    auto instance = std::make_shared<QuadrilateralBase>();
    instance->getImageCache().setCorners(corners);
    return instance;
}


/**
 * @brief 线宽限制
 */
inline int limitThickness(int thickness)
{
    // 线条的最小和最大粗细限制
    constexpr int min_thickness = 1;
    constexpr int max_thickness = 10;

    // 限制线条粗细在 [min_thickness, max_thickness] 范围内
    return std::max(min_thickness, std::min(thickness, max_thickness));
}

/**
 * @brief 绘制四边型的四个角点
 *
 * @param image 要绘制的图像
 * @param corners 四边型的角点
 * @param color 绘制颜色
 * @param thickness 绘制线条的粗细
 */
inline void drawCorners(cv::Mat &image, const std::vector<cv::Point2f> &corners, const cv::Scalar &color, int thickness)
{
    thickness = limitThickness(thickness);
    int radius = 5 * thickness;

    for (const auto &corner : corners)
    {
        circle(image, corner, radius, color, thickness);
    }
}

/**
 * @brief 绘制边框
 *
 * @param image 要绘制的图像
 * @param corners 四边型的角点
 * @param color 绘制颜色
 * @param thickness 绘制线条的粗细
 */
inline void drawQuadrilateralSide(cv::Mat &image, const std::vector<cv::Point2f> &corners, const cv::Scalar &color, int thickness)
{
    thickness = limitThickness(thickness);
    size_t corner_count = corners.size();
    if (corner_count != 4)
    {
        // VISCORE_WARNING_INFO("drawQuadrilateralSide", "角点数量不为 4 : " + std::to_string(corner_count));
        VISCORE_WARNING_INFO("drawQuadrilateralSide , 角点数量不为 4 : %i", static_cast<int>(corner_count));
        return; // 如果角点数量不为4，直接返回
    }
    // 绘制四边形的轮廓线
    for (size_t i = 0; i < corner_count; ++i)
    {
        size_t nextIndex = (i + 1) % corner_count;
        line(image, corners[i], corners[nextIndex], color, thickness);
    }
}


/**
 * @brief 标注角点的顺序
 *
 * @param image 要绘制的图像
 * @param corners 四边型的角点
 * @param color 绘制颜色
 * @param thickness 绘制线条的粗细
 */
inline void labelCorners(cv::Mat &image, const std::vector<cv::Point2f> &corners, const cv::Scalar &color, int thickness)
{
    // 选择合适的文字大小，使得文字的大小约为线条粗细的两倍
    double fontScale = 0.5 * thickness;
    thickness = limitThickness(thickness);
    // 绘制角点的索引
    for (size_t i = 0; i < corners.size(); ++i)
    {
        const auto &corner = corners[i];
        // 在角点位置绘制索引
        putText(image, to_string(i), corner + Point2f(10, 10), FONT_HERSHEY_SIMPLEX, fontScale, color, thickness);
    }
}

void QuadrilateralBase::drawFeatureImpl(cv::Mat &image, const cv::Scalar &color, int thickness, DrawMask type) const
{
    // 限制线条粗细
    thickness = limitThickness(thickness);

    // 绘制边框
    if (type & DrawBorder)
    {
        if(this->getImageCache().isSetCorners())
        {
            const auto &corners = this->getImageCache().getCorners();
            drawQuadrilateralSide(image, corners, color, thickness);
        }
    }

    // 绘制角点
    if (type & DrawCorners)
    {
        if(this->getImageCache().isSetCorners())
        {
            const auto &corners = this->getImageCache().getCorners();
            drawCorners(image, corners, color, thickness);
        }
    }

    // 绘制轮廓线
    if (type & DrawContour)
    {
        if(this->getImageCache().isSetContours())
        {
            const auto &contours = this->getImageCache().getContours();
            drawContours(image, contours,-1, color, thickness);
        }
    }

    // 标注角点的序号
    if (type & DrawCornerLabels)
    {
        if(this->getImageCache().isSetCorners())
        {
            const auto &corners = this->getImageCache().getCorners();
            labelCorners(image, corners, color, thickness);
        }
    }
}

void QuadrilateralBase::drawFeature(cv::Mat &image, const cv::Scalar &color, int thickness, DrawMask type) const
{
    drawFeatureImpl(image, color, thickness, type);
}
