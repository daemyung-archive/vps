//
// This file is part of the "vps" project
// See "LICENSE" for license information.
//

#ifndef VPS_KERNEL_GUARD
#define VPS_KERNEL_GUARD

#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>
#include <ghc/filesystem.hpp>
#include <sc/Preamble.h>
#include <sc/Spirv_reflector.h>

namespace Vps {

//----------------------------------------------------------------------------------------------------------------------

namespace fs = ghc::filesystem;

//----------------------------------------------------------------------------------------------------------------------

class Context;
class Set_pool;

//----------------------------------------------------------------------------------------------------------------------

class Technique {
public:
    explicit Technique(Context* context, const std::string& label);

    virtual ~Technique();

    inline auto label() const
    { return label_; }

protected:
    void init_signature_(const std::vector<uint32_t>& source);

    void init_shader_module_(const std::vector<uint32_t>& source);

    void deinit_shader_module_();

    void init_set_layout_();

    void deinit_set_layout_();

    void init_set_pool_();

    void init_pipeline_layout_();

    void deinit_pipeline_layout_();

    void init_pipeline_();

    void deinit_pipeline_();

    VkDescriptorSet acquire_set_();

protected:
    Context* context_;
    std::string label_;
    Sc::Signature signature_;
    VkDescriptorSetLayout set_layout_;
    std::unique_ptr<Set_pool> set_pool_;
    VkShaderModule shader_module_;
    VkPipelineLayout pipeline_layout_;
    VkPipeline pipeline_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Vps

#endif // VPS_KERNEL_GUARD