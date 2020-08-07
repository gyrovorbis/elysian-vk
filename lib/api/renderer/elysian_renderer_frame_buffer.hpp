#ifndef ELYSIAN_RENDERER_FRAME_BUFFER_HPP
#define ELYSIAN_RENDERER_FRAME_BUFFER_HPP

namespace elysian::renderer {

class Framebuffer {
public:

    class CreateInfo: public VkFramebufferCreateInfo {
    public:
        CreateInfo(VkFramebufferCreateFlags    flags,
                   VkRenderPass                renderPass,
                   std::vector<VkImageView>    attachments,
                   uint32_t                    width,
                   uint32_t                    height,
                   uint32_t                    layers):
            VkFramebufferCreateInfo({
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                nullptr,
                flags,
                renderPass,
                attachments.size(),
                attachments.data(),
                width,
                height,
                layers
            }),
            m_attachments(std::move(attachments))
        {}

    private:
        std::vector<VkImageView> m_attachments;

    };

    struct Initializer {
        std::shared_ptr<const CreateInfo> pInfo;
        const Device* pDevice = nullptr;
    };

                    Framebuffer(Initializer initializer);
                    ~Framebuffer(void);

    operator        VkFramebuffer() const;

    Result          getResult(void) const;
    bool            isValid(void) const;
    VkFramebuffer   getHandle(void) const;

    auto            getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;

private:

    Result          m_result;
    VkFramebuffer   m_handle    = VK_INVALID_HANDLE;
    const Device*   m_pDevice   = nullptr;
    std::shared_ptr<const CreateInfo>
                    m_pInfo;

};

inline Framebuffer::Framebuffer(Initializer initializer):
    m_pDevice(initializer.pDevice),
    m_pInfo(std::move(initializer.pInfo))
{
    m_result = vkCreateFramebuffer(m_pDevice, m_pInfo.get(), nullptr, &m_handle);
}

inline Framebuffer::~Framebuffer(void) {
    vkDestroyFramebuffer(m_pDevice, getHandle(), nullptr);
}

inline Result Framebuffer::getResult(void) const { return m_result; }
inline VkFramebuffer Framebuffer::getHandle(void) const { return m_handle; }
inline bool Framebuffer::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }
inline auto Framebuffer::getCreateInfo(void) const -> std::shared_ptr<const CreateInfo> { return m_pInfo; }


}

#endif // ELYSIAN_RENDERER_FRAME_BUFFER_HPP
