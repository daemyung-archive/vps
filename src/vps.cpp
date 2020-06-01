//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#include <cassert>
#include <exception>

#include "vps.h"
#include "Context.h"
#include "Image_threshold_to_zero.h"

using namespace std;
using namespace Vps;

//----------------------------------------------------------------------------------------------------------------------

VkResult vpsCreateContext(
    const VpsContextCreateInfo*                 pCreateInfo,
    VpsContext*                                 pContext)
{
    *pContext = static_cast<VpsContext>(new Context(pCreateInfo));

    if (*pContext == nullptr) {
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    return VK_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------

void vpsDestoryContext(
    VpsContext                                  pContext)
{
    delete reinterpret_cast<Context*>(pContext);
}

//----------------------------------------------------------------------------------------------------------------------

void vpsCmdImageThresholdToZero(
    VpsContext                                  context,
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 srcImageView,
    VkImageView                                 dstImageView,
    float                                       threshold,
    const float*                                transform)
{
    auto image_threshold_to_zero = static_cast<Context*>(context)->image_threshold_to_zero();
    assert(image_threshold_to_zero);

    image_threshold_to_zero->threshold(threshold);

    if (transform) {
        image_threshold_to_zero->transform(transform);
    }

    try {
        image_threshold_to_zero->encode(commandBuffer, srcImageView, dstImageView);
    }
    catch (exception& e) {
        assert(false);
    }
}

//----------------------------------------------------------------------------------------------------------------------
