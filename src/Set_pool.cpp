//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#include <cassert>
#include <vector>

#include "config.h"
#include "Context.h"
#include "Set_pool.h"

using namespace std;

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

Set_pool::Set_pool(Context* context,
                   const VkDescriptorPoolSize& pool_size, VkDescriptorSetLayout set_layout) :
    context_ {context},
    set_pool_ {VK_NULL_HANDLE},
    sets_ {},
    set_index_ {0}
{
    init_set_pool_(pool_size);
    init_sets_(set_layout);
}

//----------------------------------------------------------------------------------------------------------------------

Set_pool::~Set_pool()
{
    deinit_set_pool_();
}

//----------------------------------------------------------------------------------------------------------------------

VkDescriptorSet Set_pool::acquire()
{
    auto set = sets_[set_index_];
    set_index_ = ++set_index_ % max_inflight_resource;

    return set;
}

//----------------------------------------------------------------------------------------------------------------------

void Set_pool::init_set_pool_(VkDescriptorPoolSize pool_size)
{
    pool_size.descriptorCount *= max_inflight_resource;

    VkDescriptorPoolCreateInfo dpci {};

    dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dpci.maxSets = max_inflight_resource;
    dpci.poolSizeCount = 1;
    dpci.pPoolSizes = &pool_size;

    auto result = vkCreateDescriptorPool(context_->device(), &dpci, nullptr, &set_pool_);

    if (result != VK_SUCCESS) {
        throw runtime_error("fail to create a descriptor pool.");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Set_pool::deinit_set_pool_()
{
    if (set_pool_) {
        vkDestroyDescriptorPool(context_->device(), set_pool_, nullptr);
        set_pool_ = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Set_pool::init_sets_(VkDescriptorSetLayout set_layout)
{
    sets_.resize(max_inflight_resource);

    for (auto& set : sets_) {
        VkDescriptorSetAllocateInfo dsai {};

        dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        dsai.descriptorPool = set_pool_;
        dsai.descriptorSetCount = 1;
        dsai.pSetLayouts = &set_layout;

        vkAllocateDescriptorSets(context_->device(), &dsai, &set);
    }
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps