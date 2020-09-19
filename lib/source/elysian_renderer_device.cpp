#include <renderer/elysian_renderer_device.hpp>
#include <renderer/elysian_renderer_physical_device.hpp>
#include <renderer/elysian_renderer.hpp>
#include <renderer/elysian_renderer_debug_log.hpp>
#include <renderer/elysian_renderer_command.hpp>

namespace elysian::renderer {

Device::Device(const char* pName, const PhysicalDevice* pDevice, std::shared_ptr<const DeviceCreateInfo> pCreateInfo, Renderer* pRenderer):
    HandleObject<VkDevice, VK_OBJECT_TYPE_DEVICE>(nullptr, VK_NULL_HANDLE, pName),
    m_pCreateInfo(std::move(pCreateInfo)),
    m_pPhysicalDevice(pDevice),
    m_pRenderer(pRenderer)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
    queueCreateInfo.reserve(m_pCreateInfo->queueGroupInfo.size());
    std::vector<float> queuePriorityPool;
    for(int g = 0; g < m_pCreateInfo->queueGroupInfo.size(); ++g) {
        VkDeviceQueueCreateInfo info = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            m_pCreateInfo->queueGroupInfo[g].flags, // FUCKING FLAGS
            m_pCreateInfo->queueGroupInfo[g].properties.getFamilyIndex(),
            static_cast<uint32_t>(m_pCreateInfo->queueGroupInfo[g].queueProperties.size())
        };
        int startPriorityIdx = queuePriorityPool.size()? queuePriorityPool.size() - 1 : 0;
        for(int q = 0; q < m_pCreateInfo->queueGroupInfo[g].queueProperties.size(); ++q) {
            queuePriorityPool.push_back(m_pCreateInfo->queueGroupInfo[g].queueProperties[q].getPriority());
        }
        info.pQueuePriorities = &queuePriorityPool[startPriorityIdx];
        queueCreateInfo.push_back(std::move(info));
    }

    m_pRenderer->getLog()->verbose("Creating Device: %s", pName);
    m_pRenderer->getLog()->push();

    VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        static_cast<uint32_t>(queueCreateInfo.size()),
        queueCreateInfo.data(),
        0,
        nullptr,
        static_cast<uint32_t>(m_pCreateInfo->enabledExtensions.size()),
        m_pCreateInfo->enabledExtensions.data(),
        m_pCreateInfo->pFeatures
    };

    VkDevice device = VK_NULL_HANDLE;
    m_result = vkCreateDevice(
        m_pPhysicalDevice->getHandle(),
        &deviceCreateInfo,
        nullptr,
        //m_pRenderer.getAllocator(),
        &device);
    setHandle(device);

    if(m_result) {
        m_queueGroups = std::make_unique<std::vector<QueueGroup>>();
        for(int g = 0; g < m_pCreateInfo->queueGroupInfo.size(); ++g) {
            m_queueGroups->emplace_back(QueueGroup::Initializer{m_pCreateInfo->queueGroupInfo[g], this}, *m_pRenderer);
            //m_result &= m_queueGroups->back().isValid();
        }
    }

    VmaAllocatorCreateInfo vmaAllocatorInfo = {
        .physicalDevice = m_pPhysicalDevice->getHandle(),
        .device = getHandle(),
        .instance = m_pRenderer->getInstance()
    };
    vmaCreateAllocator(&allocatorInfo, &m_vmaAllocator);

    m_pRenderer->getLog()->pop();
}

Device::~Device(void) {
    waitIdle();
    vmaDestroyAllocator(m_vmaAllocator);
    vkDestroyDevice(getHandle(), nullptr);
}

PFN_vkVoidFunction Device::getProcAddr(const char* pName) const {
    return vkGetDeviceProcAddr(getHandle(), pName);
}

VkPeerMemoryFeatureFlags Device::getPeerMemoryFeatures(uint32_t heapIndex, uint32_t localDeviceIndex, uint32_t remoteDeviceIndex) const {
    VkPeerMemoryFeatureFlags features;
    vkGetDeviceGroupPeerMemoryFeatures(getHandle(), heapIndex, localDeviceIndex, remoteDeviceIndex, &features);
    return features;
}


void Device::log(DebugLog* pLog) const {
    if(!isValid()) {
        pLog->error("Failed to create Device: %s", getResult().toString());
    } else {
        pLog->verbose("Device Name: %s", getObjectName());
        pLog->verbose("Physical Device: %s", getPhysicalDevice().getProperties().deviceName);
        pLog->verbose("Enabled Extensions");
        pLog->push();
        for(int e = 0; e < getCreateInfo()->enabledExtensions.size(); ++e) {
            pLog->verbose("[%d]: %s", e, getCreateInfo()->enabledExtensions[e]);
        }
        pLog->pop();
        pLog->verbose("Queue Groups");
        pLog->push();
        for(int q = 0; q < m_queueGroups->size(); ++q) {
            pLog->verbose("[%d]: %s", q, (*m_queueGroups)[q].getName());
            pLog->push();
            (*m_queueGroups)[q].log(pLog);
            pLog->pop();
        }
        pLog->pop();
    }
}

CommandPool* Device::createCommandPool(const CommandPoolCreateInfo* pInfo) const {
    return new CommandPool(this, pInfo);
}

}
