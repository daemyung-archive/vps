//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#ifndef VPS_SET_POOL_GUARD
#define VPS_SET_POOL_GUARD

#include <array>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <sc/Spirv_reflector.h>

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

class Context;

//----------------------------------------------------------------------------------------------------------------------

class Set_pool {
public:
    Set_pool(Context* context,
             const VkDescriptorPoolSize& pool_size, VkDescriptorSetLayout set_layout);

    ~Set_pool();

    VkDescriptorSet acquire();

private:
    void init_set_pool_(VkDescriptorPoolSize pool_size);

    void deinit_set_pool_();

    void init_sets_(VkDescriptorSetLayout set_layout);

private:
    Context* context_;
    VkDescriptorPool set_pool_;
    std::vector<VkDescriptorSet> sets_;
    uint32_t set_index_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps

#endif // VPS_SET_POOL_GUARD