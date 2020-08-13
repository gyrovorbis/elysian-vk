#ifndef ELYSIAN_RENDERER_BUFFER_HPP
#define ELYSIAN_RENDERER_BUFFER_HPP

#include "elysian_renderer_object.hpp"

namespace elysian::renderer {

#if 0
// Provided by VK_VERSION_1_0
VkResult vkCreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView);

// Provided by VK_VERSION_1_0
typedef struct VkBufferViewCreateInfo {
    VkStructureType            sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    const void*                pNext;
    VkBufferViewCreateFlags    flags;
    VkBuffer                   buffer;
    VkFormat                   format;
    VkDeviceSize               offset;
    VkDeviceSize               range;
} VkBufferViewCreateInfo;

// Provided by VK_VERSION_1_0
void vkDestroyBufferView(
    VkDevice                                    device,
    VkBufferView                                bufferView,
    const VkAllocationCallbacks*                pAllocator);


#endif
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

class BufferCreateInfo: public VkBufferCreateInfo {
    BufferCreateInfo(VkBufferCreateFlags   flags,
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

class Buffer: public HandleObject<VkBuffer, VK_OBJECT_TYPE_BUFFER> {
public:

    struct Initializer {
        std::string     name;
        const Device*   pDevice;
        CreateInfo      info;
        const Renderer* pRenderer;
    };

                Buffer(Initializer initializer);
                Buffer(const Device* pDevice, BufferCreateInfo* pInfo);
    virtual     ~Buffer(void);

    Result      getResult(void) const;

    auto        getMemory(void) const -> std::shared_ptr<const DeviceMemory>;
    auto        getMemoryOffset(void) const -> VkDeviceSize;
    auto        getDeviceAddress(void) const -> VkDeviceAddress;

    auto        getMemoryRequirements(void) const -> VkMemoryRequirements;
    Result      bindDeviceMemory(std::shared_ptr<DeviceMemory> pMemory, VkDeviceSize offset=0) const;

private:
    Initializer                     m_initializer;
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

inline Result Buffer::getResult(void) const { return m_result; }
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

inline Result Buffer::bindDeviceMemory(std::shared_ptr<DeviceMemory> pMemory, VkDeviceSize offset=0) const {
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
