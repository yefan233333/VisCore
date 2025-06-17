#pragma once

#include "vis_core/visual/feature_node/quadrilateral.h"

class StandardRectDetector;

/**
 * @brief 标准矩形类
 */
class StandardRect : public QuadrilateralBase
{
public:
    using Ptr = std::shared_ptr<StandardRect>; //!< 智能指针类型
    StandardRect() = default;
    virtual ~StandardRect() = default;

    /**
     * @brief 获取识别器
     */
    static auto getDetector() -> std::shared_ptr<StandardRectDetector>;
};
using StandardRect_ptr = std::shared_ptr<StandardRect>; //!< 标准矩形智能指针类型

#include "standard_rect_detector.h"