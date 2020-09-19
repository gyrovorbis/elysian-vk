#ifndef ELYSIAN_RENDERER_PHYSICAL_DEVICE_HPP
#define ELYSIAN_RENDERER_PHYSICAL_DEVICE_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include "elysian_renderer_object.hpp"

namespace elysian::renderer {

class Renderer;
class DebugLog;

//VkPhysicalDeviceMemoryBudgetPropertiesEXT(3)
//typedef struct VkPhysicalDeviceIDProperties {
//typedef struct VkPhysicalDeviceSubgroupProperties {
//typedef struct VkPhysicalDeviceDriverProperties {



class PhysicalDevice: public HandleObject<VkPhysicalDevice, VK_OBJECT_TYPE_PHYSICAL_DEVICE> {
public:

    PhysicalDevice(VkPhysicalDevice physicalDevice);

    vk::PhysicalDeviceType  getType(void) const;
    Version                 getApiVersion(void) const;
    const char*             getName(void) const;

    // Gracefully return nothing if API version with this data isn't supported
    vk::DriverId            getDriverId(void) const;
    const char*             getDriverName(void) const;
    const char*             getDriverInfo(void) const;
    //Get unique identifier shit for serialization

    const vk::PhysicalDeviceProperties&                 getProperties(void) const;
    const vk::PhysicalDeviceVulkan11Properties&         getPropertiesVulkan11(void) const;
    const vk::PhysicalDeviceVulkan12Properties&         getPropertiesVulkan12(void) const;

    const vk::PhysicalDeviceFeatures&                   getFeatures(void) const;
    const vk::PhysicalDeviceVulkan11Features&           getFeaturesVulkan11(void) const;
    const vk::PhysicalDeviceVulkan12Features&           getFeaturesVulkan12(void) const;

    const vk::PhysicalDevicePCIBusInfoPropertiesEXT&    getPCIBusInfoProperties(void) const;
    //mostly bullshit feature flags that I guess should be wrapped...
    //need some shit for if they aren't available... need to be ptrs


#if 0
    vk::PhysicalDeviceMemoryBudgetPropertiesEXT //heap storage stats
    vk::PhysicalDeviceMemoryPriorityFeaturesEXT //allows for higher-priority flags when allocating!!
    vk::PhysicalDevicePerformanceQueryFeaturesKHR   //supports query pools + multiple + in secondary command buffers
    vk::PhysicalDevicePerformanceQueryPropertiesKHR //pools allowed to be used with vkCmdCopyQueryPoolResults.
    vk::PhysicalDevicePipelineCreationCacheControlFeaturesEXT //supports some extra createInfo flags + external synchronized shit?
    vk::PhysicalDevicePipelineExecutablePropertiesFeaturesKHR //bool, supports the goods
    vk::PhysicalDevicePrivateDataFeaturesEXT

    vk::SurfaceCapabilitiesKHR
    vk::SurfaceFormatKHR
    vk::PresentModeKHR

    VkResult vkGetPhysicalDeviceImageFormatProperties(
        VkPhysicalDevice                            physicalDevice,
        VkFormat                                    format,
        VkImageType                                 type,
        VkImageTiling                               tiling,
        VkImageUsageFlags                           usage,
        VkImageCreateFlags                          flags,
        VkImageFormatProperties*                    pImageFormatProperties);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
#endif

    const vk::PhysicalDeviceMemoryProperties& getMemoryProperties(void) const;
    auto getQueueFamilyProperties(void) const -> const std::vector<VkQueueFamilyProperties>&;

#if 0

        typedef struct VkPhysicalDevicePCIBusInfoPropertiesEXT {
            VkStructureType    sType;
            void*              pNext;
            uint32_t           pciDomain;
            uint32_t           pciBus;
            uint32_t           pciDevice;
            uint32_t           pciFunction;
        } VkPhysicalDevicePCIBusInfoPropertiesEXT;

        VkResult vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR(
            VkPhysicalDevice                            physicalDevice,
            uint32_t                                    queueFamilyIndex,
            uint32_t*                                   pCounterCount,
            VkPerformanceCounterKHR*                    pCounters,
            VkPerformanceCounterDescriptionKHR*         pCounterDescriptions);
#endif

    std::pair<bool, Version> supportsExtension(const char* pName) const {
        const auto* pExt = findExtensionProperties(pName);
        return pExt? std::pair(true, pExt->version) : std::pair(false, 0);
    }

    void log(DebugLog* pLog) const;

    static void log(const VkPhysicalDeviceProperties& prop, DebugLog* pLog);
    static void log(const VkPhysicalDeviceFeatures& features, DebugLog* pLog);
    static void log(const VkPhysicalDeviceMemoryProperties& memProp, DebugLog* pLog);
    static void log(const VkQueueFamilyProperties& queueProp, DebugLog* pLog);

protected:

    const vk::ExtensionProperties* findExtensionProperties(const char* pName) const {
        assert(pName);
        for(auto&& prop: m_properties) {
            if(strcmp(prop.name, pName) == 0) {
                return &prop;
            }
        }
        return nullptr;
    }

private:
    std::vector<VkQueueFamilyProperties>    m_queueFamilyProperties;
    std::vector<vk::ExtensionProperties>    m_extensionProperties;
    VkPhysicalDeviceProperties              m_properties;
    VkPhysicalDeviceFeatures                m_features;
    VkPhysicalDeviceMemoryProperties        m_memoryProperties;
};

class PhysicalDeviceGroup {
public:


                            PhysicalDeviceGroup(const VkPhysicalDeviceGroupProperties& properties,
                                                const Renderer* pRenderer=nullptr);

    void                    log(DebugLog* pLog) const;

    bool                    supportsSubsetAllocation(void) const;
    const PhysicalDevice*   getPhysicalDevice(int index) const;
    VkPhysicalDevice        getPhysicalDeviceHandle(int index) const;
    uint32_t                getPhysicalDeviceCount(void) const;

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


private:
    const Renderer*                 m_pRenderer = nullptr;
    VkPhysicalDeviceGroupProperties m_properties;
};

inline const VkPhysicalDeviceProperties& PhysicalDevice::getProperties(void) const { return m_properties; }


}

#endif // ELYSIAN_RENDERER_PHYSICAL_DEVICE_HPP
