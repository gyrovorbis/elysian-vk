#ifndef ELYSIAN_RENDERER_PIPELINE_HPP
#define ELYSIAN_RENDERER_PIPELINE_HPP

#include "elysian_renderer_render_pass.hpp"

namespace elysian::renderer::pipeline {

#if 0
VkResult vkGetPipelineExecutablePropertiesKHR(
    VkDevice                                    device,
    const VkPipelineInfoKHR*                    pPipelineInfo,
    uint32_t*                                   pExecutableCount,
    VkPipelineExecutablePropertiesKHR*          pProperties);

VkResult vkGetPipelineExecutableStatisticsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pStatisticCount,
    VkPipelineExecutableStatisticKHR*           pStatistics);

VkResult vkGetPipelineExecutableInternalRepresentationsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations);

#endif

class VertexInputBindingDescription: public VkVertexInputBindingDescription {
public:
    constexpr static const uint32_t kAutoBinding = 0xffffffff;

    VertexInputBindingDescription(uint32_t stride,
                                  VkVertexInputRate rate=VK_VERTEX_INPUT_RATE_VERTEX,
                                  uint32_t binding=kAutoBinding):
        VkVertexInputBindingDescription({
            binding,
            stride,
            rate
        })
    {}

    bool usesAutoBinding(void) const { return getBinding() == kAutoBinding; }
    uint32_t getBinding(void) const { return binding; }
    void setBinding(uint32_t value) { binding = value; }
};


class VertexInputAttributeDescription: public VkVertexInputAttributeDescription {
public:
    constexpr static const uint32_t kAutoLocation = 0xffffffff;

    VertexInputAttributeDescription(uint32_t offset, VkFormat format, uint32_t binding=0, uint32_t location=kAutoLocation):
        VkVertexInputAttributeDescription({
            location,
            binding,
            format,
            offset
        })
    {}

    bool usesAutoLocation(void) const { return getLocation() == kAutoLocation; }
    uint32_t getLocation(void) const { return location; }
    void setLocation(uint32_t location_) { location = location_; }
};


class VertexInputState: public VkPipelineVertexInputStateCreateInfo {

    VertexInputState(std::vector<VertexInputBindingDescription> bindings,
                             std::vector<VertexInputAttributeDescription> attributes):
        VkPipelineVertexInputStateCreateInfo({
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            bindings.size(),
            bindings.data(),
            attributes.size(),
            attributes.data()
        })
    {
        for(uint32_t b = 0; b < bindings.size(); ++b) {
            if(bindings[b].usesAutoBinding()) bindings[b].setBinding(b);
        }

        for(uint32_t a = 0; a < attributes.size(); ++a) {
            if(attributes[a].usesAutoLocation()) attributes[a].setLocation(a);
        }
    }
};

class InputAssemblyState: public VkPipelineInputAssemblyStateCreateInfo {
public:
    InputAssemblyState(VkPrimitiveTopology topology, bool primitiveRestartEnable=false):
        VkPipelineInputAssemblyStateCreateInfo({
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            nullptr,
            flags,
            topology,
            primitiveRestartEnable
        })
    {}
};


class TessellationState {};


//if multiple viewports feature isn't enabled, viewport and scissor count better be 1!
class ViewportState: public VkPipelineViewportStateCreateInfo {
    //Only valid for when viewport and/or scissors are dynamic!
    ViewportState(uint32_t counts=1, VkViewport* pViewports=nullptr, VkRect2D* pScissors=nullptr):
        VkPipelineViewportStateCreateInfo({
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            nullptr,
            0,
            counts,
            pViewports,
            counts,
            pScissors
        })
    {
        assert(counts);
    }

    ViewportState(std::vector<VkViewport> viewports,
                          std::vector<VkRect2D> scissors):
        ViewportState(viewports.size(), viewports.data(), scissors.data())
    {
        assert(viewports.size() == scissors.size());
    }
};

class RasterizationState: public VkPipelineRasterizationState {
public:
    struct DepthBiasInfo {
        float   constantFactor;
        float   clamp;
        float   slopeFactor;
    };

    RasterizationState(bool             depthClampEnable        = true,
                               bool             rasterizerDiscardEnable = true,
                               VkPolygonMode    polygonMode             = VK_POLYGON_MODE_FILL,
                               VkCullModeFlags  cullMode                = VK_CULL_MODE_BACK_BIT,
                               VkFrontFace      frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                               std::optional<DepthBiasInfo>
                                                depthBias               = std::nullopt,
                               float            lineWidth               = 1.0f):
        VkPipelineRasterizationState({
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            nullptr,
            0,
            depthClampEnable,
            rasterizerDiscardEnable,
            polygonMode,
            cullMode,
            frontFace,
            static_cast<bool>(depthBias),
            depthBias.value().constantFactor,
            depthBias.value().clamp,
            depthBias.value().slopeFactor,
            lineWidth
        })
    {}
};

class MultisampleState: public vkPipelineMultisampleStateCreateInfo {
public:

    MultisampleState(VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                             std::optional<float> minSampleShading      = std::nullopt,
                             std::vector<VkSampleMask> sampleMask       = {},
                             bool alphaToCoverageEnable                 = false,
                             bool alphaToOneEnable                      = false):
        vkPipelineMultisampleStateCreateInfo({
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            nullptr,
            0,
            rasterizationSamples,
            static_cast<bool>(minSampleShading),
            minSampleShading.value(),
            sampleMask.data(),
            alphaToCoverageEnable,
            alphaToOneEnable
        })
    {}
};

class DepthStencilState: public VkPipelineDepthStencilStateCreateInfo {
public:

    struct DepthBoundsTest {
        float min;
        float max;
    };

    struct StencilTest {
        VkStencilOpState front;
        VkStencilOpState back;
    };

    DepthStencilState(bool depthTestEnable,
                              bool depthWriteEnable,
                              VkCompareOp depthCompareOp,
                              std::optional<DepthBoundsTest> depthBoundsTest=std::nullopt,
                              std::optional<StencilTest> stencilTest=std::nullopt):
        VkPipelineDepthStencilStateCreateInfo({
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            nullptr,

        })
    {}
};

class ColorBlendState: public VkPipelineColorBlendStateCreateInfo {
public:
    struct BlendMode {
        VkBlendFactor  srcFactor;
        VkBlendFactor  dstFactror;
        VkBlendOp      op;
    };

    class AttachmentState: public VkPipelineColorBlendAttachmentState {
    public:
        AttachmentState(BlendMode             colorBlendMode,
                        BlendMode             alphaBlendMode,
                        VkColorComponentFlags colorWriteMask):
            AttachmentState(true,
                            colorBlendMode.srcFactor,
                            colorBlendMode.dstFactror,
                            colorBlendMode.op,
                            colorBlendMode.srcFactor,
                            colorBlendMode.dstFactror,
                            colorBlendMode.op,
                            colorWriteMask)
        {}

        AttachmentState(bool                   blendEnable         = false,
                        VkBlendFactor          srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                        VkBlendFactor          dstColorBlendFactor = VK_BLEND_FACTOR_ONE,
                        VkBlendOp              colorBlendOp        = VK_BLEND_OP_ADD,
                        VkBlendFactor          srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                        VkBlendFactor          dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                        VkBlendOp              alphaBlendOp        = VK_BLEND_OP_ADD,
                        VkColorComponentFlags  colorWriteMask      = 0):
            VkPipelineColorBlendAttachmentState({
                blendEnable,
                srcColorBlendFactor,
                dstColorBlendFactor,
                colorBlendOp,
                srcAlphaBlendFactor,
                dstAlphaBlendFactor,
                alphaBlendOp,
                colorWriteMask
            })
        {}

    };

    ColorBlendState(std::optional<VkLogicOp>     logicOp,
                            std::vector<AttachmentState> attachmentStates,
                            std::array<float, 4>         blendConstants = { 1.0f }):
        VkPipelineColorBlendStateCreateInfo({
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            nullptr,
            0,
            static_cast<bool>(logicOp),
            logicOp.value(),
            attachmentStates.size(),
            attachmentStates.data(),
            {
                blendConstants[0], blendConstants[1], blendConstants[2], blendConstants[3]
            }
        })
    {}
};

class DynamicState: public VkPipelineDynamicStateCreateInfo {
public:
    DynamicState(std::vector<VkDynamicState> dynamicStates={}):
        VkPipelineDynamicStateCreateInfo({
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            nullptr,
            0,
            dynamicStates.size(),
            dynamicStates.data()
        })
    {}
};

class PushConstantRange: public VkPushConstantRange {
public:

    PushConstantRange(VkShaderStateFlags    flags,
                      uint32_t              offset,
                      uint32_t              size):
        VkPushConstantRange({
            flags,
            offset,
            size
        })
    {}
};

/*abstraction to let you shit out a bunch of pipelines with different fixed-function configurations
// shit like "enableDepthTest(), setColorBlendState()" checking if pipeline variant exists in cache,
shitting out a new one if not
* https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/chap18.html#queries-pipestats

*/

// renderer shit
// exercise
// supplements
// diet/shopping
// NVidia references
// Med refills
//Possibly runtime polymorphic base that can be used for compute/raytracing pipelines as well
class Pipeline {
public:
    Pipeline(std::string name):
        m_name(std::move(name))
    {}

    const char* getName(void) const;
private:

    std::string        m_name;
};

//layout should be a shared pointer or some shit so other pipelines can use it!
class GraphicsPipeline: public Pipeline {
public:
    class CreateInfo: public VkGraphicsPipelineCreateInfo {
    private:
        std::vector<ShaderStage>         m_shaderStages;
        VertexInputState                 m_vertexInputState;
        InputAssemblyState               m_inputAssemblyState;
        std::optional<TessellationState> m_tessellationState     = std::nullopt;
        ViewportState                    m_viewportState;
        RasterizationState               m_rasterizationState;
        MultisampleState                 m_multisampleState;
        DepthStencilState                m_depthStencilState;
        ColorBlendState                  m_colorBlendState;
        std::optional<DynamicState>      m_dynamicState          = std::nullopt;
        Layout                           m_layout;
        RenderPass                       m_renderPass;
        GraphicsPipeline*                m_pBasePipeline         = nullptr;
    public:
        CreateInfo(VkPipelineCreateFlagBits         flags,
                   std::vector<ShaderStage>         shaderStages,
                   VertexInputState                 vertexInputState,
                   InputAssemblyState               inputAssemblyState,
                   std::optional<TessellationState> tessellationState,
                   ViewportState                    viewportState,
                   RasterizationState               rasterizationState,
                   MultisampleState                 multisampleState,
                   DepthStencilState                depthStencilState,
                   ColorBlendState                  colorBlendState,
                   std::optional<DynamicState>      dynamicState,
                   Layout                           layout,
                   RenderPass                       renderPass,
                   uint32_t                         subPass=0,
                   GraphicsPipeline*                pBasePipeline=nullptr):
            m_shaderStages(std::move(shaderStages)),
            m_vertexInputState(std::move(vertexInputState)),
            m_inputAssemblyState(std::move(inputAssemblyState)),
            m_tessellationState(std::move(tessellationState)),
            m_viewportState(std::move(viewportState)),
            m_rasterizationState(std::move(rasterizationState)),
            m_multisampleState(std::move(multisampleState)),
            m_depthStencilState(std::move(depthStencilState)),
            m_colorBlendState(std::move(colorBlendState)),
            m_dynamicState(std::move(dynamicState)),
            m_layout(std::move(layout)),
            m_renderPass(std::move(renderPass)),
            m_pBasePipeline(pBasePipeline)
        {
            const auto tempInfo = VkGraphicsPipelineCreateInfo {
                VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                nullptr,
                flags,
                m_shaderStages.size(),
                m_shaderStages.data(),
                &m_vertexInputState,
                &m_inputAssemblyState,
                m_tessellationState? &m_tessellationState.value() : nullptr,
                &m_viewportState,
                &m_rasterizationState,
                &m_multisampleState,
                &m_depthStencilState,
                &m_colorBlendState,
                m_dynamicState? &m_dynamicState.value() : nullptr,
                m_layout,
                m_renderPass,
                subPass,
                m_pBasePipeline? m_pBasePipeline->m_pipeline : VK_INVALID_HANDLE,  //base pipeline handle
                0                                                               //base pipeline index
            };

            memcpy(static_cast<VkGraphicsPipelineCreateInfo*>(this), &tempInfo, sizeof(VkGraphicsPipelineCreateInfo));
        }
    };

    struct Initializer {
        const char*     pName           = nullptr;
        CreateInfo      info;
        Device*         pDevice;
        PipelineCache*  pPipelineCache = nullptr;
        Allocator*      pAllocator     = nullptr;
    };

                              GraphicsPipeline(Initializer initializer);
                              ~GraphicsPipeline(void);
    operator                  VkGraphicsPipeline () const;

    auto                      getShaderStages(void) const -> const std::vector<ShaderStage>&;
    const VertexInputState&   getVertexInputState(void) const;
    const InputAssemblyState& getInputAssemblyState(void) const;
    auto                      getTessellationState(void) const -> const std::optional<TessellationState>&;
    const ViewportState&      getViewportState(void) const;
    const RasterizationState& getRasterizationState(void) const;
    const MultisampleState&   getMultisampleState(void) const;
    const DepthStencilState&  getDepthStencilState(void) const;
    const ColorBlendState&    getColorBlendState(void) const;
    auto                      getDynamicState(void) const -> const std::optional<DynamicState>&;
    const Layout&             getLayout(void) const;
    const RenderPass&         getRenderPass(void) const;
    const GraphicsPipeline*   getBasePipeline(void) const;

    Result                    getResult(void) const;
    bool                      isValid(void) const;

private:
    Initializer               m_initializer;
    Result                    m_result;
    VkGraphicsPipeline        m_pipeline = VK_INVALID_HANDLE;
};


inline GraphicsPipeline::GraphicsPipeline(Initializer initializer):
    Pipeline(std::move(initializer.pName)),
    m_initializer(std::move(initializer))
{
    m_result = vkCreateGraphicsPipelines(m_initializer.pDevice,
                                         m_initializer.pPipelineCache,
                                         1,
                                         &m_initializer.createInfo,
                                         m_initializer.pAllocator,
                                         &m_pipeline);
}

inline GraphicsPipeline::~GraphicsPipeline(void) {
    vkDestroyPipeline(m_initializer.pDevice,
                      m_pipeline,
                      m_initializer.pAllocator);
    //layout is going to be leaking like this!!!!
}



}

#endif // ELYSIAN_RENDERER_PIPELINE_HPP
