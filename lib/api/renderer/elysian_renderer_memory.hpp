#ifndef ELYSIAN_RENDERER_MEMORY_HPP
#define ELYSIAN_RENDERER_MEMORY_HPP

#include "elysian_renderer_object.hpp"

namespace elysian::renderer {

class DeviceMemoryAllocateInfo: public VkMemoryAllocateInfo {
public:
    DeviceMemoryAllocateInfo(VkDeviceSize allocationSize, uint32_t memoryTypeIndex, const void* pNext=nullptr):
        VkMemoryAllocateInfo({
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            pNext,
            allocationSize,
            memoryTypeIndex //includes property flags PLUS heap index
        })
    {}
};

class DeviceMemory: public HandleObject<VkDeviceMemory, VK_OBJECT_TYPE_DEVICE_MEMORY> {
public:

    struct Initializer {
        AllocateInfo    info;
        const Device*   pDevice;
    };

                   DeviceMemory(Initializer initializer);
                   DeviceMemory(const Device* pDevice, const DeviceMemoryAllocateInfo* pInfo);
    virtual        ~DeviceMemory(void);

    Result         getResult(void) const;

    uint32_t       getMemoryTypeIndex(void) const;
    VkDeviceSize   getAllocationSize(void) const;   //bytes
    VkDeviceSize   getMemoryCommitment(void) const; // bytes

    Result          mapMemory(VkDeviceSize     offset,
                             VkDeviceSize     size,
                             VkMemoryMapFlags flags,
                             void**           ppData) const;

    void           unmapMemory(void) const;

private:
    Initializer     m_initializer;
    Result          m_result;
};


inline DeviceMemory::DeviceMemory(Initializer Initializer):
    m_initializer(std::move(Initializer))
{
    m_result = vkAllocateMemory(m_initializer.pDevice,
                                &m_initializer.info,
                                nullptr,
                                &m_handle);
}

inline DeviceMemory::~DeviceMemory(void) {
    vkFreeMemory(m_initializer.pDevice, getHandle(), nullptr);
}

inline Result DeviceMemory::getResult(void) const { return m_result; }
inline uint32_t DeviceMemory::getMemoryTypeIndex(void) const { return m_initializer.info.memoryTypeIndex; }
inline VkDeviceSize DeviceMemory::getAllocationSize(void) const { return m_initializer.info.memoryAllocSize; }
inline VkDeviceSize DeviceMemory::getMemoryCommitment(void) const {
    VkDeviceSize size = 0;
    vkGetDeviceMemoryCommitment(m_initializer.pDevice, getHandle(), &size);
    return size;
}

inline DeviceMemory::mapMemory(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData) const {
    return vkMapMemory(m_initializer.pDevice, getHandle(), offset, size, flags, ppData);
}

inline DeviceMemory::unmapMemory(void) const {
    vkUnmapMemory(m_initializer.pDevice, getHandle());
}



}

#endif // ELYSIAN_RENDERER_MEMORY_HPP
