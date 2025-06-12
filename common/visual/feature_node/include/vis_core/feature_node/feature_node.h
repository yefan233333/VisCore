#pragma once

#include <opencv2/core.hpp>
#include <memory>
#include "vis_core/contourproc/contour_wrapper.hpp"
#include "vis_core/imgproc/image_wrapper.hpp"

/**
 * @brief 特征节点
 * 
 * @note 用于承载视觉识别的特征节点
 */
class FeatureNode
{
    using Ptr = std::shared_ptr<FeatureNode>;
private:
    /**
     * @brief 图像信息缓存块
     */
    struct ImageCache
    {
        //! 来源图像
        ImgPtr source_image;
        //! 轮廓组
        std::vector<Contour_ptr> contours;
    }; 

    /**
     * @brief 位姿信息缓存块
     */
    struct PoseCache
    {

    };

public:
};

using FeatureNodePtr = std::shared_ptr<FeatureNode>;