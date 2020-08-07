#ifndef ELYSIAN_RENDERER_IMAGE_HPP
#define ELYSIAN_RENDERER_IMAGE_HPP

namespace elysian::renderer {

class Image {
public:

    class CreateInfo: public VkImageCreateInfo {
    public:
        CreateInfo(VkImageCreateFlagBits    flags,
                   VkImageType              imageType,
                   VkFormat                 format,
                   VkExtent3D               extent,
                   uint32_t                 mipLevels,
                   uint32_t                 arrayLayers,
                   VkSampleCountFlagBits    samples,
                   VkImageTiling            tiling,
                   VkImageUsageFlags        usage,
                   VkSharingMode            sharingMode,
                   std::vector<uint32_t>    queueFamilyIndices,
                   VkImageLayout            initialLayout):
            VkImageCreateInfo({
                VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                nullptr,
                flags,
                imageType,
                format,
                extent,
                mipLevels,
                arrayLayers,
                samples,
                tiling,
                usage,
                sharingMode,
                queueFamilyIndices.size(),
                queueFamilyIndices.data(),
                initialLayout
            })
        {}

    };

    struct Initializer {
        std::string     name;
        std::shared_ptr<const CreateInfo>
                        pInfo;
        const Device*   pDevice;
    };

                Image(Initializer initializer);
                ~Image(void);

    operator    VkImage() const;

    bool        isValid(void) const;
    VkImage     getHandle(void) const;
    Result      getResult(void) const;
    const char* getName(void) const;

    auto        getMemory(void) const -> std::shared_ptr<DeviceMemory>;
    auto        getMemoryOffset(void) const -> VkDeviceSize;

    auto        getMemoryRequirements(void) const -> VkMemoryRequirements;
    VkResult    bindDeviceMemory(std::shared_ptr<DeviceMemory> pMemory, VkDeviceSize offset=0) const;


private:
    std::string     m_name;
    const Device*   m_pDevice = nullptr;
    VkImage         m_handle = VK_INVALID_HANDLE;
    std::shared_ptr<const CreateInfo>
                    m_pInfo = nullptr;
    std::shared_ptr<DeviceMemory> m_pMemory;
    VkDeviceSize    m_offset;
    Result          m_result;

};

#if 0
// Provided by VK_VERSION_1_0
typedef struct VkImageViewCreateInfo {
    VkStructureType            sType;
    const void*                pNext;
    VkImageViewCreateFlags     flags;
    VkImage                    image;
    VkImageViewType            viewType;
    VkFormat                   format;
    VkComponentMapping         components;
    VkImageSubresourceRange    subresourceRange;
} VkImageViewCreateInfo;
#endif

class ImageView {
public:

    class CreateInfo: public VkImageViewCreateInfo {
    public:
        CreateInfo(VkImageViewCreateFlags     flags,
                   VkImage                    image,
                   VkImageViewType            viewType,
                   VkFormat                   format,
                   VkComponentMapping         components,
                   VkImageSubresourceRange    subresourceRange):
            VkImageViewCreateInfo({
                VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                nullptr,
                flags,
                image,
                viewType,
                format,
                components,
                subresourceRange
            })
        {}
    };

    class Initializer {
        std::shared_ptr<const CreateInfo> pInfo;
        const Device* pDevice;
    };

                    ImageView(Initializer Initializer);
                    ~ImageView(void);

    operator        VkImageView() const;

    Result          getResult(void) const;
    bool            isValid(void) const;
    VkImageView     getHandle(void) const;

    // const Image*    getImage(void) const; ?!

    auto            getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;

private:
    VkImageView     m_handle = VK_INVALID_HANDLE;
    std::shared_ptr<const CreateInfo>
                    m_pInfo;
    const Device*   m_pDevice = nullptr;
    Result          m_result;
};

#if 0
// Provided by VK_VERSION_1_0
typedef struct VkSamplerCreateInfo {
    VkStructureType         sType;
    const void*             pNext;
    VkSamplerCreateFlags    flags;
    VkFilter                magFilter;
    VkFilter                minFilter;
    VkSamplerMipmapMode     mipmapMode;
    VkSamplerAddressMode    addressModeU;
    VkSamplerAddressMode    addressModeV;
    VkSamplerAddressMode    addressModeW;
    float                   mipLodBias;
    VkBool32                anisotropyEnable;
    float                   maxAnisotropy;
    VkBool32                compareEnable;
    VkCompareOp             compareOp;
    float                   minLod;
    float                   maxLod;
    VkBorderColor           borderColor;
    VkBool32                unnormalizedCoordinates;
} VkSamplerCreateInfo;
#endif

class Sampler {
public:

    class CreateInfo: public VkSamplerCreateInfo {
    public:
        CreateInfo(VkSamplerCreateFlags    flags,
                   VkFilter                magFilter,
                   VkFilter                minFilter,
                   VkSamplerMipmapMode     mipmapMode,
                   VkSamplerAddressMode    addressModeU,
                   VkSamplerAddressMode    addressModeV,
                   VkSamplerAddressMode    addressModeW,
                   float                   mipLodBias,
                   VkBool32                anisotropyEnable,
                   float                   maxAnisotropy,
                   VkBool32                compareEnable,
                   VkCompareOp             compareOp,
                   float                   minLod,
                   float                   maxLod,
                   VkBorderColor           borderColor,
                   VkBool32                unnormalizedCoordinates):
            VkSamplerCreateInfo({
                VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                nullptr,
                flags,
                magFilter,
                minFilter,
                mipmapMode,
                addressModeU,
                addressModeV,
                addressModeW,
                mipLodBias,
                anisotropyEnable,
                maxAnisotropy,
                compareEnable,
                compareOp,
                minLod,
                maxLod,
                borderColor,
                unnormalizedCoordinates
            })
        {}
    };

    struct Initializer {
        std::string name;
        std::shared_ptr<const CreateInfo> pInfo;
        const Device* pDevice;
    };

                Sampler(Initializer initializer);
                ~Sampler(void);

    operator    VkSampler() const;

    VkSampler   getHandle(void) const;
    Result      getResult(void) const;
    bool        isValid(void) const;
    const char* getName(void) const;

    auto        getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;

private:
    std::string m_name;
    std::shared_ptr<const CreateInfo>
                m_pInfo;
    VkSampler   m_handle = VK_INVALID_HANDLE;
    Result      m_result;
    const Device*
                m_pDevice = nullptr;
};

inline Sampler::Sampler(Initializer initializer):
    m_name(std::move(initializer.name)),
    m_pInfo(std::move(initializer.pInfo)),
    m_pDevice(initializer.pDevice)
{
    m_result = vkCreateSampler(m_pDevice, m_pInfo.get(), nullptr, &m_handle);
}

inline Sampler::~Sampler(void) {
    vkDestroySampler(m_pDevice, getHandle(), nullptr);
}

inline Sampler::operator VkSampler() const { return getHandle(); }
inline VkSampler Sampler::getHandle(void) const { return m_handle; }
inline const char* Sampler::getName(void) const { return m_name.c_str(); }
inline Result Sampler::getResult(void) const { return m_result; }
inline bool Sampler::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }
inline std::shared_ptr<const CreateInfo> Sampler::getCreateInfo(void) const { return m_pInfo; }

inline ImageView::ImageView(Initializer initializer):
    m_pInfo(std::move(initializer.pInfo)),
    m_pDevice(initializer.pDevice)
{
    m_result = vkCreateImageView(m_pDevice, &viewInfo, nullptr, &m_handle);
}

inline ImageView::~ImageView(void) {
    vkDestroyImageView(m_pDevice, getHandle(), nullptr);
}

inline ImageView::operator VkImageView() const { return getHandle(); }
inline std::shared_ptr<const CreateInfo> ImageView::getCreateInfo(void) const { return m_pInfo; }
inline Result ImageView::getResult(void) const { return m_result; }
inline VkImageView ImageView::getHandle(void) const { return m_handle; }
inline bool ImageView::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }


inline Image::Image(Initializer initializer):
    m_name(std::move(initializer.name)),
    m_pInfo(std::move(initializer.pInfo)),
    m_pDevice(initializer.pDevice)
{
    std::assert(m_pDevice);
    m_reuslt = vkCreateImage(m_pDevice, m_pInfo.get(), nullptr, &m_handle);
}

inline Image::~Image(void) {
    vkDestroyImage(m_pDevice, getHandle(), nullptr);
}

inline Image::operator VkImage() const { return getHandle(); }
inline const char* Image::getName(void) const { return m_name.c_cstr(); }
inline Result Image::getResult(void) const { return m_result; }
inline bool Image::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; && getMemory(); }
inline VkImage Image::getHandle(void) const { return m_handle; }
inline std::shared_ptr<DeviceMemory> Image::getMemory(void) const { return m_pMemory; }
inline VkDeviceSize Image::getMemoryOffset(void) const { return m_offset; }

inline VkMemoryRequirements Image::getMemoryRequirements(void) const {
    VkMemoryRequirements req;
    vkGetImageMemoryRequirements(m_pDevice, getHandle(), &req);
    return req;
}

VkResult Image::bindDeviceMemory(std::shared_ptr<DeviceMemory> pMemory, VkDeviceSize offset) const {
    m_pMemory = std::move(pMemory);
    m_offset = offset;
    return vkBindImageMemory(m_pDevice, getHandle(), m_pMemory.get()->getHandle(), m_offset);
}



}

#endif // ELYSIAN_RENDERER_IMAGE_HPP
