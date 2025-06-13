#pragma once

#include <opencv2/core.hpp>
#include <memory>

#include "vis_core/geom_utils/type_utils.hpp"

//! Transform6D 的传入参数类型概念
namespace transform6D_concepts
{
    /**
     * @brief 去除常量和引用限定的旋转矩阵类型概念
     */
    template <typename T>
    using base_type = std::remove_cvref_t<T>;

    /**
     * @brief 旋转矩阵类型概念
     * @tparam T 旋转矩阵类型
     * 
     * @note 要求 T 为 cv::Matx<Tp, 3, 3> 
     */
    template <typename T>
    concept rmat_type = requires(T a) {
        requires std::is_same_v<base_type<T>, cv::Matx<typename T::value_type, 3, 3>>;    // 要求 T 的 value_type 为三维向量类型
        requires geom_utils_concepts::vector_arithmetic<typename T::value_type>; // 要求 value_type 是算术类型
    };

    /**
     * @brief 平移向量类型概念
     * @tparam T 平移向量类型
     */
    template <typename T>
    concept tvec_type = geom_utils_concepts::vector_3_type<T>;
}

//! Transform6D 类的辅助函数
namespace transform6D_utils
{
    /**
     * @brief 将旋转矩阵转换为 Transform6D 所需的旋转矩阵类型
     * @tparam T 旋转矩阵类型
     * @param rmat 输入的旋转矩阵
     * @return 返回转换后的旋转矩阵
     */
    template <transform6D_concepts::rmat_type T>
    cv::Matx33d convertRMat(const T &rmat);

    /**
     * @brief 将平移向量转换为 Transform6D 所需的平移向量类型
     * @tparam T 平移向量类型
     * @param tvec 输入的平移向量
     * @return 返回转换后的平移向量
     */
    template <transform6D_concepts::tvec_type T>
    cv::Matx31d convertTVec(const T &tvec);
}

class Transform6D
{
public:
    //! 旋转矩阵存储类型
    using RmatType = cv::Matx33d;
    //! 平移向量存储类型
    using TvecType = cv::Matx31d;

    /**
     * @brief 默认构造函数
     */
    Transform6D();

    /**
     * @brief 构造函数
     * @param rmat 旋转矩阵
     * @param tvec 平移向量
     */
    template<transform6D_concepts::rmat_type T, transform6D_concepts::tvec_type U>
    Transform6D(const T &rmat, const U &tvec);


private:
    RmatType __rmat;  //!< 旋转矩阵
    TvecType __tvec;  //!< 平移向量

};




template <transform6D_concepts::rmat_type T, transform6D_concepts::tvec_type U>
Transform6D::Transform6D(const T &rmat, const U &tvec)
    : __rmat(transform6D_utils::convertRMat(rmat)), __tvec(transform6D_utils::convertTVec(tvec))
{
}

Transform6D::Transform6D()
    : __rmat(RmatType::eye()), __tvec(TvecType::zeros())
{
}

namespace transform6D_utils
{
    // ---------------【转换函数实现】----------------
    template <transform6D_concepts::rmat_type T>
    inline Transform6D::RmatType convertRMat(const T &rmat)
    {
        return static_cast<Transform6D::RmatType>(rmat);
    }

    template <transform6D_concepts::tvec_type T>
    inline Transform6D::TvecType convertTVec(const T &tvec)
    {
        return geom_utils_concepts::cvtMatx3<Transform6D::TvecType::value_type>(tvec);
    }
}