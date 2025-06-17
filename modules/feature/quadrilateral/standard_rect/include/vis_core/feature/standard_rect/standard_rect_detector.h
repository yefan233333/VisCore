#pragma once

#include "standard_rect.h"
#include "vis_core/visual/img_proc/image_wrapper.hpp"
#include "vis_core/utils/camera/camera_wrapper.h"

/**
 * @brief 标准矩形识别器
 */
class StandardRectDetector 
{
public:
    using Ptr = std::shared_ptr<StandardRectDetector>; //!< 智能指针类型
    StandardRectDetector() = default;
    virtual ~StandardRectDetector() = default;


protected:
    //! 原图像
    DEFINE_PROPERTY(SourceImage, public, protected, (cv::Mat));
    //! 二值化图像
    DEFINE_PROPERTY(BinaryImage, public, protected, (cv::Mat));
    //! 相机信息
    DEFINE_PROPERTY(Camera, public, protected, (Camera_ptr));
public:
    /**
     * @brief 构造接口
     */
    static Ptr create();

    /**
     * @brief 识别标准矩形
     * @param[in] img_ptr 输入图像
     * @param[in] camera_ptr 相机信息（可选）
     * @return std::vector<StandardRect_ptr> 识别到的标准矩形列表
     * 
     * @note - 要求图像包装器中必须包括的 binary 图像，否则使用自带的二值化方法
     */
    auto detect(Img_ptr &img_ptr, const Camera_ptr &camera_ptr) -> std::vector<StandardRect_ptr>;
protected:
    /**
     * @brief 识别标准矩形
     * @param[in] img_ptr 输入图像
     * @return std::vector<StandardRect_ptr> 识别到的标准矩形列表
     */
    auto detectImpl(Img_ptr &img_ptr, const Camera_ptr &camera_ptr) -> std::vector<StandardRect_ptr>;


    /**
     * @brief 二值化图像
     * @param[in,out] img_ptr 输入图像
     * 
     * @note 为 img_ptr 添加 binary 图像和 hsv 图像
     */
    void binarize(Img_ptr &img_ptr);



};
using StandardRectDetector_ptr = std::shared_ptr<StandardRectDetector>; //!< 标准矩形识别器智能指针类型