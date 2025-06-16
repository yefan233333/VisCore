#pragma once

#include "feature_node.h"

/**
 * @brief 四边形类
 *
 * @note - 该类继承自 FeatureNode，表示一个四边形特征节点的基类。
 *       具体的四边形特征节点可以通过继承此类来实现。
 *
 * @param 角点 存放在 `FeatureNode::ImageCache::Corners` 中
 *
 * @param 轮廓组 存放在 `FeatureNode::ImageCache::Contours` 中
 *
 * @param 位姿节点 存放在 `FeatureNode::PoseCache::PoseNodes` 中
 */
class QuadrilateralBase : public FeatureNode
{
    using Ptr = std::shared_ptr<QuadrilateralBase>; //!< 智能指针类型
public:
    virtual ~QuadrilateralBase() = default;
    QuadrilateralBase() = default;

    /**
     * @brief 构造接口
     * 
     * @param corners 四边形的角点
     */
    static Ptr create(const std::vector<cv::Point2f> &corners);

    //=======================【绘制函数相关】========================
    //! 是否绘制四边形边框
    static constexpr DrawMask DrawBorder = 1u << 0; //!< 0x0001
    //! 是否绘制角点
    static constexpr DrawMask DrawCorners = 1u << 1; //!< 0x0002
    //! 是否绘制轮廓线
    static constexpr DrawMask DrawContour = 1u << 2; //!< 0x0004
    //! 是否标注角点的序号
    static constexpr DrawMask DrawCornerLabels = 1u << 3; //!< 0x0008

    /**
     * @brief 绘制四边形特征
     *
     * @param image 要绘制的图像
     * @param color 绘制颜色
     * @param thickness 绘制线条的粗细
     * @param type 绘制类型
     *
     * @note - 该方法用于在图像上绘制四边形特征的可视化表示
     *
     *      - type 可以由 `DrawMask` 组合而成，表示是否绘制边框、角点、轮廓线等，
     *              例如 type = DrawBorder | DrawCorners ,表示绘制边框和角点。
     */
    virtual void drawFeature(cv::Mat &image, const cv::Scalar &color = cv::Scalar(100, 255, 0), int thickness = 2, DrawMask type = 0) const override;

protected:
    /**
     * @brief 绘制四边形特征 (实现)
     *
     * @param image 要绘制的图像
     * @param color 绘制颜色
     * @param thickness 绘制线条的粗细
     * @param type 绘制类型
     *
     * @note - 该方法用于在图像上绘制四边形特征的可视化表示
     *
     *      - type 可以由 `DrawMask` 组合而成，表示是否绘制边框、角点、轮廓线等，
     *              例如 type = DrawBorder | DrawCorners ,表示绘制边框和角点。
     */
    virtual void drawFeatureImpl(cv::Mat &image, const cv::Scalar &color, int thickness, DrawMask type) const;
};
using QuadrilateralBase_ptr = std::shared_ptr<QuadrilateralBase>;
using Quadrilateral_ptr = QuadrilateralBase_ptr; //!< 四边形特征节点指针类型