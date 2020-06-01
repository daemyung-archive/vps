//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#include <cassert>

#include "vps.h"
#include "Context.h"
#include "Set_pool.h"
#include "Image_threshold_to_zero.h"

#include <iostream>
#include <sc/Spirv_compiler.h>
#include <sc/Spirv_reflector.h>

using namespace std;
using namespace Sc;

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

Image_threshold_to_zero::Image_threshold_to_zero(Context* context) :
    Technique {context, "image threshold to zero"},
    threshold_ {0.5f},
    transform_ {0.299f, 0.587f, 0.114f},
    dirty_flag_ {true}
{
    auto shader = compile_shader_();
    assert(!shader.empty());

    init_signature_(shader);
    init_shader_module_(shader);
    init_set_layout_();
    init_set_pool_();
    init_pipeline_layout_();
    init_pipeline_();
}

//----------------------------------------------------------------------------------------------------------------------

void Image_threshold_to_zero::encode(VkCommandBuffer command_buffer, VkImageView src_image_view, VkImageView dst_image_view)
{
    update_push_constants_(command_buffer);
    update_images_(command_buffer, src_image_view, dst_image_view);

    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout_,
                            0, 1, &set_, 0, nullptr);
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_);
    vkCmdDispatch(command_buffer, 512 / 8, 512 / 8, 1);
}

//----------------------------------------------------------------------------------------------------------------------

void Image_threshold_to_zero::threshold(float threshold)
{
    if (threshold_ == threshold) {
        return;
    }

    threshold_ = threshold;
    dirty_flag_ = true;
}

//----------------------------------------------------------------------------------------------------------------------

void Image_threshold_to_zero::transform(const float* transform)
{
    if (!memcmp(transform_.data(), transform, sizeof(float) * transform_.size())) {
        return;
    }

    for (auto i = 0; i != transform_.size(); ++i) {
        transform_[i] = transform[i];
    }

    dirty_flag_ = true;
}

//----------------------------------------------------------------------------------------------------------------------

std::vector<uint32_t> Image_threshold_to_zero::compile_shader_()
{
    try {
        return Spirv_compiler().compile(VPS_ASSET_PATH"/shader/image_threshold_to_zero.comp");
    }
    catch (exception& e) {
        throw runtime_error(e.what());
    }
}

//----------------------------------------------------------------------------------------------------------------------

void Image_threshold_to_zero::update_push_constants_(VkCommandBuffer command_buffer)
{
    if (!dirty_flag_) {
        return;
    }

    vkCmdPushConstants(command_buffer, pipeline_layout_, VK_SHADER_STAGE_COMPUTE_BIT,
                       0, sizeof(threshold_), &threshold_);
    vkCmdPushConstants(command_buffer, pipeline_layout_, VK_SHADER_STAGE_COMPUTE_BIT,
                       sizeof(threshold_), sizeof(transform_), transform_.data());

    dirty_flag_ = false;
}

//----------------------------------------------------------------------------------------------------------------------

void Image_threshold_to_zero::update_images_(VkCommandBuffer command_buffer,
                                             VkImageView src_image_view, VkImageView dst_image_view)
{
    if ((src_image_view_ == src_image_view) && (dst_image_view_ == dst_image_view)) {
        return;
    }

    auto set = acquire_set_();

    if (src_image_view_ != src_image_view) {
        VkDescriptorImageInfo dii {};

        dii.imageView = src_image_view;
        dii.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkWriteDescriptorSet wds {};

        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = set;
        wds.dstBinding = 0;
        wds.dstArrayElement = 0;
        wds.descriptorCount = 1;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        wds.pImageInfo = &dii;

        vkUpdateDescriptorSets(context_->device(), 1, &wds, 0, nullptr);
        src_image_view_ = src_image_view;
    }
    else {
        VkCopyDescriptorSet cds {};

        cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        cds.srcSet = set_;
        cds.srcBinding = 0;
        cds.srcArrayElement = 0;
        cds.dstSet = set;
        cds.dstBinding = 0;
        cds.dstArrayElement = 0;
        cds.descriptorCount = 1;

        vkUpdateDescriptorSets(context_->device(), 0, nullptr, 1, &cds);
    }

    if (dst_image_view_ != dst_image_view) {
        VkDescriptorImageInfo dii {};

        dii.imageView = dst_image_view;
        dii.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

        VkWriteDescriptorSet wds {};

        wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wds.dstSet = set;
        wds.dstBinding = 1;
        wds.dstArrayElement = 0;
        wds.descriptorCount = 1;
        wds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        wds.pImageInfo = &dii;

        vkUpdateDescriptorSets(context_->device(), 1, &wds, 0, nullptr);
    }
    else {
        VkCopyDescriptorSet cds {};

        cds.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        cds.srcSet = set_;
        cds.srcBinding = 1;
        cds.srcArrayElement = 0;
        cds.dstSet = set;
        cds.dstBinding = 0;
        cds.dstArrayElement = 0;
        cds.descriptorCount = 1;

        vkUpdateDescriptorSets(context_->device(), 0, nullptr, 1, &cds);
        dst_image_view_ = dst_image_view;
    }

    set_ = set;
}

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps