#ifndef ELYSIAN_RENDERER_BUFFER_HPP
#define ELYSIAN_RENDERER_BUFFER_HPP

namespace elysian::renderer {
/*
// 1 - Buffer and Image might share commonality in "Resource" base
// 2 - DeviceMemory simplification
        a. Advanced mode - you bind manually
        b. Easy mode - exclusive memory is created and assigned automatically
// 3 - Convenience constructors for basic types
        a. vertex buffer
        b. index buffer
        c. uniform buffer
        d. samplers?
     */
class Buffer {
public:

    class CreateInfo: public VkBufferCreateInfo {
        CreateInfo(VkBufferCreateFlags   flags,
                   VkDeviceSize          size,
                   VkBufferUsageFlags    usage,
   \               std::vector<uint32_t> queueFamilyIndices={}): //only used with concurrent access
            VkBufferCreateInfo({
                VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                nullptr,
                flags,
                size,
                usage,
                queueFamilyIndices.size()? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
                queueFamilyIndices.size(),
                queueFamilyIndices.data()
            })
        {}
    };

    struct Initializer {
        std::string     name;
        const Device*   pDevice;
        CreateInfo      info;
        const Renderer* pRenderer;
    };

                Buffer(Initializer initializer);
                ~Buffer(void);
    operator    VkBuffer() const;

    Result      getResult(void) const;
    bool        isValid(void) const;
    VkBuffer    getHandle(void) const;
    const char* getName(void) const;


    auto        getMemory(void) const -> std::shared_ptr<const DeviceMemory>;
    auto        getMemoryOffset(void) const -> VkDeviceSize;
    auto        getDeviceAddress(void) const -> VkDeviceAddress;

    auto        getMemoryRequirements(void) const -> VkMemoryRequirements;
    VkResult    bindDeviceMemory(std::shared_ptr<DeviceMemory> pMemory, VkDeviceSize offset=0) const;

private:
    Initializer                     m_initializer;
    VkBuffer                        m_handle        = VK_INVALID_HANDLE;
    std::shared_ptr<DeviceMemory>   m_pMemory       = nullptr; // shared ptr?
    VkDeviceSize                    m_memoryOffset  = 0;
    Result                          m_result;

};

inline Buffer::Buffer(Initializer initializer):
    m_initializer(std::move(initializer))
{
    m_result = vkCreateBuffer(m_initializer.pDevice,
                              &m_initializer.info,
                              pRenderer? pRenderer->getAllocator() : nullptr,
                              &m_handle);
}

inline Buffer::~Buffer(void) {
    vkDestroyBuffer(m_initializer.pDevice,
                    getHandle(),
                    pRenderer? pRenderer->getAllocator() : nullptr);
}

Buffer::operator VkBuffer() const { return getHandle(); }

inline Result Buffer::getResult(void) const { return m_result; }
inline bool Buffer::isValid(void) const { return getHandle() != VK_INVALID_HANDLE && getMemory(); }
inline VkBuffer Buffer::getHandle(void) const { return m_handle; }
inline const char* Buffer::getName(void) const { return m_initializer.name.c_str(); }
inline auto Buffer::getMemoryOffset(void) const -> VkDeviceSize { return m_memoryOffset; }

inline VkMemoryRequirements Buffer::getMemoryRequirements(void) const {
    VkMemoryRequirements requirements;
    memset(&requirements, 0, sizeof(VkMemoryRequirements));
    vkGetBufferMemoryRequirements(m_initializer.pDevice, getHandle(), &requirements);
    return requirements;
}

inline VkDeviceAddress Buffer::getDeviceAddress(void) const {
    // Provided by VK_VERSION_1_2
    const auto info = VkBufferDeviceAddressInfo {
        VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
        nullptr,
        getHandle()
    };

    return vkGetBufferDeviceAddress(m_initializer.pDevice, &info);
}

inline VkResult Buffer::bindDeviceMemory(std::shared_ptr<DeviceMemory> pMemory, VkDeviceSize offset=0) const {
    const Result result = vkBindBufferMemory(m_initializer.pDevice,
                                             getHandle(),
                                             pMemory.get(),
                                             offset);
    if(result) {
        m_pMemory = pMemory;
        m_memoryOffset = offset;
    } else {
        m_pMemory = nullptr;
        m_memoryOffset = 0;
    }

    return result;
}


}

#endif // ELYSIAN_RENDERER_BUFFER_HPP
