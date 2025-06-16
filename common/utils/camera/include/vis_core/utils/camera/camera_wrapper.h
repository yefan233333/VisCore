#pragma once


#include <opencv2/core.hpp>
#include <memory>


/**
 * @brief 相机包装类
 */
class CameraWrapper
{
public:
    using Ptr = std::shared_ptr<CameraWrapper>; //!< 智能指针类型
    CameraWrapper() = default;
    virtual ~CameraWrapper() = default;
};

using CameraWrapper_ptr = std::shared_ptr<CameraWrapper>; //!< 相机包装类智能指针类型
using Camera_ptr = CameraWrapper_ptr; //!< 相机包装类智能指针类型别名