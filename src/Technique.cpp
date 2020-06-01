//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#include <cassert>
#include <sc/Spirv_compiler.h>

#include "vps.h"
#include "config.h"
#include "Context.h"
#include "Set_pool.h"
#include "Technique.h"

using namespace std;
using namespace Sc;

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

Technique::Technique(Context* context, const std::string& label) :
    context_ {context},
    label_ {label},
    signature_ {},
    set_layout_ {VK_NULL_HANDLE},
    set_pool_ {},
    shader_module_ {VK_NULL_HANDLE},
    pipeline_layout_ {VK_NULL_HANDLE},
    pipeline_ {VK_NULL_HANDLE}
{
}

//----------------------------------------------------------------------------------------------------------------------

Technique::~Technique()
{
    deinit_pipeline_();
    deinit_pipeline_layout_();
    deinit_set_layout_();
    deinit_shader_module_();
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::init_signature_(const std::vector<uint32_t>& source)
{
    try {
        signature_ = Spirv_reflector().reflect(source);
    }
    catch (exception& e) {
        throw runtime_error(e.what());
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::init_shader_module_(const std::vector<uint32_t>& source)
{
    VkShaderModuleCreateInfo smci {};

    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = sizeof(uint32_t) * source.size();
    smci.pCode = source.data();

    auto result = vkCreateShaderModule(context_->device(), &smci, nullptr, &shader_module_);

    if (result != VK_SUCCESS) {
        throw runtime_error("fail to create a shader module.");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::deinit_shader_module_()
{
    if (shader_module_) {
        vkDestroyShaderModule(context_->device(), shader_module_, nullptr);
        shader_module_ = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::init_set_layout_()
{
    assert(!signature_.images.empty());

    vector<VkDescriptorSetLayoutBinding> dslbs;

    for (auto& [binding, image] : signature_.images) {
        VkDescriptorSetLayoutBinding dslb {};

        dslb.binding = binding;
        dslb.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        dslb.descriptorCount = 1;
        dslb.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        dslbs.push_back(dslb);
    }

    VkDescriptorSetLayoutCreateInfo dslci {};

    dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dslci.bindingCount = static_cast<uint32_t>(dslbs.size());
    dslci.pBindings = dslbs.data();

    auto result = vkCreateDescriptorSetLayout(context_->device(),
                                              &dslci, nullptr, &set_layout_);

    if (result != VK_SUCCESS) {
        throw runtime_error("fail to create a descriptor set layout.");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::deinit_set_layout_()
{
    if (set_layout_) {
        vkDestroyDescriptorSetLayout(context_->device(), set_layout_, nullptr);
        set_layout_ = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::init_set_pool_()
{
    VkDescriptorPoolSize pool_size {};

    pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    pool_size.descriptorCount = signature_.images.size() * max_inflight_resource;

    set_pool_ = make_unique<Set_pool>(context_, pool_size, set_layout_);
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::init_pipeline_layout_()
{
    VkPushConstantRange pcr {};

    pcr.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pcr.offset = 0;
    pcr.size = context_->physical_device_properties().limits.maxPushConstantsSize;

    VkPipelineLayoutCreateInfo plci {};

    plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plci.setLayoutCount = 1;
    plci.pSetLayouts = &set_layout_;
    plci.pushConstantRangeCount = 1;
    plci.pPushConstantRanges = &pcr;

    auto result = vkCreatePipelineLayout(context_->device(), &plci, nullptr, &pipeline_layout_);

    if (result != VK_SUCCESS) {
        throw runtime_error("fail to create a pipeline layout.");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::deinit_pipeline_layout_()
{
    if (pipeline_layout_) {
        vkDestroyPipelineLayout(context_->device(), pipeline_layout_, nullptr);
        pipeline_layout_ = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::init_pipeline_()
{
    VkComputePipelineCreateInfo cpci {};

    cpci.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    cpci.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    cpci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cpci.stage.module = shader_module_;
    cpci.stage.pName = "main";
    cpci.layout = pipeline_layout_;

    auto result = vkCreateComputePipelines(context_->device(), VK_NULL_HANDLE,
                                           1, &cpci, nullptr, &pipeline_);

    if (result != VK_SUCCESS) {
        throw runtime_error("fail to create a pipeline.");
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Technique::deinit_pipeline_()
{
    if (pipeline_) {
        vkDestroyPipeline(context_->device(), pipeline_, nullptr);
        pipeline_ = VK_NULL_HANDLE;
    }
}

//----------------------------------------------------------------------------------------------------------------------

VkDescriptorSet Technique::acquire_set_()
{
    return set_pool_->acquire();
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps