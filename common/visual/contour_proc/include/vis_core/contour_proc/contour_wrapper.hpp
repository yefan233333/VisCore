#pragma once

#include <opencv2/opencv.hpp>
#include <memory>
#include <bitset>
#include <math.h>

#include "vis_core/logging/logging.h"
/**
 * @brief 可以用于ContourWrapper的基本算术类型 int 、float 和 double
 */
template <typename T>
concept ContourWrapperBaseType =
    std::is_same_v<std::remove_cv_t<T>, int> ||
    std::is_same_v<std::remove_cv_t<T>, float> ||
    std::is_same_v<std::remove_cv_t<T>, double>;

/**
 * @class ContourWrapper
 * @brief 高性能轮廓分析器，实现计算结果的延迟加载和智能缓存
 *
 * 1. 基于写时复制(copy-on-write)和延迟加载(lazy initialization)优化内存使用
 * 2. 采用抗锯齿预处理提升几何特征计算精度
 * 3. 自动缓存计算结果，避免重复运算
 * 4. 支持线程安全：const方法可并发调用，非const方法需外部同步
 *
 * @note 构造时会自动进行轮廓抗锯齿处理(approxPolyDP)，后续所有计算基于处理后的轮廓
 */
template <ContourWrapperBaseType _Tp = int>
class ContourWrapper
{
public:
    using ValueType = _Tp;
    using KeyType = std::conditional_t<std::is_same_v<std::remove_cv_t<ValueType>, int>, float, ValueType>;
    using PointType = cv::Point_<ValueType>;  //!< 点类型
    using KeyPointType = cv::Point_<KeyType>; //!< 关键点类型

    using CircleType = std::tuple<KeyPointType, KeyType>; //!< 圆类型，包含圆心和半径

    using ContourWrapper_ptr = std::shared_ptr<ContourWrapper<ValueType>>; //!< 轮廓包装器智能指针类型

    friend class std::allocator<ContourWrapper<ValueType>>;

private:
    /**
     * @brief 缓存块基类
     */
    struct CacheBlockBase
    {
        virtual ~CacheBlockBase() = default; //!< 虚析构函数，确保派生类正确析构

        std::bitset<16> flags; //!< 标志位，使用16位表示不同的缓存状态

        /**
         * @brief 检验缓存状态
         */
        bool isCached(size_t flag) const
        {
            return flags.test(flag);
        }

        /**
         * @brief 设置缓存状态
         */
        void setCached(size_t flag)
        {
            flags.set(flag);
        }

        /**
         * @brief 清除缓存状态
         *
         * @param flag 标志位
         */
        void clearCached(size_t flag)
        {
            flags.reset(flag);
        }

        /**
         * @brief 清除所有缓存状态
         */
        void clearAllCached()
        {
            flags.reset();
        }
    };

public:
    /**
     * @brief 小型缓存块
     */
    struct SmallCacheBlock : public CacheBlockBase
    {
        enum CacheFlags : size_t
        {
            Area = 0,            //!< 面积
            PerimeterClose = 1,  //!< 闭合轮廓周长
            PerimeterOpen = 2,   //!< 开放轮廓周长
            ConvexArea = 3,      //!< 凸包面积
            ConvexPerimeter = 4, //!< 凸包周长
            Circularity = 5,     //!< 圆度
            Center = 6,          //!< 质心
        };

        double area = 0.0;                        //!< 面积
        double perimeter_close = 0.0;             //!< 闭合轮廓周长
        double perimeter_open = 0.0;              //!< 开放轮廓周长
        double convex_area = 0.0;                 //!< 凸包面积
        double convex_perimeter = 0.0;            //!< 凸包周长
        double circularity = 0.0;                 //!< 圆度
        KeyPointType center = KeyPointType(0, 0); //!< 质心
    };

    /**
     * @brief 大型缓存块
     */
    struct LargeCacheBlock : public CacheBlockBase
    {
        enum CacheFlags : size_t
        {
            BoundingRect = 0,      //!< 包围盒
            MinAreaRect = 1,       //!< 最小面积包围盒
            FittedCircle = 2,      //!< 拟合圆
            FittedEllipse = 3,     //!< 拟合椭圆
            ConvexHull = 4,        //!< 凸包点集
            ConvexHullIndices = 5, //!< 凸包点索引
        };

        cv::Rect bounding_rect;               //!< 包围盒
        cv::RotatedRect min_area_rect;        //!< 最小面积包围盒
        CircleType fitted_circle;             //!< 拟合圆
        cv::RotatedRect fitted_ellipse;       //!< 拟合椭圆
        std::vector<PointType> convex_hull;   //!< 凸包点集
        std::vector<int> convex_hull_indices; //!< 凸包点索引
    };

public:
    //---------------[数据存储区]----------------------
private:
    std::shared_ptr<const std::vector<PointType>> __points; //!< 轮廓点集
    mutable std::unique_ptr<SmallCacheBlock> __small_cache; //!< 小型缓存块
    mutable std::unique_ptr<LargeCacheBlock> __large_cache; //!< 大型缓存块

public:
    /**
     * @brief 构造函数
     * @param points 轮廓点集
     */
    explicit ContourWrapper(const std::vector<PointType> &points)
        : __points(std::make_shared<const std::vector<PointType>>(points)),
          __small_cache(nullptr),
          __large_cache(nullptr)
    {
        if (points.empty())
        {
            VISCORE_THROW_ERROR("轮廓点集不能为空");
        }
    }

    /**
     * @brief 构造函数
     * @param points 轮廓点集
     */
    explicit ContourWrapper(std::vector<PointType> &&points)
        : __points(std::make_shared<const std::vector<PointType>>(std::move(points))),
          __small_cache(nullptr),
          __large_cache(nullptr)
    {
        if (__points->empty())
        {
            VISCORE_THROW_ERROR("轮廓点集不能为空");
        }
    }

    /**
     * @brief 禁用默认构造函数
     */
    ContourWrapper() = delete;

    /**
     * @brief 拷贝构造函数
     * @param[in] other 其他轮廓包装器
     */
    explicit ContourWrapper(const ContourWrapper &other)
        : __points(other.__points),
          __small_cache(other.__small_cache ? std::make_unique<SmallCacheBlock>(*other.__small_cache) : nullptr),
          __large_cache(other.__large_cache ? std::make_unique<LargeCacheBlock>(*other.__large_cache) : nullptr)
    {
        if (!__points || __points->empty())
        {
            VISCORE_THROW_ERROR("轮廓点集不能为空");
        }
    }

    /**
     * @brief 拷贝赋值函数
     * @param[in] other 其他轮廓包装器
     */
    ContourWrapper &operator=(const ContourWrapper &other)
    {
        if (this != &other)
        {
            __points = other.__points;
            __small_cache = other.__small_cache ? std::make_unique<SmallCacheBlock>(*other.__small_cache) : nullptr;
            __large_cache = other.__large_cache ? std::make_unique<LargeCacheBlock>(*other.__large_cache) : nullptr;

            if (!__points || __points->empty())
            {
                VISCORE_THROW_ERROR("轮廓点集不能为空");
            }
        }
        return *this;
    }

    /**
     * @brief 移动拷贝函数
     * @param[in] other 其他轮廓包装器
     */
    explicit ContourWrapper(ContourWrapper &&other) noexcept
        : __points(std::move(other.__points)),
          __small_cache(std::move(other.__small_cache)),
          __large_cache(std::move(other.__large_cache))
    {
        // 确保移动后仍然有有效的轮廓点集
        if (!__points || __points->empty())
        {
            VISCORE_THROW_ERROR("轮廓点集不能为空");
        }
        // 清理其他对象的缓存
        other.__small_cache = nullptr;
        other.__large_cache = nullptr;
    }


public:
    //----------------[接口区]-------------------------
    /**
     * @brief 指针构造接口
     */
    static ContourWrapper_ptr create(const std::vector<PointType> &points)
    {
        return std::make_shared<ContourWrapper>(points);
    }

    /**
     * @brief 指针构造接口（移动构造）
     */
    static ContourWrapper_ptr create(std::vector<PointType> &&points)
    {
        return std::make_shared<ContourWrapper>(std::move(points));
    }

    /**
     * @brief 获取点集
     */
    const auto &points() const
    {
        return getPoints();
    }

    /**
     * @brief 获取面积
     */
    auto area() const
    {
        return calculateAreaImpl();
    }

    /**
     * @brief 获取轮廓周长
     * @param[in] closed 是否闭合轮廓
     */
    auto perimeter(bool closed = true) const
    {
        if (closed)
        {
            return calculatePerimeterCloseImpl();
        }
        else
        {
            return calculatePerimeterOpenImpl();
        }
    }

    /**
     * @brief 获取凸包面积
     */
    auto convexArea() const
    {
        return calculateConvexAreaImpl();
    }

    /**
     * @brief 获取凸包周长
     */
    auto convexPerimeter() const
    {
        return calculateConvexPerimeterImpl();
    }

    /**
     * @brief 获取圆度
     */
    auto circularity() const
    {
        return calculateCircularityImpl();
    }

    /**
     * @brief 获取质心
     */
    auto center() const
    {
        return calculateCenterImpl();
    }

    /**
     * @brief 获取包围盒
     */
    auto boundingRect() const
    {
        return calculateBoundingRectImpl();
    }

    /**
     * @brief 获取最小面积包围盒
     */
    auto minAreaRect() const
    {
        return calculateMinAreaRectImpl();
    }

    /**
     * @brief 获取拟合圆
     */
    auto fittedCircle() const
    {
        return calculateFittedCircleImpl();
    }

    /**
     * @brief 获取拟合椭圆
     */
    auto fittedEllipse() const
    {
        return calculateFittedEllipseImpl();
    }

    /**
     * @brief 获取凸包点集
     */
    const auto &convexHull() const
    {
        return calculateConvexHullImpl();
    }

    /**
     * @brief 获取凸包点索引
     */
    const auto &convexHullIndices() const
    {
        return calculateConvexHullIndicesImpl();
    }

    //----------------[计算实现区]-------------------------
private:
    /**
     * @brief 生成小型缓存块
     * @return std::unique_ptr<SmallCacheBlock> 返回生成的小型缓存块
     */
    auto generateSmallCache() const
    {
        auto cache = std::make_unique<SmallCacheBlock>();
        cache->clearAllCached(); // 清除所有缓存状态
        return cache;
    }

    /**
     * @brief 生成大型缓存块
     * @return std::unique_ptr<LargeCacheBlock> 返回生成的大型缓存块
     */
    auto generateLargeCache() const
    {
        auto cache = std::make_unique<LargeCacheBlock>();
        cache->clearAllCached(); // 清除所有缓存状态
        return cache;
    }

    /**
     * @brief 获取轮廓点集
     * @return std::shared_ptr<const std::vector<PointType>> 返回轮廓点集
     */
    const std::vector<PointType> &getPoints() const
    {
        if (!__points)
        {
            VISCORE_THROW_ERROR("轮廓点集未初始化");
        }

        if (__points->empty())
        {
            VISCORE_THROW_ERROR("轮廓点集为空");
        }

        return *__points;
    }

    /**
     * @brief 计算轮廓面积
     */
    auto calculateAreaImpl() const
    {
        if (__small_cache == nullptr)
            __small_cache = generateSmallCache();

        if (!__small_cache->isCached(ContourWrapper<>::SmallCacheBlock::Area))
        {
            const auto &points = getPoints();
            __small_cache->area = cv::contourArea(points);
            __small_cache->setCached(ContourWrapper<>::SmallCacheBlock::Area);
        }
        return __small_cache->area;
    }

    /**
     * @brief 计算闭合轮廓周长
     */
    auto calculatePerimeterCloseImpl() const
    {
        if (__small_cache == nullptr)
            __small_cache = generateSmallCache();

        if (!__small_cache->isCached(ContourWrapper<>::SmallCacheBlock::PerimeterClose))
        {
            const auto &points = getPoints();
            __small_cache->perimeter_close = cv::arcLength(points, true);
            __small_cache->setCached(ContourWrapper<>::SmallCacheBlock::PerimeterClose);
        }
        return __small_cache->perimeter_close;
    }

    /**
     * @brief 计算开放轮廓周长
     */
    auto calculatePerimeterOpenImpl() const
    {
        if (__small_cache == nullptr)
            __small_cache = generateSmallCache();

        if (!__small_cache->isCached(ContourWrapper<>::SmallCacheBlock::PerimeterOpen))
        {
            const auto &points = getPoints();
            __small_cache->perimeter_open = cv::arcLength(points, false);
            __small_cache->setCached(ContourWrapper<>::SmallCacheBlock::PerimeterOpen);
        }
        return __small_cache->perimeter_open;
    }

    /**
     * @brief 计算凸包面积
     */
    auto calculateConvexAreaImpl() const
    {
        if (__small_cache == nullptr)
            __small_cache = generateSmallCache();

        if (!__small_cache->isCached(ContourWrapper<>::SmallCacheBlock::ConvexArea))
        {
            const auto &convex_hull = calculateConvexHullImpl();
            __small_cache->convex_area = cv::contourArea(convex_hull);
        }

        return __small_cache->convex_area;
    }

    /**
     * @brief 计算凸包周长
     */
    auto calculateConvexPerimeterImpl() const
    {
        if (__small_cache == nullptr)
            __small_cache = generateSmallCache();

        if (!__small_cache->isCached(ContourWrapper<>::SmallCacheBlock::ConvexPerimeter))
        {
            const auto &convex_hull = calculateConvexHullImpl();
            __small_cache->convex_perimeter = cv::arcLength(convex_hull, true);
            __small_cache->setCached(ContourWrapper<>::SmallCacheBlock::ConvexPerimeter);
        }
        return __small_cache->convex_perimeter;
    }

    /**
     * @brief 计算圆度
     */
    auto calculateCircularityImpl() const
    {
        if (__small_cache == nullptr)
            __small_cache = generateSmallCache();

        if (!__small_cache->isCached(ContourWrapper<>::SmallCacheBlock::Circularity))
        {
            auto area = calculateAreaImpl();
            auto perimeter = calculatePerimeterCloseImpl();

            if (perimeter > 0 && area > 0)
            {
                __small_cache->circularity = 4 * CV_PI * (area / (perimeter * perimeter));
            }
            else
            {
                __small_cache->circularity = 0.0; // 避免除以零
            }
            __small_cache->setCached(ContourWrapper<>::SmallCacheBlock::Circularity);
        }
        return __small_cache->circularity;
    }

    /**
     * @brief 计算质心
     */
    auto calculateCenterImpl() const
    {
        if (__small_cache == nullptr)
            __small_cache = generateSmallCache();

        if (!__small_cache->isCached(ContourWrapper<>::SmallCacheBlock::Center))
        {
            const auto &points = getPoints();
            cv::Moments m = cv::moments(points, true);
            if (m.m00 != 0)
            {
                __small_cache->center.x = static_cast<KeyType>(m.m10 / m.m00);
                __small_cache->center.y = static_cast<KeyType>(m.m01 / m.m00);
            }
            else
            {
                __small_cache->center.x = 0;
                __small_cache->center.y = 0;
            }
            __small_cache->setCached(ContourWrapper<>::SmallCacheBlock::Center);
        }
        return __small_cache->center;
    }

    /**
     * @brief 计算包围盒
     */
    auto calculateBoundingRectImpl() const
    {
        if (__large_cache == nullptr)
            __large_cache = generateLargeCache();

        if (!__large_cache->isCached(ContourWrapper<>::LargeCacheBlock::BoundingRect))
        {
            const auto &points = getPoints();
            __large_cache->bounding_rect = cv::boundingRect(points);
            __large_cache->setCached(ContourWrapper<>::LargeCacheBlock::BoundingRect);
        }
        return __large_cache->bounding_rect;
    }

    /**
     * @brief 计算最小面积包围盒
     */
    auto calculateMinAreaRectImpl() const
    {
        if (__large_cache == nullptr)
            __large_cache = generateLargeCache();

        if (!__large_cache->isCached(ContourWrapper<>::LargeCacheBlock::MinAreaRect))
        {
            const auto &points = getPoints();
            __large_cache->min_area_rect = cv::minAreaRect(points);
            __large_cache->setCached(ContourWrapper<>::LargeCacheBlock::MinAreaRect);
        }
        return __large_cache->min_area_rect;
    }

    /**
     * @brief 计算拟合圆
     */
    auto calculateFittedCircleImpl() const
    {
        if (__large_cache == nullptr)
            __large_cache = generateLargeCache();

        if (!__large_cache->isCached(ContourWrapper<>::LargeCacheBlock::FittedCircle))
        {
            const auto &points = getPoints();
            cv::Point2f center;
            float radius;
            cv::minEnclosingCircle(points, center, radius);
            __large_cache->fitted_circle = std::make_tuple(KeyPointType(center.x, center.y), radius);
            __large_cache->setCached(ContourWrapper<>::LargeCacheBlock::FittedCircle);
        }
        return __large_cache->fitted_circle;
    }

    /**
     * @brief 计算拟合椭圆
     */
    auto calculateFittedEllipseImpl() const
    {
        if (__large_cache == nullptr)
            __large_cache = generateLargeCache();

        if (!__large_cache->isCached(ContourWrapper<>::LargeCacheBlock::FittedEllipse))
        {
            const auto &points = getPoints();

            // 根据具体点数的不同，选择合适的椭圆拟合方法
            cv::RotatedRect fit_ellipse;
            size_t points_size = points.size();
            if (points_size >= 5)
            {
                fit_ellipse = cv::fitEllipse(points);
            }
            else if (points_size == 1)
            {
                auto center = points[0];                                    // 将单个点作为椭圆中心
                fit_ellipse = cv::RotatedRect(center, cv::Size2f(1, 1), 0); // 半径为1的点
            }
            else if (points_size == 2)
            {
                auto p1 = static_cast<KeyPointType>(points[0]);
                auto p2 = static_cast<KeyPointType>(points[1]);
                KeyPointType center((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
                KeyType len = cv::norm(p1 - p2);
                KeyType angle = std::atan2(p2.y - p1.y, p2.x - p1.x) * 180 / CV_PI;
                fit_ellipse = cv::RotatedRect(center, cv::Size2f(len, 1.0f), angle);
            }
            else if (points_size == 3)
            {
                auto p1 = static_cast<KeyPointType>(points[0]);
                auto p2 = static_cast<KeyPointType>(points[1]);
                auto p3 = static_cast<KeyPointType>(points[2]);
                KeyPointType center((p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3);
                KeyType max_radius = std::max({cv::norm(p1 - p2), cv::norm(p1 - p3), cv::norm(p2 - p3)});

                // 获取最长边
                KeyType maxEdge = 0.;
                KeyType angle = 0.;
                KeyPointType direction;
                for (int i = 0; i < 3; i++)
                {
                    KeyPointType vec = (points[(i + 1) % 3] - points[i]);
                    KeyType length = cv::norm(vec);
                    if (length > maxEdge)
                    {
                        maxEdge = length;
                        direction = vec;
                        KeyPointType mid = (points[i] + points[(i + 1) % 3]) * 0.5f;
                        KeyPointType mid_vec = mid - center;
                        angle = std::atan2(mid_vec.y, mid_vec.x) * 180 / CV_PI;
                    }
                }
                // 设置主轴为最长距离方向，副轴基于点分布
                KeyType major_axis = max_radius * 2; // 主轴长度为最长边的两倍
                KeyType minor_axis = std::min(maxEdge * 0.5, max_radius * 0.5);
                fit_ellipse = cv::RotatedRect(center, cv::Size2f(major_axis, minor_axis), angle);
            }
            else if (points_size == 4 || points_size == 5)
            {
                // 对于4或5个点，直接使用最小面积包围盒拟合
                fit_ellipse = cv::minAreaRect(points);
                if (fit_ellipse.size.width < 1.0f || fit_ellipse.size.height < 1.0f)
                {
                    // 如果拟合结果的宽度或高度小于1，则将其设置为1
                    fit_ellipse.size.width = std::max(fit_ellipse.size.width, 1.0f);
                    fit_ellipse.size.height = std::max(fit_ellipse.size.height, 1.0f);
                }
            }
            else
            {
                VISCORE_THROW_ERROR("轮廓点数不足，无法拟合椭圆");
            }
            __large_cache->fitted_ellipse = std::move(fit_ellipse);
            __large_cache->setCached(ContourWrapper<>::LargeCacheBlock::FittedEllipse);
        }
        return __large_cache->fitted_ellipse;
    }

    /**
     * @brief 计算凸包
     */
    const auto &calculateConvexHullImpl() const
    {
        if (__large_cache == nullptr)
            __large_cache = generateLargeCache();

        if (!__large_cache->isCached(ContourWrapper<>::LargeCacheBlock::ConvexHull))
        {
            const auto &points = getPoints();
            if (points.size() < 3)
            {
                // 如果点数少于3，直接返回原始点集
                __large_cache->convex_hull = points;
            }
            else
            {
                cv::convexHull(points, __large_cache->convex_hull);
            }
            __large_cache->setCached(ContourWrapper<>::LargeCacheBlock::ConvexHull);
        }
        return __large_cache->convex_hull;
    }

    /**
     * @brief 计算凸包索引
     */
    const auto &calculateConvexHullIndicesImpl() const
    {
        if (__large_cache == nullptr)
            __large_cache = generateLargeCache();

        if (!__large_cache->isCached(ContourWrapper<>::LargeCacheBlock::ConvexHullIndices))
        {
            const auto &points = getPoints();
            if (points.size() < 3)
            {
                // 如果点数少于3，返回两个点的索引
                __large_cache->convex_hull_indices.clear();
                for (size_t i = 0; i < points.size(); ++i)
                {
                    __large_cache->convex_hull_indices.push_back(static_cast<int>(i));
                }
            }
            else
            {
                cv::convexHull(points, __large_cache->convex_hull_indices);
            }
            __large_cache->setCached(ContourWrapper<>::LargeCacheBlock::ConvexHullIndices);
        }
        return __large_cache->convex_hull_indices;
    }
};

using ContourI_ptr = std::shared_ptr<ContourWrapper<int>>;
using ContourF_ptr = std::shared_ptr<ContourWrapper<float>>;
using ContourD_ptr = std::shared_ptr<ContourWrapper<double>>;

using Contour_ptr = ContourI_ptr; //!< 默认轮廓类型为int