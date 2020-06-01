//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#ifndef VPS_IMAGE_THRESHOLD_TO_ZERO_GUARD
#define VPS_IMAGE_THRESHOLD_TO_ZERO_GUARD

#include <array>
#include <vk_mem_alloc.h>

#include "Technique.h"

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

class Image_threshold_to_zero : public Technique {
public:
    Image_threshold_to_zero(Context* context);

    void encode(VkCommandBuffer command_buffer, VkImageView src_image_view, VkImageView dst_image_view);

    void threshold(float threshold);

    void transform(const float* transform);

private:
    std::vector<uint32_t> compile_shader_();

    void update_push_constants_(VkCommandBuffer command_buffer);

    void update_images_(VkCommandBuffer command_buffer,
                        VkImageView src_image_view, VkImageView dst_image_view);

private:
    float threshold_;
    std::array<float, 3> transform_;
    bool dirty_flag_;
    VkImageView src_image_view_;
    VkImageView dst_image_view_;
    VkDescriptorSet set_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps

#endif // VPS_IMAGE_THRESHOLD_TO_ZERO_GUARD