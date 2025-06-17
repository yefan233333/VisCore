#pragma once


#include <opencv2/core.hpp>
#include <memory>

#include "vis_core/core/type_utils/cv_expansion_type.h"


/**
 * @brief 相机包装类
 */
class CameraWrapper
{
public:
    using Ptr = std::shared_ptr<CameraWrapper>; //!< 智能指针类型
    CameraWrapper() = default;
    virtual ~CameraWrapper() = default;

    /**
     * @brief 构造接口
     * 
     * @param camera_matrix 相机内参矩阵
     * @param dist_coeffs 相机畸变系数
     */
    static auto create(const cv::Matx33f &camera_matrix = cv::Matx33f::eye(), const cv::Matx51f &dist_coeffs = cv::Matx51f::zeros()) -> Ptr
    {
        auto instance = std::make_shared<CameraWrapper>();
        instance->camera_matrix = camera_matrix;
        instance->dist_coeffs = dist_coeffs;
        return instance;
    }

private:
    //! 相机内参矩阵
    cv::Matx33f camera_matrix = cv::Matx33f::eye(); //!< 相机内参矩阵
    //! 相机畸变系数
    cv::Matx51f dist_coeffs = cv::Matx51f::zeros(); //!< 相机畸变系数

};

using CameraWrapper_ptr = std::shared_ptr<CameraWrapper>; //!< 相机包装类智能指针类型
using Camera_ptr = CameraWrapper_ptr; //!< 相机包装类智能指针类型别名