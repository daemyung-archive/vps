//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#ifndef VPS_VPS_GUARD
#define VPS_VPS_GUARD

#include <vulkan/vulkan.h>

//----------------------------------------------------------------------------------------------------------------------

using VpsContext = void*;

//----------------------------------------------------------------------------------------------------------------------

typedef struct VpsContextCreateInfo {
    VkInstance                         instance;
    VkPhysicalDevice                   physicalDevice;
    VkDevice                           device;
} VpsContextCreateInfo;

//----------------------------------------------------------------------------------------------------------------------

VKAPI_ATTR VkResult VKAPI_CALL vpsCreateContext(
    const VpsContextCreateInfo*                 pCreateInfo,
    VpsContext*                                 pContext);

//----------------------------------------------------------------------------------------------------------------------

extern void vpsDestoryContext(
    VpsContext                                  pContext);

//----------------------------------------------------------------------------------------------------------------------

extern void vpsCmdImageThresholdToZero(
    VpsContext                                  context,
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 srcImageView,
    VkImageView                                 dstImageView,
    float                                       threshold,
    const float*                                transform);

//----------------------------------------------------------------------------------------------------------------------

#endif // VPS_VPS_GUARD