#pragma once


#include "contour_wrapper.hpp"
#include <array>
/**
 * @brief 增强版轮廓检测函数，返回智能轮廓对象集合
 *
 * @param[in] image 输入图像(二值图，建议使用clone保留原始数据)
 * @param[out] contours 输出轮廓集合(Contour_ptr对象)
 * @param[out] hierarchy 输出轮廓层级信息
 * @param[in] mode 轮廓检索模式
 * @param[in] method 轮廓近似方法
 * @param[in] offset 轮廓点坐标偏移量
 *
 * @note 自动执行抗锯齿处理(根据ENABLE_SMOOTH_CONTOUR_CALC配置)
 */

inline void findContours(cv::InputArray image,
                         std::vector<Contour_ptr> &contours,
                         cv::OutputArray hierarchy,
                         int mode = cv::RETR_TREE,
                         int method = cv::CHAIN_APPROX_NONE,
                         const cv::Point &offset = cv::Point(0, 0))
{
    // 确保输出轮廓集合清空
    contours.clear();

    // 调用OpenCV的findContours函数
    std::vector<std::vector<cv::Point>> cv_contours;
    cv::findContours(image, cv_contours, hierarchy, mode, method, offset);

    // 将OpenCV的轮廓转换为智能指针类型
    for (auto &&cv_contour : cv_contours)
    {
        contours.emplace_back(ContourWrapper<int>::create(std::move(cv_contour)));
    }
}

/**
 * @brief 增强版轮廓检测函数，返回智能轮廓对象集合
 *
 * @param[in] image 输入图像(二值图，建议使用clone保留原始数据)
 * @param[out] contours 输出轮廓集合(ContourWrapper对象)
 * @param[out] hierarchy 输出轮廓层级信息 (用 unordered_map 代替)
 * @param[in] mode 轮廓检索模式
 * @param[in] method 轮廓近似方法
 * @param[in] offset 轮廓点坐标偏移量
 *
 * @note   - hierarchy : std::unordered_map<Contour_ptr, std::tuple<Contour_ptr, Contour_ptr, Contour_ptr, Contour_ptr>>
 * 
 *         - hierarchy : unordered_map < 当前轮廓, std::tuple<后一个轮廓,前一个轮廓,内嵌轮廓,父轮廓>
 */
inline void findContours(cv::InputArray image,
                         std::vector<Contour_ptr> &contours,
                         std::unordered_map<Contour_ptr, std::tuple<Contour_ptr, Contour_ptr, Contour_ptr, Contour_ptr>> &hierarchy,
                         int mode = cv::RETR_TREE,
                         int method = cv::CHAIN_APPROX_NONE,
                         const cv::Point &offset = cv::Point(0, 0))
{
    std::vector<std::vector<cv::Point>> raw_contours;
    std::vector<cv::Vec4i> hierarchy_vec;
    cv::findContours(image, raw_contours, hierarchy_vec, mode, method, offset);
    contours.reserve(raw_contours.size());
    for (auto &&contour : raw_contours)
    {
        contours.emplace_back(ContourWrapper<int>::create(std::move(contour)));
    }
    hierarchy.reserve(raw_contours.size());
    for (size_t i = 0; i < raw_contours.size(); ++i)
    {
        auto &contour = contours[i];
        auto &h = hierarchy_vec[i];
        hierarchy[contour] = std::make_tuple(
            (h[0] != -1) ? contours[h[0]] : nullptr,
            (h[1] != -1) ? contours[h[1]] : nullptr,
            (h[2] != -1) ? contours[h[2]] : nullptr,
            (h[3] != -1) ? contours[h[3]] : nullptr);
    }
}

/**
 * @brief 增强版轮廓检测函数，返回智能轮廓对象集合
 *
 * @param[in] image 输入图像(二值图，建议使用clone保留原始数据)
 * @param[out] contours 输出轮廓集合(ContourWrapper对象)
 * @param[out] hierarchy 输出轮廓层级信息 (用 unordered_map 代替)
 * @param[in] mode 轮廓检索模式
 * @param[in] method 轮廓近似方法
 * @param[in] offset 轮廓点坐标偏移量
 *
 * @note   - hierarchy : std::unordered_map<Contour_ptr,std::array<Contour_ptr, 4>>
 *
 *         - hierarchy : unordered_map < 当前轮廓, std::array<后一个轮廓,前一个轮廓,内嵌轮廓,父轮廓>
 */
inline void findContours(cv::InputArray image,
                         std::vector<Contour_ptr> &contours,
                         std::unordered_map<Contour_ptr, std::array<Contour_ptr, 4>> &hierarchy,
                         int mode = cv::RETR_TREE,
                         int method = cv::CHAIN_APPROX_NONE,
                         const cv::Point &offset = cv::Point(0, 0))
{
    std::vector<std::vector<cv::Point>> raw_contours;
    std::vector<cv::Vec4i> hierarchy_vec;
    cv::findContours(image, raw_contours, hierarchy_vec, mode, method, offset);
    contours.reserve(raw_contours.size());
    for (auto &&contour : raw_contours)
    {
        contours.emplace_back(ContourWrapper<int>::create(std::move(contour)));
    }
    hierarchy.reserve(raw_contours.size());
    for (size_t i = 0; i < raw_contours.size(); ++i)
    {
        auto &contour = contours[i];
        auto &h = hierarchy_vec[i];
        hierarchy[contour] = { 
            (h[0] != -1) ? contours[h[0]] : nullptr,
            (h[1] != -1) ? contours[h[1]] : nullptr,
            (h[2] != -1) ? contours[h[2]] : nullptr,
            (h[3] != -1) ? contours[h[3]] : nullptr
        };
    }
}

/**
 * @brief 增强版轮廓检测函数，返回智能轮廓对象集合
 *
 * @param[in] image 输入图像(二值图，建议使用clone保留原始数据)
 * @param[out] contours 输出轮廓集合(ContourWrapper对象)
 * @param[in] mode 轮廓检索模式
 * @param[in] method 轮廓近似方法
 * @param[in] offset 轮廓点坐标偏移量
 *
 * @note 不输出层级信息
 */
inline void findContours(cv::InputArray image,
                         std::vector<Contour_ptr> &contours,
                         int mode = cv::RETR_TREE,
                         int method = cv::CHAIN_APPROX_NONE,
                         const cv::Point &offset = cv::Point(0, 0))
{
    std::vector<std::vector<cv::Point>> raw_contours;
    cv::findContours(image, raw_contours, cv::RETR_TREE, method, offset);
    contours.reserve(raw_contours.size());
    for (auto &&contour : raw_contours)
    {
        contours.emplace_back(ContourWrapper<int>::create(std::move(contour)));
    }
}                         

