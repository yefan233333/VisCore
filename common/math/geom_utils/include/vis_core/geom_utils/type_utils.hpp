#pragma once

#include <type_traits>
#include <opencv2/core.hpp>

namespace geom_utils_concepts
{

    /**
     * @brief 去除const限定和引用限定
     */
    template<typename T>
    using base_type = std::remove_cvref_t<T>;

    /**
     * @brief 常用算术类型概念
     */
    template <typename T>
    concept vctor_arithmetic = std::is_arithmetic_v<base_type<T>> && (std::is_same_v<base_type<T>, int> ||
                                                                         std::is_same_v<base_type<T>, float> ||
                                                                         std::is_same_v<base_type<T>, double>);

    /**
     * @brief 二维向量类型概念_1
     */
    template <typename T>
    concept vector_2_type_sub_1 = requires(T a) {
        a.x;
        a.y;
        requires(vctor_arithmetic<decltype(a.x)>);
        requires(vctor_arithmetic<decltype(a.y)>);
    } && !requires(T a) {
        a.z; // 确保没有 z 成员
    };

    /**
     * @brief 二维向量类型概念_2
     */
    template <typename T>
    concept vector_2_type_sub_2 = requires(T a) {
        // 可以访问 (0,0)(0,1)
        a(0, 0);
        a(0, 1);
        requires(vctor_arithmetic<decltype(a(0, 0))>);
        requires(vctor_arithmetic<decltype(a(0, 1))>);
        requires(T::rows == 1 && T::cols == 2); // 确保是二维向量
    };

    /**
     * @brief 二维向量类型概念_3
     */
    template <typename T>
    concept vector_2_type_sub_3 = requires(T a) {
        // 可以访问 (0,0)(1,0)
        a(0, 0);
        a(1, 0);
        requires(vctor_arithmetic<decltype(a(0, 0))>);
        requires(vctor_arithmetic<decltype(a(1, 0))>);
        requires(T::rows == 2 && T::cols == 1); // 确保是二维向量
    };

    /**
     * @brief 二维向量类型概念_4
     */
    template <typename T>
    concept vector_2_type_sub_4 = requires(T a) {
        // 为cv::Vec<U, 2>类型,并且U为基本算术类型
        requires(std::is_same_v<T, cv::Vec<typename T::value_type, 2>>);
        requires(vctor_arithmetic<typename T::value_type>);
    };

    /**
     * @brief 二维向量类型概念
     */
    template <typename T>
    concept vector_2_type = vector_2_type_sub_1<T> || vector_2_type_sub_2<T> || vector_2_type_sub_3<T> || vector_2_type_sub_4<T>;

    /**
     * @brief 三维向量类型概念_1
     */
    template <typename T>
    concept vector_3_type_sub_1 = requires(T a) {
        a.x;
        a.y;
        a.z;
        requires vctor_arithmetic<decltype(a.x)>;
        requires vctor_arithmetic<decltype(a.y)>;
        requires vctor_arithmetic<decltype(a.z)>;
    };

    template <typename T>
    concept vector_3_type_sub_2 = requires(T a) {
        a(0, 0);
        a(0, 1);
        a(0, 2);
        requires vctor_arithmetic<decltype(a(0, 0))>;
        requires vctor_arithmetic<decltype(a(0, 1))>;
        requires vctor_arithmetic<decltype(a(0, 2))>;
        requires(T::rows == 1 && T::cols == 3);
    };

    template <typename T>
    concept vector_3_type_sub_3 = requires(T a) {
        a(0, 0);
        a(1, 0);
        a(2, 0);
        requires vctor_arithmetic<decltype(a(0, 0))>;
        requires vctor_arithmetic<decltype(a(1, 0))>;
        requires vctor_arithmetic<decltype(a(2, 0))>;
        requires(T::rows == 3 && T::cols == 1);
    };

    template <typename T>
    concept vector_3_type_sub_4 = requires {
        requires std::is_same_v<T, cv::Vec<typename T::value_type, 3>>;
        requires vctor_arithmetic<typename T::value_type>;
    };

    template <typename T>
    concept vector_3_type = vector_3_type_sub_1<T> || vector_3_type_sub_2<T> || vector_3_type_sub_3<T> || vector_3_type_sub_4<T>;

    // 声明用于静态断言的辅助模板
    namespace details
    {
        template <typename>
        constexpr bool always_false = false;
    }

    /**
     * @brief 提取向量x分量（支持2D/3D向量）
     * @tparam T 向量类型（需满足vector_2_type或vector_3_type）
     * @param v 向量实例
     * @return x分量值
     */
    template <typename T>
        requires(vector_2_type<T> || vector_3_type<T>) // 模板参数约束
    constexpr auto get_x(const T &v) noexcept
    {
        if constexpr (vector_2_type_sub_1<T> || vector_3_type_sub_1<T>)
        {
            return v.x;
        }
        else if constexpr (vector_2_type_sub_2<T>)
        {
            return v(0, 0);
        }
        else if constexpr (vector_2_type_sub_3<T> || vector_3_type_sub_3<T>)
        {
            return v(0, 0);
        }
        else if constexpr (vector_2_type_sub_4<T> || vector_3_type_sub_4<T>)
        {
            return v[0];
        }
        else if constexpr (vector_3_type_sub_2<T>)
        {
            return v(0, 0);
        }
        else
        {
            static_assert(details::always_false<T>, "Unsupported type for get_x");
        }
    }

    /**
     * @brief 提取向量y分量（支持2D/3D向量）
     * @tparam T 向量类型（需满足vector_2_type或vector_3_type）
     * @param v 向量实例
     * @return y分量值
     */
    template <typename T>
        requires(vector_2_type<T> || vector_3_type<T>) // 模板参数约束
    constexpr auto get_y(const T &v) noexcept
    {
        if constexpr (vector_2_type_sub_1<T> || vector_3_type_sub_1<T>)
        {
            return v.y;
        }
        else if constexpr (vector_2_type_sub_2<T>)
        {
            return v(0, 1);
        }
        else if constexpr (vector_2_type_sub_3<T>)
        {
            return v(1, 0);
        }
        else if constexpr (vector_3_type_sub_3<T>)
        {
            return v(1, 0);
        }
        else if constexpr (vector_2_type_sub_4<T> || vector_3_type_sub_4<T>)
        {
            return v[1];
        }
        else if constexpr (vector_3_type_sub_2<T>)
        {
            return v(0, 1);
        }
        else
        {
            static_assert(details::always_false<T>, "Unsupported type for get_y");
        }
    }

    /**
     * @brief 提取向量z分量（仅支持3D向量）
     * @tparam T 向量类型（需满足vector_3_type）
     * @param v 3D向量实例
     * @return z分量值
     */
    template <typename T>
        requires vector_3_type<T> // 模板参数约束
    constexpr auto get_z(const T &v) noexcept
    {
        if constexpr (vector_3_type_sub_1<T>)
        {
            return v.z;
        }
        else if constexpr (vector_3_type_sub_2<T>)
        {
            return v(0, 2);
        }
        else if constexpr (vector_3_type_sub_3<T>)
        {
            return v(2, 0);
        }
        else if constexpr (vector_3_type_sub_4<T>)
        {
            return v[2];
        }
        else
        {
            static_assert(details::always_false<T>,
                          "Unsupported type for get_z (must be 3D vector)");
        }
    }
} // namespace geom_utils_concepts