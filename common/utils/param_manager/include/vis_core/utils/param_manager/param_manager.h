#pragma once

#include "yml_manager.hpp"


/**
 * @brief 参数管理器——初始化
 */
#define PARAM_MANAGER_INIT(...) YML_INIT(__VA_ARGS__)

/**
 * @brief 参数管理器——添加参数
 */
#define PARAM_MANAGER_ADD_PARAM(...) YML_ADD_PARAM(__VA_ARGS__)