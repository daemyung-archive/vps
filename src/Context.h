//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#ifndef VPS_CONTEXT_GUARD
#define VPS_CONTEXT_GUARD

#include <memory>
#include <vulkan/vulkan.h>

struct VpsContextCreateInfo;

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

class Image_threshold_to_zero;

class Context {
public:
    Context(const VpsContextCreateInfo* create_info);

    inline auto instance() const
    { return instance_; }

    inline auto physical_device() const
    { return physical_device_; }

    inline auto device() const
    { return device_; }

    inline auto physical_device_properties() const
    { return physical_device_properties_; }

    Image_threshold_to_zero* image_threshold_to_zero();

private:
    VkInstance instance_;
    VkPhysicalDevice physical_device_;
    VkDevice device_;
    VkPhysicalDeviceProperties physical_device_properties_;
    std::unique_ptr<Image_threshold_to_zero> image_threshold_to_zero_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps

#endif // VPS_CONTEXT_GUARD