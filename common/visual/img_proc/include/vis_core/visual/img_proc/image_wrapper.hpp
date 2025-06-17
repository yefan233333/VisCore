#pragma once

#include<opencv2/opencv.hpp>
#include<vector>
#include<unordered_map>
#include<string>

#include"vis_core/core/logging/logging.h"
#include "vis_core/visual/contour_proc/contour_proc.h"

/**
 * @class ImageWrapper
 * @brief 图像包装器
 */
class ImageWrapper
{
public:
    // ---------------[ 类型定义区 ]-------------------------
    using Ptr = std::shared_ptr<ImageWrapper>; //!< 图像包装器智能指针类型
    using ProcImgKey = std::string; //!< 处理图像映射的键类型
    using ContourGroupKey = std::string; //!< 轮廓组映射的键类型
    using ContourGroup = std::vector<Contour_ptr>; //!< 轮廓组类型，存储多个轮廓指针


public:
    // 禁用常规构造和拷贝构造
    ImageWrapper() = delete;
    ImageWrapper(const ImageWrapper&) = delete;
    ImageWrapper& operator=(const ImageWrapper&) = delete;

    /**
     * @brief 构造函数
     * @param[in] source_img 源图像
     */
    ImageWrapper(const cv::Mat& source_img)
        : __source_image(source_img.clone())
    {
        if (__source_image.empty())
        {   
            VISCORE_THROW_ERROR("源图像不能为空");
        }        
    }

    /**
     * @brief 移动构造函数
     * @param[in,out] source_img 源图像
     */
    ImageWrapper(cv::Mat&& source_img)
        : __source_image(std::move(source_img))
    {
        if (__source_image.empty())
        {   
            VISCORE_THROW_ERROR("源图像不能为空");
        }        
    }

public:
    // -----------------[ 接口区 ]-------------------------

    /**
     * @brief 构造接口
     *
     * @param[in] source_img 源图像
     */
    static Ptr create(const cv::Mat &source_img)
    {
        return std::make_shared<ImageWrapper>(source_img);
    }

    /**
     * @brief 构造接口（移动构造）
     *
     * @param[in] source_img 源图像
     */
    static Ptr create(cv::Mat &&source_img)
    {
        return std::make_shared<ImageWrapper>(std::move(source_img));
    }

    /**
     * @brief 获取源图像
     */
    const cv::Mat& img() const
    {
        return getSourceImageImpl();
    }
    /**
     * @brief 获取源图像（非const版本）
     */
    cv::Mat& img()
    {
        return getSourceImageImpl();
    }

    /**
     * @brief 判断图像是否存在
     * @param[in] key 处理图像的键
     * @return true 存在
     */
    bool hasImg(const ProcImgKey &key) const
    {
        return __processed_image_map.find(key) != __processed_image_map.end();
    }

    /**
     * @brief 根据键获取图像
     * 
     * @param[in] key 处理图像的键
     */
    cv::Mat& getImg(const ProcImgKey &key)
    {
        return getProcessedImageImpl(key);
    }

    /**
     * @brief 获取处理过的图像
     * 
     * @param[in] key 处理图像的键
     */
    const cv::Mat& getImg(const ProcImgKey &key) const
    {
        return getProcessedImageImpl(key);
    }

    /**
     * @brief 设置处理图像
     * 
     * @param[in] key 处理图像的键
     * @param[in] image 处理图像
     */
    void setImg(const ProcImgKey &key, const cv::Mat &image)
    {
        setProcessedImageImpl(key, image);
    }


    /**
     * @brief 设置处理图像（移动版本）
     * 
     * @param[in] key 处理图像的键
     * @param[in] image 处理图像
     */
    void setImg(const ProcImgKey &key, cv::Mat &&image)
    {
        setProcessedImageImpl(key, std::move(image));
    }

    /**
     * @brief 获取轮廓组
     * 
     * @param[in] key 轮廓组的键
     */
    const ContourGroup& contour_group(const ContourGroupKey &key) const
    {
        return getContourGroupImpl(key);
    }

    /**
     * @brief 设置轮廓组
     * 
     * @param[in] key 轮廓组的键
     * @param[in] contours 轮廓组
     */
    void setContourGroup(const ContourGroupKey &key, const ContourGroup &contours)
    {
        setContourGroupImpl(key, contours);
    }

    /**
     * @brief 设置轮廓组（移动版本）
     * 
     * @param[in] key 轮廓组的键
     * @param[in] contours 轮廓组
     */
    void setContourGroup(const ContourGroupKey &key, ContourGroup &&contours)
    {
        setContourGroupImpl(key, std::move(contours));
    }

private:
    //------------------[ 处理实现区 ]-------------------------
    
    /**
     * @brief 获取源图像
     */
    const cv::Mat& getSourceImageImpl() const
    {
        return __source_image;
    }

    /**
     * @brief 获取源图像（非const版本）
     */
    cv::Mat& getSourceImageImpl()
    {
        return __source_image;
    }

    /**
     * @brief 获取处理过的图像
     * 
     * @param[in] key 处理图像的键
     * 
     * @note 如果图像不存在，则抛出异常
     */
    cv::Mat& getProcessedImageImpl(const ProcImgKey &key)
    {
        auto it = __processed_image_map.find(key);
        if (it != __processed_image_map.end())
        {
            return it->second;
        }
        else
        {
            VISCORE_THROW_ERROR("处理图像不存在，键：%s", key.c_str());
        }
    }


    /**
     * @brief 通过键获取处理过的图像
     * 
     * @param[in] key 处理图像的键
     */
    const cv::Mat& getProcessedImageImpl(const ProcImgKey &key) const
    {
        auto it = __processed_image_map.find(key);
        if (it != __processed_image_map.end())
        {
            return it->second;
        }
        else
        {
            VISCORE_THROW_ERROR("处理图像不存在，键：%s", key.c_str());
        }
    }

    /**
     * @brief 设置处理图像
     * 
     * @param[in] key 处理图像的键
     * @param[in] image 处理图像
     * 
     * @note 会将图像的副本存储在内部映射中，建议使用移动的版本
     */
    void setProcessedImageImpl(const ProcImgKey &key, const cv::Mat &image)
    {
        if(image.empty())
        {
            VISCORE_THROW_ERROR("处理图像不能为空，键：%s", key.c_str());
        }
        __processed_image_map[key] = image.clone(); // 确保存储的是图像的副本
    }
    
    /**
     * @brief 设置处理图像（移动版本）
     * 
     * @param[in] key 处理图像的键
     * @param[in] image 处理图像
     */
    void setProcessedImageImpl(const ProcImgKey &key, cv::Mat &&image)
    {
        if(image.empty())
        {
            VISCORE_THROW_ERROR("处理图像不能为空，键：%s", key.c_str());
        }
        __processed_image_map[key] = std::move(image); // 移动存储图像
    }


    /**
     * @brief 获取轮廓组
     * 
     * @param[in] key 轮廓组的键
     */
    const ContourGroup& getContourGroupImpl(const ContourGroupKey &key) const
    {
        auto it = __contour_group_map.find(key);
        if (it != __contour_group_map.end())
        {
            return it->second;
        }
        else
        {
            VISCORE_THROW_ERROR("轮廓组不存在，键：%s", key.c_str());
        }
    }

    /**
     * @brief 设置轮廓组
     * 
     * @param[in] key 轮廓组的键
     * @param[in] contours 轮廓组
     */
    void setContourGroupImpl(const ContourGroupKey &key, const ContourGroup &contours)
    {
        if (contours.empty())
        {
            VISCORE_THROW_ERROR("轮廓组不能为空，键：%s", key.c_str());
        }
        __contour_group_map[key] = contours; // 存储轮廓组
    }

    /**
     * @brief 设置轮廓组（移动版本）
     * 
     * @param[in] key 轮廓组的键
     * @param[in] contours 轮廓组
     */
    void setContourGroupImpl(const ContourGroupKey &key, ContourGroup &&contours)
    {
        if (contours.empty())
        {
            VISCORE_THROW_ERROR("轮廓组不能为空，键：%s", key.c_str());
        }
        __contour_group_map[key] = std::move(contours); // 移动存储轮廓组
    }



private:
    cv::Mat __source_image; //!< 源图像
    std::unordered_map<ProcImgKey, cv::Mat> __processed_image_map;                 //!< 处理过的图像
    std::unordered_map<ContourGroupKey, ContourGroup> __contour_group_map;         //!< 轮廓组
};

using ImageWrapper_ptr = std::shared_ptr<ImageWrapper>; //!< 图像包装器指针类型
using Img_ptr = ImageWrapper_ptr; //!< 图像包装器智能指针类型别名