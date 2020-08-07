#ifndef ELYSIAN_RENDERER_MEMORY_HPP
#define ELYSIAN_RENDERER_MEMORY_HPP

namespace elysian::renderer {

class DeviceMemory {
public:
    class AllocateInfo: public VkMemoryAllocateInfo {
    public:
        AllocateInfo(VkDeviceSize allocationSize, uint32_t memoryTypeIndex, const void* pNext=nullptr):
            VkMemoryAllocateInfo({
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                pNext,
                allocationSize,
                memoryTypeIndex //includes property flags PLUS heap index
            })
        {}
    };

    struct Initializer {
        AllocateInfo    info;
        const Device*   pDevice;
    };

                   DeviceMemory(Initializer initializer);
                   ~DeviceMemory(void);
    operator       VkDeviceMemory() const;

    Result         getResult(void) const;
    bool           isValid(void) const;

    VkDeviceMemory getHandle(void) const;
    uint32_t       getMemoryTypeIndex(void) const;
    VkDeviceSize   getAllocationSize(void) const;

    VkResult       mapMemory(VkDeviceSize     offset,
                             VkDeviceSize     size,
                             VkMemoryMapFlags flags,
                             void**           ppData) const;

    void           unmapMemory(void) const;

private:
    Initializer     m_initializer;
    Result          m_result;
    VkDeviceMemory  m_handle        = VK_INVALID_HANDLE;
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

inline DeviceMemory::operator VkDeviceMemory() const { return getHandle(); }

inline Result DeviceMemory::getResult(void) const { return m_result; }
inline bool DeviceMemory::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }
inline VkDeviceMemory DeviceMemory::getHandle(void) const { return m_handle; }
inline uint32_t DeviceMemory::getMemoryTypeIndex(void) const { return m_initializer.info.memoryTypeIndex; }
inline VkDeviceSize DeviceMemory::getAllocationSize(void) const { return m_initializer.info.memoryAllocSize; }

inline DeviceMemory::mapMemory(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData) const {
    return vkMapMemory(m_initializer.pDevice, getHandle(), offset, size, flags, ppData);
}

inline DeviceMemory::unmapMemory(void) const {
    vkUnmapmemory(m_initializer.pDevice, getHandle());
}



}

#endif // ELYSIAN_RENDERER_MEMORY_HPP
