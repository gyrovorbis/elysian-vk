#ifndef ELYSIAN_RENDERER_DEVICE_HPP
#define ELYSIAN_RENDERER_DEVICE_HPP

#include "elysian_renderer_queue.hpp"
#include "elysian_renderer_object.hpp"

namespace elysian::renderer {

class PhysicalDevice;
class CommandPool;
class CommandPoolCreateInfo;

#if 0
struct PhysicalDeviceSelector {
public:
    int operator()(const PhysicalDevice* pDevice) {
        int total = 0;
        if(m_physicalDeviceSelector) total += m_physicalDeviceSelector(pDevice);
        if(m_propertySelector) total += m_propertySelector(pDevice->getProperties());
        if(m_featureSelector) total += m_featureSelector(pDevice->getFeatures());
        if(m_memoryPropertySelector) total += m_memoryPropertySelector(pDevice->getMemoryProperties());
        return total;
    }

    std::function<int(const PhysicalDevice*)> m_physicalDeviceSelector;
    std::function<int(const VkPhysicalDeviceProperties*)> m_propertySelector;
    std::function<int(const VkPhysicalDeviceFeatures*)> m_featureSelector;
    std::function<int(const VkPhysicalDeviceMemoryProperties*)> m_memoryPropertySelector;
};
#endif

struct DeviceCreateInfo {
    std::vector<QueueGroupCreateInfo> queueGroupInfo;
    std::vector<const char*> enabledExtensions;
    const VkPhysicalDeviceFeatures* pFeatures   = nullptr;
};


class Device: public HandleObject<VkDevice, VK_OBJECT_TYPE_DEVICE> {
public:
#if 0
    typedef struct VkDeviceCreateInfo {
        VkStructureType                    sType;
        const void*                        pNext;
        VkDeviceCreateFlags                flags;
        uint32_t                           queueCreateInfoCount;
        const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
        uint32_t                           enabledLayerCount;
        const char* const*                 ppEnabledLayerNames;
        uint32_t                           enabledExtensionCount;
        const char* const*                 ppEnabledExtensionNames;
        const VkPhysicalDeviceFeatures*    pEnabledFeatures;
    } VkDeviceCreateInfo;

    // Provided by VK_EXT_display_control
    VkResult vkRegisterDeviceEventEXT(
        VkDevice                                    device,
        const VkDeviceEventInfoEXT*                 pDeviceEventInfo,
        const VkAllocationCallbacks*                pAllocator,
        VkFence*                                    pFence);
#endif


    Device(const char* pName, const PhysicalDevice* pDevice, std::shared_ptr<const DeviceCreateInfo> pCreateInfo, Renderer* pRenderer);
    Device(Device&& rhs);

    virtual ~Device(void);

    const PhysicalDevice& getPhysicalDevice(void) const { return *m_pPhysicalDevice; }
    std::shared_ptr<const DeviceCreateInfo> getCreateInfo(void) const { return m_pCreateInfo; }

    const QueueGroup* getQueueGroup(int index) const;
    const QueueGroup* getQueueGroup(const char* pName) const;
    const QueueGroup* getQueueGroupByFamily(int familyIndex) const;

    auto getQueueGroups(void) const -> const std::vector<QueueGroup>& { return *m_queueGroups; }

    template<typename G, typename Q>
    const Queue* getQueue(G groupId, Q indexId={}) const {
        return getQueueGroup(groupId).getQueue(indexId);
    }

    template<typename Q>
    const Queue* getQueueByFamily(int familyIndex, Q indexId={}) const {
        return getQueueGroupByFamily(familyIndex)->getQueue(indexId);
    }

    template<typename G>
    auto getQueues(G groupId) const -> const std::vector<Queue>& { return getQueueGroup(groupId)->getQueues(); }

    Result waitIdle(void) const { return vkDeviceWaitIdle(getHandle()); }

    Result getResult(void) const { return m_result; }

    PFN_vkVoidFunction getProcAddr(const char* pName) const;
    VkPeerMemoryFeatureFlags getPeerMemoryFeatures(uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex) const;

    CommandPool* createCommandPool(const CommandPoolCreateInfo* pInfo) const;

#if 0
    void vkGetDescriptorSetLayoutSupport(
        VkDevice                                    device,
        const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
        VkDescriptorSetLayoutSupport*               pSupport);
#endif


    void log(DebugLog* pLog) const;


#if 0
    // Provided by VK_VERSION_1_0
    typedef struct VkMappedMemoryRange {
        VkStructureType    sType;
        const void*        pNext;
        VkDeviceMemory     memory;
        VkDeviceSize       offset;
        VkDeviceSize       size;
    } VkMappedMemoryRange;

    // Provided by VK_VERSION_1_0
    VkResult vkFlushMappedMemoryRanges(
        VkDevice                                    device,
        uint32_t                                    memoryRangeCount,
        const VkMappedMemoryRange*                  pMemoryRanges);

    // Provided by VK_VERSION_1_0
    VkResult vkInvalidateMappedMemoryRanges(
        VkDevice                                    device,
        uint32_t                                    memoryRangeCount,
        const VkMappedMemoryRange*                  pMemoryRanges);

#endif

private:
    std::shared_ptr<const DeviceCreateInfo>  m_pCreateInfo;
    std::unique_ptr<std::vector<QueueGroup>> m_queueGroups;
    const PhysicalDevice*                    m_pPhysicalDevice   = nullptr;
    Renderer*                                m_pRenderer         = nullptr;
    Result                                   m_result;
};


inline Device::Device(Device&& rhs):
    m_pCreateInfo(std::move(rhs.m_pCreateInfo)),
    m_queueGroups(std::move(rhs.m_queueGroups)),
    m_pPhysicalDevice(rhs.m_pPhysicalDevice),
    m_pRenderer(rhs.m_pRenderer),
    m_result(rhs.m_result)
{}




}

#endif // ELYSIAN_RENDERER_DEVICE_HPP
