//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#include "vps.h"
#include "Image_threshold_to_zero.h"
#include "Context.h"

using namespace std;

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

Context::Context(const VpsContextCreateInfo* create_info) :
    instance_ {create_info->instance},
    physical_device_ {create_info->physicalDevice},
    device_ {create_info->device},
    physical_device_properties_ {},
    image_threshold_to_zero_ {}
{
    vkGetPhysicalDeviceProperties(physical_device_, &physical_device_properties_);
}

//----------------------------------------------------------------------------------------------------------------------

Image_threshold_to_zero* Context::image_threshold_to_zero()
{
    if (!image_threshold_to_zero_) {
        image_threshold_to_zero_ = make_unique<Image_threshold_to_zero>(this);
    }

    return image_threshold_to_zero_.get();
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps