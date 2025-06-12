#pragma once

#include <opencv2/core.hpp>
#include <memory>

#include "vis_core/geom_utils/type_utils.hpp"

class Transform6D
{
public:
    /**
     * @brief 默认构造函数
     */
    Transform6D();

    /**
     * @brief 构造函数
     * @param tvec 平移向量
     * @param rmat 旋转矩阵
     */
    Transform6D(const cv::Vec3d& tvec, const cv::Matx33d& rmat);

    /**
     * @brief 移动构造函数
     * @param tvec 平移向量
     * @param rmat 旋转矩阵
     */
    Transform6D(Transform6D&& other) noexcept;


private:
    cv::Matx31d __tvec; //!< 平移向量
    cv::Matx33d __rmat; //!< 旋转矩阵
};