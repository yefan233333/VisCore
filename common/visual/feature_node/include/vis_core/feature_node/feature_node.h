#pragma once

#include <opencv2/core.hpp>
#include <memory>
#include "vis_core/property_wrapper/property_wrapper.hpp"
#include "vis_core/contour_proc/contour_wrapper.hpp"
#include "vis_core/img_proc/image_wrapper.hpp"
#include "vis_core/pose_proc/transform6D.hpp"

/**
 * @brief 特征节点
 * 
 * @note 用于承载视觉识别的特征节点
 */
class FeatureNode
{
    using Ptr = std::shared_ptr<FeatureNode>;     //! 指针类型
    using FeatureNodeMap = std::unordered_map<std::string, FeatureNode>;    //! 特征节点映射表类型

public:
    /**
     * @brief 图像信息缓存块
     */
    struct ImageCache
    {
        //! 来源图像
        DEFINE_PROPERTY(SourceImage, public, public, (ImgPtr));
        //! 轮廓组
        DEFINE_PROPERTY(Contours, public, public, (std::vector<Contour_ptr>));
        //! 角点集
        DEFINE_PROPERTY(Corners, public, public, (std::vector<cv::Point2f>));
    }; 

    /**
     * @brief 位姿信息缓存块
     */
    struct PoseCache
    {
        //! 位姿节点映射表类型
        using PoseNodeMap = std::unordered_map<std::string, PoseNode>;
        //! 位姿节点映射
        DEFINE_PROPERTY(PoseNodes, public, public, (PoseNodeMap));
    };
protected:
    //! 图像信息缓存
    DEFINE_PROPERTY_WITH_INIT(ImageCache, public, protected, (ImageCache), ImageCache());
    //! 位姿信息缓存
    DEFINE_PROPERTY_WITH_INIT(PoseCache, public, protected, (PoseCache), PoseCache());
    //! 子特征节点映射表
    DEFINE_PROPERTY_WITH_INIT(ChildFeatures, public, public, (FeatureNodeMap), FeatureNodeMap());
};

using FeatureNodePtr = std::shared_ptr<FeatureNode>;