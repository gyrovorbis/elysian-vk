#ifndef ELYSIAN_RENDERER_COMPUTE_PIPELINE_HPP
#define ELYSIAN_RENDERER_COMPUTE_PIPELINE_HPP

#if 0
// Provided by VK_VERSION_1_0
VkResult vkCreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines);

// Provided by VK_VERSION_1_0
typedef struct VkComputePipelineCreateInfo {
    VkStructureType                    sType;
    const void*                        pNext;
    VkPipelineCreateFlags              flags;
    VkPipelineShaderStageCreateInfo    stage;
    VkPipelineLayout                   layout;
    VkPipeline                         basePipelineHandle;
    int32_t                            basePipelineIndex;
} VkComputePipelineCreateInfo;

// Provided by VK_VERSION_1_0
typedef struct VkPipelineShaderStageCreateInfo {
    VkStructureType                     sType;
    const void*                         pNext;
    VkPipelineShaderStageCreateFlags    flags;
    VkShaderStageFlagBits               stage;
    VkShaderModule                      module;
    const char*                         pName;
    const VkSpecializationInfo*         pSpecializationInfo;
} VkPipelineShaderStageCreateInfo;
#endif

namespace elysian::renderer {


class ComputePipeline: public Pipeline {
public:

    class CreateInfo: public VkComputePipelineCreateInfo {
    public:
        CreateInfo(VkPipelineCreateFlags              flags,
                   const ShaderStage*                 pStage,
                   VkPipelineLayout                   layout,
                   VkPipeline                         basePipelineHandle,
                   int32_t                            basePipelineIndex):
            VkComputePipelineCreateInfo({
                VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                nullptr,
                flags,
                stage,
                layout,
                basePipelineHandle,
                basePipelineIndex
            }),
            m_pStage(pStage)
        {}

    private:
        const ShaderStage* m_pStage = nullptr;
    };

    struct Initializer {
        std::string                         name;
        std::shared_ptr<const CreateInfo>   pInfo;
        const Device*                       pDevice;
        PipelineCache*                      pPipelineCache;
    };

                ComputePipeline(Initializer initializer);
                ~ComputePipeline(void);

    operator    VkPipeline() const;

    auto        getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;
    Result      getResult(void) const;
    bool        isValid(void) const;
    VkPipeline  getHandle(void) const;

private:
    Result          m_result;
    VkPipeline      m_handle            = VK_INVALID_HANDLE;
    std::shared_ptr<const CreateInfo>
                    m_pInfo;
    std::string     m_name;
    const Device*   m_pDevice           = nullptr;
    PipelineCache*  m_pPipelineCache    = nullptr;
};

inline ComputePipeline::ComputePipeline(Initializer initializer):
    Pipeline(std::move(initializer.pInfo)),
    m_name(std::move(initializer.name)),
    m_pDevice(initializer.pDevice),
    m_pPipelineCache(initializer.pPipelineCache)
{
    m_result = vkCreateComputePipelines(m_pDevice, m_pPipelineCache, 1, m_pInfo.get(), nullptr, &m_handle);
}

inline ComputePipeline::~ComputePipeline(void) {
    vkDestroyPipeline(m_pDevice, getHandle(), nullptr);
}

inline ComputePipeline::operator VkPipeline() const { return getHandle(); }
inline std::shared_ptr<const ComputePipeline::CreateInfo> ComputePipeline::getCreateInfo(void) const { return m_pInfo; }
inline Result ComputePipeline::getResult(void) const { return m_result; }
inline bool ComputePipeline::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }
inline VkPipeline ComputePipeline::getHandle(void) const { return m_handle; }



}

#endif // ELYSIAN_RENDERER_COMPUTE_PIPELINE_HPP
