#ifndef ELYSIAN_RENDERER_RENDER_PASS_HPP
#define ELYSIAN_RENDERER_RENDER_PASS_HPP

namespace elysian::renderer {

class AttachmentDescription: public VkAttachmentDescription {
public:
    AttachmentDescription(VkAttachmentDescription   flags,
                          VkFormat                  format,
                          VkSampleCountFlagBits     samples,
                          VkAttachmentLoadOp        loadOp,
                          VkAttachmentStoreOp       storeOp,
                          VkAttachmentLoadOp        stencilLoadOp,
                          VkAttachmentStoreOp       stencilStoreOp,
                          VkImageLayout             initialLayout,
                          VkImageLayout             finalLayout):
        VkAttachmentDescription({
            flags,
            format,
            samples,
            loadOp,
            storeOp,
            stencilLoadOp,
            stencilStoreOp,
            initialLayout,
            finalLayout
        })
    {}
};


class SubpassDescription: public VkSubpassDescription {
public:
#if 0
    typedef struct VkSubpassDescription {
        VkSubpassDescriptionFlags       flags;
        VkPipelineBindPoint             pipelineBindPoint;
        uint32_t                        inputAttachmentCount;
        const VkAttachmentReference*    pInputAttachments;
        uint32_t                        colorAttachmentCount;
        const VkAttachmentReference*    pColorAttachments;
        const VkAttachmentReference*    pResolveAttachments;
        const VkAttachmentReference*    pDepthStencilAttachment;
        uint32_t                        preserveAttachmentCount;
        const uint32_t*                 pPreserveAttachments;
    } VkSubpassDescription;
#endif
};


class SubpassDependency: public VkSubpassDependency {
public:
#if 0
    typedef struct VkSubpassDependency {
        uint32_t                srcSubpass;
        uint32_t                dstSubpass;
        VkPipelineStageFlags    srcStageMask;
        VkPipelineStageFlags    dstStageMask;
        VkAccessFlags           srcAccessMask;
        VkAccessFlags           dstAccessMask;
        VkDependencyFlags       dependencyFlags;
    } VkSubpassDependency;
#endif
};




class RenderPass: public VkRenderPassCreateInfo {
#if 0
    typedef struct VkRenderPassCreateInfo {
        VkStructureType                   sType;
        const void*                       pNext;
        VkRenderPassCreateFlags           flags;
        uint32_t                          attachmentCount;
        const VkAttachmentDescription*    pAttachments;
        uint32_t                          subpassCount;
        const VkSubpassDescription*       pSubpasses;
        uint32_t                          dependencyCount;
        const VkSubpassDependency*        pDependencies;
    } VkRenderPassCreateInfo;
#endif
public:
    RenderPass(std::vector<AttachmentDescription>   attachments={},
               std::vector<SubpassDescription>      subpassDescriptions={},
               std::vector<SubpassDependency>       subpassDependencies={}):
        VkRenderPassCreateInfo({
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            nullptr,
            0, //does actually have some qualcomm shit for transform?
            attachments.size(),
            attachments.data(),
            subpassDescriptions.size(),
            subpassDescriptions.data(),
            subpassDependencies.size(),
            subpassDependencies.data()
        })
    {}
};

}

#endif // ELYSIAN_RENDERER_RENDER_PASS_HPP
