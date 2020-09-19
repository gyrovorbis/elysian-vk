#ifndef ELYSIAN_RENDERER_DEVICE_HPP
#define ELYSIAN_RENDERER_DEVICE_HPP

#include "elysian_renderer_queue.hpp"
#include "elysian_renderer_object.hpp"
#include "VmaUsage.h"

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


class DeviceProcAddrCache: public ProcAddrCache {
public:
    InstanceProcAddrCache(LogicalDevice* pDevice):
        m_pDevice(pDevice) {}

    virtual PFN_vkVoidFunction getProcAddr(const char* pName) override {
        return vkGetDeviceProcAddr(m_pDevice->getHandle(), pName);
    }

private:
    Device* m_pDevice = nullptr;
};

class Device:
        public HandleObject<VkDevice, VK_OBJECT_TYPE_DEVICE>,
        public Context {
public:
    enum class Type: uint8_t {
        Device,
        DeviceGroup
    };

            Device(Instance* pInstance, std::shared_ptr<const DeviceCreateInfo> pCreateInfo, const char* pName);
    virtual ~Device(void);

    virtual Type getDeviceType(void) const = 0;
    virtual uint32_t getPhysicalDeviceCount(void) const = 0;
    virtual PhysicalDevice* getPhysicalDevice(uint32_t index=0) const = 0;

    std::pair<bool, Version> supportsExtension(const char* pName) const {
        Version maxVersion(std::numeric_limits<uint32_t>::max());
        bool supported = true;
        for(int d = 0; d < getPhysicalDeviceCount(); ++d) {
            const auto* pDev  = getPhysicalDevice(d);
            auto& [supported, version] = pDev->supportsExtension(pName);

            if(!supported) {
                supported = false;
                maxVersion = 0;
                break;
            } else if(version < maxVersion) {
                maxVersion  = version;
            }
        }
        return std::pair(supported, maxVersion);
    }

    virtual void log(DebugLog* pLog) const;

    DeviceProcAddrCache& getProcAddrCache(void) const { return m_procAddrCache; }
    PFN_vkVoidFunction getProcAddr(const char* pName) const;

    Result waitIdle(void) const { return vkDeviceWaitIdle(getHandle()); }

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

#if 0
    void vkGetDescriptorSetLayoutSupport(
        VkDevice                                    device,
        const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
        VkDescriptorSetLayoutSupport*               pSupport);
#endif


private:
    mutable DeviceProcAddrCache m_procAddrCache; // use this for all internal shit!
    std::shared_ptr<const DeviceCreateInfo>  m_pCreateInfo;
    std::unique_ptr<std::vector<QueueGroup>> m_queueGroups;
    Allocator* m_pAllocator = nullptr; // one per device!!
    Instance* m_pInstance = nullptr;
    // shader pipeline cache
};


class Device: public LogicalDevice {
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

    virtual Type getDeviceType(void) const override { return Type::Device; }
    virtual uint32_t getPhysicalDeviceCount(void) const override { return 1; }
    virtual PhysicalDevice* getPhysicalDevice(uint32_t index=0) const override { return index == 0? getPhysicalDevice() : nullptr; }

    std::shared_ptr<const DeviceCreateInfo> getCreateInfo(void) const { return m_pCreateInfo; }
    uint32_t getPhysicalDeviceCount(void) const override { return 1; }

    void log(DebugLog* pLog) const;

private:
    const PhysicalDevice*                    m_pPhysicalDevice   = nullptr;
};

class DeviceGroup: public Device {

    DeviceGroup(Instance* pInstance,
                const PhysicalDeviceGroup* physicalDeviceGroup,
                uint32_t deviceMask,
                std::shared_ptr<const DeviceCreateInfo> pCreateInfo,
                const char* pName):
        LogicalDevice(pInstance, std::move(pCreateInfo), pName),
        m_physicalDevices(std::move(physicalDevices))

    {
        vk::StructureChain<vk::DeviceCreateInfo, vk::MemoryDedicatedAllocateInfo> c = {
          vk::MemoryAllocateInfo(size, type),
          vk::MemoryDedicatedAllocateInfo(image)
        };

    }

    virtual Type getDeviceType(void) const override { return Type::DeviceGroup; }
    virtual uint32_t getPhysicalDeviceCount(void) const override { return m_physicalDevices.size(); }
    virtual PhysicalDevice* getPhysicalDevice(uint32_t index=0) const override { return index < m_physicalDevices.size()? m_physicalDevices[index] : nullptr; }

    //BINDING WORKS DIFFERENTLY, SUPPOSED TO HAVE AN ARRAY FOR ALL DEVICES

    uint32_t getDeviceMask(const PhysicalDevice* pPhysicalDevice) const;
    uint32_t getLocalDeviceIndex(const PhysicalDevice* pPhysicalDevice) const;
    PhysicalDevice* getPhysicalDevice(uint32_t localIndex) const;
    PhysicalDeviceGroup* getPhysicalDeviceGroup(void) const;

    VkPeerMemoryFeatureFlags getPeerMemoryFeatures(uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex) const;

    // Provided by VK_VERSION_1_1
    typedef struct VkDeviceGroupDeviceCreateInfo {
        VkStructureType            sType;
        const void*                pNext;
        uint32_t                   physicalDeviceCount;
        const VkPhysicalDevice*    pPhysicalDevices;
    } VkDeviceGroupDeviceCreateInfo;

    //get active device mask
    //get mask from local index
    //get mask from PhysicalDevice
#if 0
    // Provided by VK_VERSION_1_1
    void vkGetDeviceGroupPeerMemoryFeatures(
        VkDevice                                    device,
        uint32_t                                    heapIndex,
        uint32_t                                    localDeviceIndex,
        uint32_t                                    remoteDeviceIndex,
        VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures);
#endif

private:
    //std::map<uint64_t, VkPeerMemoryFeatureFlags>    m_PeerMemoryFeatureFlags; cache this shit....
    std::vector<PhysicalDevice*>                    m_physicalDevices;
    PhysicalDeviceGroup*                            m_pPhysicalDeviceGroup;
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
