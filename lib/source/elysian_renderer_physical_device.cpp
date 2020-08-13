#include <renderer/elysian_renderer_physical_device.hpp>
#include <renderer/elysian_renderer.hpp>
#include <renderer/elysian_renderer_debug_log.hpp>
#include <renderer/elysian_renderer_result.hpp>
#include <cinttypes>

namespace elysian::renderer {

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice):
    HandleObject<VkPhysicalDevice, VK_OBJECT_TYPE_PHYSICAL_DEVICE>(nullptr, physicalDevice, nullptr)
{
    vkGetPhysicalDeviceProperties(getHandle(), &m_properties);
    vkGetPhysicalDeviceFeatures(getHandle(), &m_features);
    vkGetPhysicalDeviceMemoryProperties(getHandle(), &m_memoryProperties);
    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(getHandle(), &queueFamilyPropertyCount, nullptr);
    m_queueFamilyProperties.resize(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(getHandle(), &queueFamilyPropertyCount, m_queueFamilyProperties.data());

    uint32_t extensionPropertyCount = 0;
    Result result = vkEnumerateDeviceExtensionProperties(getHandle(), nullptr, &extensionPropertyCount, nullptr);
    if(result) {
        m_extensionProperties.resize(extensionPropertyCount);
        vkEnumerateDeviceExtensionProperties(getHandle(), nullptr, &extensionPropertyCount, m_extensionProperties.data());
    }
}

void PhysicalDevice::log(DebugLog *pLog) const {

    pLog->push();

    if(!isValid()) {
        pLog->error("Invalid physical device handle!");

    } else {

        pLog->verbose("Device Properties");
        pLog->push();
        log(m_properties, pLog);
        pLog->pop();

        pLog->verbose("Device Features");
        pLog->push();
        log(m_features, pLog);
        pLog->pop();

        pLog->verbose("Memory Properties");
        pLog->push();
        log(m_memoryProperties, pLog);
        pLog->pop();

        pLog->verbose("Queue Family Properties");
        pLog->push();
        for(int q = 0; q < m_queueFamilyProperties.size(); ++q) {
            pLog->verbose("Queue[%d]", q);
            pLog->push();
            log(m_queueFamilyProperties[q], pLog);
            pLog->pop();
        }
        pLog->pop();

    }

    pLog->pop();
}

#if 0
typedef struct VkPhysicalDeviceProperties {
    uint32_t                            apiVersion;
    uint32_t                            driverVersion;
    uint32_t                            vendorID;
    uint32_t                            deviceID;
    VkPhysicalDeviceType                deviceType;
    char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
    VkPhysicalDeviceLimits              limits;
    VkPhysicalDeviceSparseProperties    sparseProperties;
} VkPhysicalDeviceProperties;
#endif

void PhysicalDevice::log(const VkPhysicalDeviceProperties& prop, DebugLog* pLog) {

    const char* pDeviceTypeStr = "UNKNOWN";
    switch(prop.deviceType) {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:             pDeviceTypeStr = "OTHER";           break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:    pDeviceTypeStr = "INTEGRATED_GPU";  break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:      pDeviceTypeStr = "DISCRETE GPU";    break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:       pDeviceTypeStr = "VIRTUAL GPU";     break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:               pDeviceTypeStr = "CPU";             break;
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:          break;
    }

    pLog->verbose("%-45s: %40s", "API Version",     Version(prop.apiVersion).toString().c_str());
    pLog->verbose("%-45s: %40s", "Driver Version",  Version(prop.driverVersion).toString().c_str());
    pLog->verbose("%-45s: %40u", "Vendor ID",       prop.vendorID);
    pLog->verbose("%-45s: %40u", "Device ID",       prop.deviceID);
    pLog->verbose("%-45s: %40s", "Device Type",     pDeviceTypeStr);
    pLog->verbose("%-45s: %40s", "Device Name",     prop.deviceName);

    //pipeline cache UUID

    pLog->verbose("limits:");
    pLog->push();

    pLog->pop();

    pLog->verbose("sparse properties:");
    pLog->push();

    pLog->pop();
}

void PhysicalDevice::log(const VkPhysicalDeviceFeatures& features, DebugLog* pLog) {
#define LOG_BOOL_FEATURE(F) \
    pLog->verbose("%-45s: %20s", #F, features.F? "TRUE" : "FALSE")

    LOG_BOOL_FEATURE(robustBufferAccess);
    LOG_BOOL_FEATURE(fullDrawIndexUint32);
    LOG_BOOL_FEATURE(imageCubeArray);
    LOG_BOOL_FEATURE(independentBlend);
    LOG_BOOL_FEATURE(geometryShader);
    LOG_BOOL_FEATURE(tessellationShader);
    LOG_BOOL_FEATURE(sampleRateShading);
    LOG_BOOL_FEATURE(dualSrcBlend);
    LOG_BOOL_FEATURE(logicOp);
    LOG_BOOL_FEATURE(multiDrawIndirect);
    LOG_BOOL_FEATURE(drawIndirectFirstInstance);
    LOG_BOOL_FEATURE(depthClamp);
    LOG_BOOL_FEATURE(depthBiasClamp);
    LOG_BOOL_FEATURE(fillModeNonSolid);
    LOG_BOOL_FEATURE(depthBounds);
    LOG_BOOL_FEATURE(wideLines);
    LOG_BOOL_FEATURE(largePoints);
    LOG_BOOL_FEATURE(alphaToOne);
    LOG_BOOL_FEATURE(multiViewport);
    LOG_BOOL_FEATURE(samplerAnisotropy);
    LOG_BOOL_FEATURE(textureCompressionETC2);
    LOG_BOOL_FEATURE(textureCompressionASTC_LDR);
    LOG_BOOL_FEATURE(textureCompressionBC);
    LOG_BOOL_FEATURE(occlusionQueryPrecise);
    LOG_BOOL_FEATURE(pipelineStatisticsQuery);
    LOG_BOOL_FEATURE(vertexPipelineStoresAndAtomics);
    LOG_BOOL_FEATURE(fragmentStoresAndAtomics);
    LOG_BOOL_FEATURE(shaderTessellationAndGeometryPointSize);
    LOG_BOOL_FEATURE(shaderImageGatherExtended);
    LOG_BOOL_FEATURE(shaderStorageImageExtendedFormats);
    LOG_BOOL_FEATURE(shaderStorageImageMultisample);
    LOG_BOOL_FEATURE(shaderStorageImageReadWithoutFormat);
    LOG_BOOL_FEATURE(shaderStorageImageWriteWithoutFormat);
    LOG_BOOL_FEATURE(shaderUniformBufferArrayDynamicIndexing);
    LOG_BOOL_FEATURE(shaderSampledImageArrayDynamicIndexing);
    LOG_BOOL_FEATURE(shaderStorageBufferArrayDynamicIndexing);
    LOG_BOOL_FEATURE(shaderStorageImageArrayDynamicIndexing);
    LOG_BOOL_FEATURE(shaderClipDistance);
    LOG_BOOL_FEATURE(shaderCullDistance);
    LOG_BOOL_FEATURE(shaderFloat64);
    LOG_BOOL_FEATURE(shaderInt64);
    LOG_BOOL_FEATURE(shaderInt16);
    LOG_BOOL_FEATURE(shaderResourceResidency);
    LOG_BOOL_FEATURE(shaderResourceMinLod);
    LOG_BOOL_FEATURE(sparseBinding);
    LOG_BOOL_FEATURE(sparseResidencyBuffer);
    LOG_BOOL_FEATURE(sparseResidencyImage2D);
    LOG_BOOL_FEATURE(sparseResidencyImage3D);
    LOG_BOOL_FEATURE(sparseResidency2Samples);
    LOG_BOOL_FEATURE(sparseResidency4Samples);
    LOG_BOOL_FEATURE(sparseResidency8Samples);
    LOG_BOOL_FEATURE(sparseResidency16Samples);
    LOG_BOOL_FEATURE(sparseResidencyAliased);
    LOG_BOOL_FEATURE(variableMultisampleRate);
    LOG_BOOL_FEATURE(inheritedQueries);

#undef LOG_BOOL_FEATURE
}
#if 0
typedef struct VkPhysicalDeviceMemoryProperties {
    uint32_t        memoryTypeCount;
    VkMemoryType    memoryTypes[VK_MAX_MEMORY_TYPES];
    uint32_t        memoryHeapCount;
    VkMemoryHeap    memoryHeaps[VK_MAX_MEMORY_HEAPS];
} VkPhysicalDeviceMemoryProperties;
#endif
void PhysicalDevice::log(const VkPhysicalDeviceMemoryProperties& memProp, DebugLog* pLog) {
    pLog->verbose("Memory Types");
    pLog->push();
    for(uint32_t t = 0; t < memProp.memoryTypeCount; ++t) {
        pLog->verbose("memoryType[%u]", t);
        pLog->push();
        typedef struct VkMemoryType {
            VkMemoryPropertyFlags    propertyFlags;
            uint32_t                 heapIndex;
        } VkMemoryType;

        BitFieldStringifier stringifier(std::initializer_list<std::pair<uint64_t, std::string>>{
            {VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, "DEVICE_LOCAL"},
            {VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "HOST_VISIBLE"},
            {VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, "HOST_COHERENT"},
            {VK_MEMORY_PROPERTY_HOST_CACHED_BIT, "HOST_CACHED"},
            {VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, "LAZILY_ALLOCATED"},
            {VK_MEMORY_PROPERTY_PROTECTED_BIT, "PROTECTED"},
            {VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD, "DEVICE_COHERENT_AMD"},
            {VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD, "DEVICE_UNCACHED_AMD"}
        });
            /// uint32_t bitMask = 0x3;
           //std::string str = stringifier.stringifyMask(memProp.memoryTypes[t].propertyFlags);

        pLog->verbose("heapIndex: %u", memProp.memoryTypes[t].heapIndex);
        pLog->verbose("propertyFlags: %u", memProp.memoryTypes[t].propertyFlags);
        pLog->pop();
    }
    pLog->pop();

    pLog->verbose("Memory Heaps");
    pLog->push();
    for(uint32_t h = 0; h < memProp.memoryHeapCount; ++h) {
        pLog->verbose("Heap[%u]", h);
        pLog->push();
        pLog->verbose("size: %" PRIu64 " (%" PRIu64 " MB)", memProp.memoryHeaps[h].size, memProp.memoryHeaps[h].size / 1024 / 1024);
        pLog->verbose("flags: %u", memProp.memoryHeaps[h].flags);
        pLog->pop();
    }
    pLog->pop();
}

#if 0
typedef struct VkQueueFamilyProperties {
    VkQueueFlags    queueFlags;
    uint32_t        queueCount;
    uint32_t        timestampValidBits;
    VkExtent3D      minImageTransferGranularity;
} VkQueueFamilyProperties;
#endif
void PhysicalDevice::log(const VkQueueFamilyProperties& queueProp, DebugLog* pLog) {
    pLog->verbose("flags: %u", queueProp.queueFlags);
    pLog->verbose("count: %u", queueProp.queueCount);
    pLog->verbose("timestampValidBits: %u", queueProp.timestampValidBits);
    pLog->verbose("minImageTransferGranularity: [%u, %u, %u]",  queueProp.minImageTransferGranularity.width,
                                                                queueProp.minImageTransferGranularity.height,
                                                                queueProp.minImageTransferGranularity.depth);
}


PhysicalDeviceGroup::PhysicalDeviceGroup(const VkPhysicalDeviceGroupProperties& properties,
                    const Renderer* pRenderer):
    m_pRenderer(pRenderer)
{
    memcpy(&m_properties, &properties, sizeof(VkPhysicalDeviceGroupProperties));
}


bool PhysicalDeviceGroup::supportsSubsetAllocation(void) const { return m_properties.subsetAllocation; }

const PhysicalDevice* PhysicalDeviceGroup::getPhysicalDevice(int index) const {
    const PhysicalDevice* pDev = nullptr;
    VkPhysicalDevice handle = getPhysicalDeviceHandle(index);
    if(handle != VK_NULL_HANDLE && m_pRenderer) {
        int d = 0;
        const PhysicalDevice* pCur;
        while((pCur = m_pRenderer->getPhysicalDevice(d++))) {
            if(pCur->getHandle() == handle) {
                pDev = pCur;
                break;
            }
        }
    }
    return pDev;
}

VkPhysicalDevice PhysicalDeviceGroup::getPhysicalDeviceHandle(int index) const {
    assert(index < getPhysicalDeviceCount());
    VkPhysicalDevice handle = VK_NULL_HANDLE;
    if(index < getPhysicalDeviceCount()) {
        handle = m_properties.physicalDevices[index];
    }
    return handle;
}
uint32_t PhysicalDeviceGroup::getPhysicalDeviceCount(void) const { return m_properties.physicalDeviceCount; }

void PhysicalDeviceGroup::log(DebugLog* pLog) const {
    pLog->verbose("subsetAllocation: %s", supportsSubsetAllocation()? "TRUE" : "FALSE");
    pLog->verbose("devices: %d", getPhysicalDeviceCount());
    pLog->push();
    for(int d = 0; d < getPhysicalDeviceCount(); ++d) {
        pLog->verbose("device[%d]: %s", d, getPhysicalDevice(d)->getProperties().deviceName);
    }
    pLog->pop();
}

}
