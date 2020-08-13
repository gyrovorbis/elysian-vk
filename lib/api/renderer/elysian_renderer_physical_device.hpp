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

    const VkPhysicalDeviceProperties&  getProperties(void) const;
    const VkPhysicalDeviceFeatures& getFeatures(void) const;
    const VkPhysicalDeviceMemoryProperties& getMemoryProperties(void) const;
    auto getQueueFamilyProperties(void) const -> const std::vector<VkQueueFamilyProperties>&;

    void log(DebugLog* pLog) const;

    static void log(const VkPhysicalDeviceProperties& prop, DebugLog* pLog);
    static void log(const VkPhysicalDeviceFeatures& features, DebugLog* pLog);
    static void log(const VkPhysicalDeviceMemoryProperties& memProp, DebugLog* pLog);
    static void log(const VkQueueFamilyProperties& queueProp, DebugLog* pLog);

private:
    std::vector<VkQueueFamilyProperties>    m_queueFamilyProperties;
    std::vector<VkExtensionProperties>      m_extensionProperties;
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

private:
    const Renderer*                 m_pRenderer = nullptr;
    VkPhysicalDeviceGroupProperties m_properties;
};

inline const VkPhysicalDeviceProperties& PhysicalDevice::getProperties(void) const { return m_properties; }


}

#endif // ELYSIAN_RENDERER_PHYSICAL_DEVICE_HPP
