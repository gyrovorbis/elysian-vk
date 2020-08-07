#ifndef ELYSIAN_RENDERER_DEVICE_HPP
#define ELYSIAN_RENDERER_DEVICE_HPP

namespace elysian::renderer {

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

class PhysicalDevice {
private:
    std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
    VkPhysicalDeviceProperties m_properties;
    VkPhysicalDeviceFeatures m_features;
    VkPhysicalDevice m_device = VK_NULL_HANDLE;
public:

    PhysicalDevice(VkPhysicalDevice physicalDevice);

    bool isValid(void) const;

    const VkPhysicalDeviceProperties*  getProperties(void) const;
    const vkPhysicalDeviceFeatures* getFeatures(void) const;
    const VkPhysicalDeviceMemoryProperties* getMemoryProperties(void) const;
    const auto getQueueFamilyProperties(void) const -> std::vector<VkQueueFamilyProperties>&;

    friend std::ostream& operator<<(std::ostream& os, const PhysicalDevice& device);
};

class Device {
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
#endif

    //Layers are no longer distinguished between Instance and Devices! Device layers ignored!
    // - supports way more shit with pNext
    class CreateInfo: public VkDeviceCreateInfo {
    public:
        CreateInfo(std::vector<QueueGroup::CreateInfo> queueGroupInfos={},
                   std::vector<const char*> enabledExtensions={},
                   const VkPhysicalDeviceFeatures* pFeatures=nullptr):
            VkDeviceCreateInfo({
                VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                nullptr,
                0,
                queueInfos.size(),
                queueInfos.data(),
                0,
                nullptr,
                enabledExtensions.size(),
                enabledExtensions.data(),
                pFeatures
            })
        {}
    private:
        VkPhysicalDeviceFeatures* pFeatures=nullptr;
    };

    struct Initializer {
        const char* pName                           = nullptr;
        std::vector<const char*> enabledExtensions;
        const VkPhysicalDeviceFeatures* pFeatures   = nullptr;
        std::vector<QueueGroup::CreateInfo> queueGroupInfo;
        const PhysicalDevice* pDevice               = nullptr;
    };

    Device(Initializer initializer, Renderer* pRenderer):
        m_name(initializer.pName),
        m_pPhysicalDevice(initializer.pDevice),
        m_pRenderer(pRenderer)
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfo(initializer.queueGroupInfo.size());
        std::vector<float> queuePriorityPool;
        for(int g = 0; g < initializer.queueGroupInfo.size(); ++g) {
            VkDeviceQueueCreateInfo info = {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                initializer.queueGroupInfo[g].flags, // FUCKING FLAGS
                initializer.queueGroupInfo[g].properties.getFamilyIndex(),
                initializer.queueGroupInfo[g].queueProperties.size()
            };
            int startPriorityIdx = std::max(queuePriorityPool.size() - 1, 0);
            for(int q = 0; q < initializer.queueGroupInfo[g].queueProperties.size(); ++q) {
                queuePriorityPool.push_back(initializer.queueGroupInfo[g].queueProperties[q].priority);
            }
            info.pQueuePriorities = &queuePriorityPool[startPriorityIdx];
            queueCreateInfo.push_back(std::move(info));
        }

        VkDeviceCreateInfo deviceCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr,
            0,
            queueCreateInfo.size(),
            queueCreateInfo.data(),
            0,
            nullptr,
            initializer.enabledExtensions.size(),
            initializer.enabledExtensions.data(),
            initializer.pFeatures
        };

        m_result = vkCreateDevice(
            m_pPhysicalDevice,
            &deviceCreateInfo,
            m_pRenderer.getAllocator(),
            &m_device);

        if(m_result) {
            m_queueGroups = new vector<QueueGroup>(initializer.queueGroupInfo.size());
            for(int g = 0; g < initializer.queueGroupInfo.size(); ++g) {
                m_queueGroups->push_back(QueueGroup(QueueGroup::Initializer(std::move(initializer.queueGroupInfo[g]), this)));
                m_result &= m_queueGroups->last().isValid();
            }
        }
    }

    ~Device(void) {
        vkDestroyDevice(m_device, m_pRenderer->getAllocator());
    }

    operator VkDevice(void) const { return m_device; }

    const char* getName(void) const { return m_name; }
    const PhysicalDevice& getPhysicalDevice(void) const { return *m_pPhysicalDevice; }

    const QueueGroup* getQueueGroup(int index) const;
    const QueueGroup* getQueueGroup(const char* pName) const;
    const QueueGroup* getQueueGroupByFamily(int familyIndex) const;

    auto getQueueGroups(void) const -> const std::vector<QueueGroup>& { return m_queueGroups; }

    template<typename G, typename Q>
    const Queue* getQueue(G groupId, Q indexId={}) const {
        return getQueueGroup(groupId).getQueue(indexId);
    }

    template<typename Q>
    const Queue* getQueueByFamily(int familyIndex, Q indexId={}) const {
        return getQueueGroupByFamily(familyIndex).getQueue(indexId);
    }

    template<typename G>
    auto getQueues(G groupId) const -> const std::vector<Queue>& { return getQueueGroup(groupId)->getQueues(); }

    VkResult waitIdle(void) const { return vkDeviceWaitIdle(m_device); }

    bool isValid(void) const { return m_result && m_device != VK_INVALID_HANDLE; }
    Result getResult(void) const { return m_result; }

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
protected:

private:
    std::unique_ptr<std::vector<QueueGroup>> m_queueGroups;
    std::string                              m_name;
    VkDevice                                 m_device            = VK_INVALID_HANDLE;
    const PhysicalDevice*                    m_pPhysicalDevice   = nullptr;
    Renderer*                                m_pRenderer         = nullptr;
    Result                                   m_result;
};



}

#endif // ELYSIAN_RENDERER_DEVICE_HPP
