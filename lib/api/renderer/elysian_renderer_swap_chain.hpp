#ifndef ELYSIAN_RENDERER_SWAP_CHAIN_HPP
#define ELYSIAN_RENDERER_SWAP_CHAIN_HPP

namespace elyisan::renderer {

class Swapchain {
public:

    class CreateInfo: public VkSwapchainCreateInfoKHR {
    public:
        CreateInfo(VkSwapchainCreateFlagsKHR        flags,
                   VkSurfaceKHR                     surface,
                   uint32_t                         minImageCount,
                   VkFormat                         imageFormat,
                   VkColorSpaceKHR                  imageColorSpace,
                   VkExtent2D                       imageExtent,
                   uint32_t                         imageArrayLayers,
                   VkImageUsageFlags                imageUsage,
                   VkSharingMode                    imageSharingMode,
                   std::vector<uint32_t>            queueIndices,
                   VkSurfaceTransformFlagBitsKHR    preTransform,
                   VkCompositeAlphaFlagBitsKHR      compositeAlpha,
                   VkPresentModeKHR                 presentMode,
                   VkBool32                         clipped,
                   VkSwapchainKHR                   oldSwapchain):
            VkSwapchainCreateInfoKHR({
                VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                nullptr,
                flags,
                surface,
                minImageCount,
                imageFormat,
                imageColorSpace,
                imageExtent,
                imageArrayLayers,
                imageUsage,
                imageSharingMode,
                queueIndices.size(),
                queueIndices.data(),
                preTransform,
                compositeAlpha,
                presentMode,
                clipped,
                oldSwapchain
            }),
            m_queueIndices(std::move(queueIndices))
        {}

        VkSwapchainCreateFlagsKHR       getFlags(void) const;
        VkSurfaceKHR                    getSurface(void) const;
        uint32_t                        getMinImageCount(void) const;
        VkFormat                        getImageFormat(void) const;
        VkColorSpaceKHR                 getImageColorSpace(void) const;
        VkExtent2D                      getImageExtent(void) const;
        uint32_t                        getImageArrayLayers(void) const;
        VkImageUsageFlags               getImageUsage(void) const;
        VkSharingMode                   getImageSharingMode(void) const;
        const std::vector<uint32_t>&    getQueueIndices(void) const;
        VkSurfaceTransformFlagBitsKHR   getPreTransform(void) const;
        VkCompositeAlphaFlagBitsKHR     getCompositeAlpha(void) const;
        VkPresentModeKHR                getPresentMode(void) const;
        bool                            isClipped(void) const;
        VkSwapChainKHR                  getOldSwapchain(void) const;

    private:
        std::vector<uint32_t>       m_queueIndices;
    };

    struct Initializer {
        std::shared_ptr<const CreateInfo> pInfo;
        const Device* pDevice;
    };

                    Swapchain(Initializer initializer);
                    ~Swapchain(void);

    operator        VkSwapChainKHR() const;

    Result          getResult(void) const;
    bool            isValid(void) const;
    VkSwapchainKHR  getHandle(void) const;

    auto            getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;
    auto            getImages(void) const -> const std::vector<VkImage>&;

private:
    VkSwapchainKHR  m_handle = VK_INVALID_HANDLE;
    Result          m_result;
    const Device*   m_pDevice = nullptr;
    std::shared_ptr<const CreateInfo>
                    m_pInfo;
    std::vector<VkImage>
                    m_images;
};

inline Swapchain::Swapchain(Initializer initializer):
    m_pDevice(initializer.pDevice),
    m_pInfo(std::move(initializer.pInfo))
{
    m_result = vkCreateSwapchainKHR(m_pDevice, m_pInfo.get(), nullptr, &m_handle);
    uint32_t imageCount;

    if(m_result) {
        vkGetSwapchainImagesKHR(m_pDevice, m_handle, &imageCount, nullptr);
        m_images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_pDevice, m_handle, &imageCount, m_images.data());
    }
}

inline Swapchain::~Swapchain(void) {
    vkDestroySwapchainKHR(m_pDevice, getHandle(), nullptr);
}

inline Swapchain::operator VkSwapChainKHR() const { return getHandle(); }
inline Result Swapchain::getResult(void) const { return m_result; }
inline VkSwapchainKHR Swapchain::getHandle(void) const { return m_handle; }
inline bool Swapchain::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }
inline auto Swapchain::getCreateInfo(void) const -> std::shared_ptr<const CreateInfo> { return m_pInfo; }
inline const std::vector<VkImage>& Swapchain::getImages(void) const { return m_images; }

}

#endif // ELYSIAN_RENDERER_SWAP_CHAIN_HPP
