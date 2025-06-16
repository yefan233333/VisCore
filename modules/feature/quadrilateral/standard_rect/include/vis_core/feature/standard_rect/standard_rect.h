#pragma once

#include "vis_core/visual/feature_node/quadrilateral.h"


/**
 * @brief 标准矩形特征节点
 * 
 */
class StandardRect : public QuadrilateralBase
{
public:
    using Ptr = std::shared_ptr<StandardRect>; //!< 智能指针类型
    StandardRect() = default;
    virtual ~StandardRect() = default;

};