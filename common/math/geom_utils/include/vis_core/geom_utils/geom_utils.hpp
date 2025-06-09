#pragma once

#include <opencv2/core.hpp>

#include "vis_core/geom_utils/type_utils.hpp"
#include "vis_core/logging/logging.h"

//! 角度制
enum AngleMode : bool
{
    RAD = true,
    DEG = false
};

// -------------------- 角度规范化函数 (取代宏) --------------------
template <typename T>
inline T NormalizeDegree(T degrees)
{
    while (degrees > 180)
        degrees -= 360;
    while (degrees <= -180)
        degrees += 360;
    return degrees;
}

template <typename T>
inline T NormalizeRadian(T radians)
{
    while (radians > CV_PI)
        radians -= 2 * CV_PI;
    while (radians <= -CV_PI)
        radians += 2 * CV_PI;
    return radians;
}

// ------------------------【常用变换公式】------------------------

/**
 * @brief 角度转换为弧度
 *
 * @tparam Tp 变量类型
 * @param[in] deg 角度
 * @return 弧度
 */
template <typename Tp>
inline Tp deg2rad(Tp deg)
{
    return deg * static_cast<Tp>(CV_PI) / static_cast<Tp>(180);
}

/**
 * @brief 弧度转换为角度
 *
 * @tparam Tp 变量类型
 * @param[in] rad 弧度
 * @return 角度
 */
template <typename Tp>
inline Tp rad2deg(Tp rad) { return rad * static_cast<Tp>(180) / static_cast<Tp>(CV_PI); }

/**
 * @brief Point类型转换为Matx类型
 *
 * @tparam Tp 数据类型
 * @param[in] point Point类型变量
 * @return Matx类型变量
 */
template <typename Tp>
inline cv::Matx<Tp, 3, 1> point2matx(cv::Point3_<Tp> point) { return cv::Matx<Tp, 3, 1>(point.x, point.y, point.z); }

/**
 * @brief Matx类型转换为Point类型
 *
 * @tparam Tp 数据类型
 * @param[in] matx Matx类型变量
 * @return Point类型变量
 */
template <typename Tp>
inline cv::Point3_<Tp> matx2point(cv::Matx<Tp, 3, 1> matx) { return cv::Point3_<Tp>(matx(0), matx(1), matx(2)); }

/**
 * @brief Matx类型转换为Vec类型
 *
 * @tparam Tp 数据类型
 * @param[in] matx Matx类型变量
 * @return Vec类型变量
 */
template <typename Tp>
inline cv::Vec<Tp, 3> matx2vec(cv::Matx<Tp, 3, 1> matx) { return cv::Vec<Tp, 3>(matx(0), matx(1), matx(2)); }

// ------------------------【几何距离计算】------------------------

/**
 * @brief 计算二维向量之间的欧氏距离
 *
 * @param v1 第一个向量
 * @param v2 第二个向量
 * @return 两个向量之间的距离
 */
template <geom_utils_concepts::vector_2_type Vec1,
          geom_utils_concepts::vector_2_type Vec2>
inline auto calcDist(const Vec1 &v1, const Vec2 &v2) noexcept
{
    using namespace geom_utils_concepts;
    const auto dx = get_x(v1) - get_x(v2);
    const auto dy = get_y(v1) - get_y(v2);
    return std::sqrt(dx * dx + dy * dy);
}

/**
 * @brief 计算三维向量之间的欧氏距离
 * 
 * @param v1 第一个三维向量
 * @param v2 第二个三维向量
 * @return 两个三维向量之间的距离
 */
template <geom_utils_concepts::vector_3_type Vec1,
          geom_utils_concepts::vector_3_type Vec2>
inline auto calcDist(const Vec1 &v1, const Vec2 &v2) noexcept
{
    using namespace geom_utils_concepts;
    const auto dx = get_x(v1) - get_x(v2);
    const auto dy = get_y(v1) - get_y(v2);
    const auto dz = get_z(v1) - get_z(v2);
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}