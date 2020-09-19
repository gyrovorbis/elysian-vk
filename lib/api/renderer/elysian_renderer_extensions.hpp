#ifndef ELYSIAN_RENDERER_EXTENSIONS_HPP
#define ELYSIAN_RENDERER_EXTENSIONS_HPP

#include "elysian_renderer_object.hpp"

namespace elysian::renderer {

class ExtensionBase {
public:
    virtual vk::ObjectType  getParentType(void) const = 0;
    virtual const char*     getName(void)       const = 0;
    virtual bool            isSupported(void)   const = 0;
    virtual bool            wasRequested(void)  const = 0;
    virtual bool            isEnabled(void)     const = 0;
    virtual Version         getVersion(void)    const = 0;
};


template<typename Parent>
class Extension: public ExtensionBase, public ChildObject<Parent> {
public:
    Extension(Parent* pParent, bool requested=true):
        ChildObject<Parent>(pParent),
        m_enabled(enabled)
    {
        m_supported  = getParent()->supportsExtension(getName());
    }

    operator bool() const { return isEnabled(); }

    virtual vk::ObjectType getParentType(void) const override { return Parent::ObjectType; }
    virtual bool isSupported(void) const override { return m_supported.first(); }
    virtual bool isEnabled(void) const override { return m_enabled; }
    virtual bool wasRequested(void) const override { return m_requested; }
    virtual Version getVersion(void) const override { return m_supported.second(); }

private:
    std::pair<bool, Version>    m_supported;
    bool                        m_requested = true;
    bool                        m_enabled   = false;
};

template<typename Parent, constexpr const char* Name>
class StaticExtension: public Extension<Parent> {
public:
    static constexpr const char* cName = Name;

    StaticExtension(Parent* pParent, bool requested=true):
        Extension<Parent>(pParent, requested)
    {
        m_supported  = getParent()->supportsExtension(getName());
    }

    virtual const char* getName(void) const override { return cName; }
};

template<constexpr const char* Name>
using StaticDeviceExtension = StaticExtension<Device, Name>;

template<typename Parent>
class DynamicExtension: public Extension<Parent> {
public:

    DynamicExtension(const char* pName, Parent* pParent=nullptr, bool requested=true):
        Extension<Parent>(pParent, requested)
    {
        strncpy(m_name, pName, sizeof(m_name));
        m_supported  = getParent()->supportsExtension(getName());
    }

    virtual const char* getName(void) const override { return m_name; }

private:
    char m_name[VK_MAX_EXTENSION_NAME_SIZE] = { '\0' };
};

using DynamicDeviceExtension = DynamicExtension<Device>;


template<typename Parent, typename... Exts>
class ExtensionManager: public ChildObject<Parent> {
public:

    ExtensionManager(void);

    void createChildHandles(void);
    void destroyChildHandles(void);

    const DynamicDeviceExtension& findExtension(const char* pName) const {
        auto it = m_extMap.find(pName);
        return it != m_extMap.end()? *it : DynamicDeviceExtension(pName, getParent());
    }

    template<typename  Ext>
    decltype(auto) getExtension(void) const { return std::get<Ext>(m_staticExts); }

    template<typename  Ext>
    decltype(auto) getExtension(void) { return std::get<Ext>(m_staticExts); }

protected:


private:
    std::tuple<Exts...>                      m_staticExts;
    std::vector<DynamicDeviceExtension>      m_dynamicExts;
    std::map<std::string, DynamicDeviceExtension*>
                                             m_extMap;
};

template<constexpr const char* Name>
struct DeviceExtensionTraits {
    using CppType = DynamicDeviceExtension;
    using NameType = Name;
};

template<>
struct InstanceExtensionTraits<"VK_EXT_debug_utils"> {
    using CppType = DebugUtilsMessengerExtension;
};


class DebugUtilsMessengerExtension: public StaticInstanceExtension<"VK_EXT_debug_utils"> {
public:

    class DebugUtilsMessengerExt: public DeviceChildHandle<vk::DebugUtilsMessengerEXT> {
    public:
        DebugUtilsMessengerExt(Instance* pInstance, vk::DebugUtilsMessengerCreateInfoEXT createInfo);
        ~DebugUtilsMessengerExt(void);
    };

    using ChildHandleType           = DebugUtilsMessengerExt;
    using ChildHandleCreateInfoType = vk::DebugUtilsMessengerCreateInfoEXT;
    using ParentCreateInfoPNext     = vk::DebugUtilsMessengerCreateInfoEXT;

    DebugUtilsMessengerEXT(vk::DebugUtilsMessengerEXTCreateInfo* pInfo, Instance* pInstance);
    ~DebugUtilsMessengerEXT(void);
public:
//PFN_vkDebugUtilsMessengerCallbackEXT
    ChildHandleType* getChild(void) const;

private:
    DebugUtilsMessengerExt* m_child  = nullptr;

};

template<>
struct DeviceExtensionTraits<"VK_EXT_private_data"> {
    using CppType = PrivateDataDeviceExtension;
};

class PrivateDataDeviceExtension: public StaticDeviceExtension<"VK_EXT_private_data"> {
public:

#if 0
    If the VkPhysicalDevicePrivateDataFeaturesEXT structure is included in the pNext chain of VkPhysicalDeviceFeatures2,
    it is filled with values indicating whether the feature is supported.
    VkPhysicalDevicePrivateDataFeaturesEXT can also be used in the pNext chain of
    VkDeviceCreateInfo to enable the features.
#endif

    template<typename Parent>
    class PrivateDataSlot: public ObjectHandle<vk::PrivateDataSlotEXT>, ObjectChild<Parent> {
    public:
        DeviceChildPrivateDataSlot(Parent* pParent, PrivateDataDeviceExtension* pExt, vk::PrivateDataSlotCreateInfoEXT info={}):
            m_pParent(pParent),
            m_pExt(pExt)
        {
            setHandle(m_pExt->createPrivateDataSlot(&info));
        }

        ~DeviceChildPrivateDataSlot(void) {
            m_pExt->destroyPrivateDataSlot(*this);
        }

        Result setData(uint64_t value)  {  m_pExt->setPrivateData(m_pParent->getType(), m_pParent->getHandle(), *this, value); }
        uint64_t getData(void) const { return m_pExt->getPrivateData(m_pParent->getType(), m_pParent->getHandle(), *this); }
    private:
        Parent*                     m_pParent  = nullptr;
        PrivateDataDeviceExtension* m_pExt     = nullptr;
    };

    using ChildHandleType               = PrivateDataSlot;
    using ChildHandleCreateInfoType     = vk::PrivateDataSlotCreateInfoEXT;
    using DeviceCreateInfoPNext         = vk::PhysicalDevicePrivateDataFeaturesEXT; // enables support when creating device
    using PhysicalDeviceFeatures2PNext  = vk::PhysicalDevicePrivateDataFeaturesEXT; // checks for support in physical device?

    auto createPrivateDataSlot(const vk::PrivateDataSlotCreateInfoEXT* pCreateInfo) {
        return vk::createPrivateDataSlot(getParent(), pCreateInfo, getInstance()->getAllocator(), getDispatcher());
    }
    void destroyPrivateDataSlot(VkPrivateDataSlotEXT pChild) {
        vk::destroyPrivateDataSlotEXT(getParent(), pChild, getInstance()->getAllocator(), getDispatcher());
    }

    uint64_t  getPrivateData(vk::ObjectType ownerType, uint64_t handle, VkPrivateDataSlotEXT slot) const {
        uint64_t data;
        vk::GetPrivateDataEXT(getParent(), ownerType, handle, slot, &data, getDispatcher());
        return data;
    }

    Result setPrivateData(vk::ObjectType ownerType, uint64_t handle, VkPrivateDataSlotEXT slot, uint64_t data) const {
        return vk::SetPrivateDataEXT(getParent(), ownerType, handle, slot, data, getDispatcher());
    }
};

template<>
struct DeviceExtensionTraits<"VK_KHR_performance_query"> {
    using CppType = PerformanceQueryDeviceExtension;
};

class PerformanceQueryDeviceExtension: public StaticDeviceExtension<"VK_KHR_performance_query"> {};

template<>
struct DeviceExtensionTraits<"VK_KHR_pipeline_executable_properties"> {
    using CppType = PipelineExecutablePropertiesDeviceExtension;
};

// Provided by VK_KHR_pipeline_executable_properties
VkResult vkGetPipelineExecutablePropertiesKHR(
    VkDevice                                    device,
    const VkPipelineInfoKHR*                    pPipelineInfo,
    uint32_t*                                   pExecutableCount,
    VkPipelineExecutablePropertiesKHR*          pProperties);
//this is a tree structure!!!

class PipelineExecutablePropertiesDeviceExtension: public StaticDeviceExtension<"VK_KHR_pipeline_executable_properties"> {
public:

    using PhysicalDeviceProperties2PNext =  vk::PhysicalDevicePipelineExecutablePropertiesFeaturesKHR; // check support
    using DeviceCreateInfoPNext          =  vk::PhysicalDevicePipelineExecutablePropertiesFeaturesKHR; // enable support

    decltype(auto) getInternalRepresentations(const vk::PipelineExecutableInfoKHR& executableInfo) const {
        return getDevice()->getVkHandle().getInternalRepresentations(executableInfo, getDispatch());
    }

    decltype(auto) getProperties(const vk::PipelineInfoKHR& pipelineInfo) const
    {
        return getDevice()->getVkHandle().getPipelineExecutablePropertiesKHR(pipelineInfo, getDispatch());
    }

    decltype(auto) getStatistics(const vk::PipelineExecutableInfoKHR& executableInfo) const
    {
        return getDevice()->getVkHandle().getPipelineExecutableStatisticsKHR(executableInfo, getDispatch());
    }

    // Provided by VK_KHR_pipeline_executable_properties
    VkResult vkGetPipelineExecutablePropertiesKHR(
        VkDevice                                    device,
        const VkPipelineInfoKHR*                    pPipelineInfo,
        uint32_t*                                   pExecutableCount,
        VkPipelineExecutablePropertiesKHR*          pProperties);

    // Provided by VK_KHR_pipeline_executable_properties
    VkResult vkGetPipelineExecutableStatisticsKHR(
        VkDevice                                    device,
        const VkPipelineExecutableInfoKHR*          pExecutableInfo,
        uint32_t*                                   pStatisticCount,
        VkPipelineExecutableStatisticKHR*           pStatistics);
};

template<>
struct DeviceExtensionTraits<"VK_KHR_shader_clock"> {
    using CppType = ShaderClockDeviceExtension;
};

class ShaderClockDeviceExtension: public StaticDeviceExtension<"VK_KHR_shader_clock"> {};

template<>
struct DeviceExtensionTraits<"VK_EXT_calibrated_timestamps"> {
    using CppType = CalibratedTimestampsDeviceExtension;
};

class CalibratedTimestampsDeviceExtension: public StaticDeviceExtension<"VK_EXT_calibrated_timestamps"> {};

template<>
struct DeviceExtensionTraits<"VK_KHR_swapchain"> {
    using CppType = SwapChainDeviceExtension;
};

class SwapChainDeviceExtension: public StaticDeviceExtension<"VK_KHR_swapchain"> {};

template<>
struct DeviceExtensionTraits<"VK_EXT_pci_bus_info"> {
    using CppType = PciBusInfoDeviceExtension;
};

class PciBusInfoDeviceExtension: public StaticDeviceExtension<"VK_EXT_pci_bus_info"> {
public:
    using PhysicalDeviceProperties2PNext = vk::PhysicalDevicePCIBusInfoPropertiesEXT;
};

template<>
struct DeviceExtensionTraits<"VK_EXT_memory_budget"> {
    using CppType = MemoryBudgetDeviceExtension;
};

class MemoryBudgetDeviceExtension: public StaticDeviceExtension<"VK_EXT_memory_budget"> {
public:
    using PhysicalDeviceMemoryProperties2Next = vk::PhysicalDeviceMemoryBudgetPropertiesEXT;
    //not sure how to handle this, it should be filled by the physical device...
};

template<>
struct DeviceExtensionTraits<"VK_EXT_memory_priority"> {
    using CppType = MemoryPriorityDeviceExtension;
};

class MemoryPriorityDeviceExtension: public StaticDeviceExtension<"VK_EXT_memory_priority"> {
public:
    using MemoryAllocateInfoPNext       = vk::MemoryPriorityAllocateInfoEXT;
    using PhysicalDeviceFeatures2PNext  = vk::PhysicalDeviceMemoryPriorityFeaturesEXT; //returns whether feature is supported
    using DeviceCreateInfoPNext         = vk::PhysicalDeviceMemoryPriorityFeaturesEXT; //enables it
};

template<>
struct DeviceExtensionTraits<"VK_EXT_conditional_rendering"> {
    using CppType = ConditionalRenderingDeviceExtension;
};

class ConditionalRenderingDeviceExtension: public StaticDeviceExtension<"VK_EXT_conditional_rendering"> {};

template<>
struct DeviceExtensionTraits<"VK_EXT_extended_dynamic_state"> {
    using CppType = ExtendedDynamicStateDeviceExtension;
};

class ExtendedDynamicStateDeviceExtension: public StaticDeviceExtension<"VK_EXT_extended_dynamic_state"> {
public:
    using DeviceCreateInfoPNext         = vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT; // enables support when creating device
    using PhysicalDeviceFeatures2PNext  = vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT; // checks for support in physical device

    void cmdBindVertexBuffers(
        vk::CommandBuffer     commandBuffer,
        uint32_t              firstBinding,
        uint32_t              bindingCount,
        const vk::Buffer*     pBuffers,
        const vk::DeviceSize* pOffsets,
        const vk::DeviceSize* pSizes,
        const vk::DeviceSize* pStrides) const
    {
        vk::BindVertexBuffers2EXT(commandBuffer, firstBinding, bindingCount,
                                  pBuffers, pOffsets, pSizes, pStrides, getDispatch());
    }

    void cmdSetCullMode (
        vk::CommandBuffer  commandBuffer,
        vk::CullModeFlags  cullMode) const
    {
        vk::CmdSetCullModeEXT(commandBuffer, cullMode, getDispatch());
    }

    void cmdSetDepthBoundsTestEnable(
        vk::CommandBuffer commandBuffer,
        bool              depthBoundsTestEnable) const
    {
        vk::CmdSetDepthBoundsTestEnableEXT(commandBuffer, depthBoundsTestEnable,
                                           getDispatch());
    }

    void cmdSetDepthCompareOp(
        vk::CommandBuffer commandBuffer,
        vk::CompareOp     depthCompareOp) const
    {
        vk::CmdSetDepthCompareOpEXT(commandBuffer, depthCompareOp, getDispatch());
    }

    void cmdSetDepthTestEnable(
        vk::CommandBuffer commandBuffer,
        bool              depthTestEnable) const
    {
        vk::CmdSetDepthTestEnableEXT(commandBuffer, depthTestEnable, getDispatch());
    }

    void cmdSetDepthWriteEnable(
        vk::CommandBuffer commandBuffer,
        bool              depthWriteEnable) const
    {
        vk::CmdSetDepthWriteEnableEXT(commandBuffer, depthWriteEnable, getDispatch());
    }

    void cmdSetFrontFace(
        vk::CommandBuffer commandBuffer,
        vk::FrontFace     frontFace) const
    {
        vk::CmdSetFrontFaceEXT(commandBuffer, frontFace, getDispatch());
    }

    void cmdSetPrimitiveTopology(
        vk::CommandBuffer      commandBuffer,
        vk::PrimitiveTopology  primitiveTopology) const
    {
        vk::CmdSetPrimitiveTopologyEXT(commandBuffer, primitiveTopology, getDispatch());
    }

    void cmdSetScissorWithCount(
        vk::CommandBuffer commandBuffer,
        uint32_t          scissorCount,
        const vk::Rect2D* pScissors) const
    {
        vk::CmdSetScissorWithCountEXT(commandBuffer, scissorCount, pScissors, getDispatch());
    }

    void cmdSetStencilOp(
        vk::CommandBuffer     commandBuffer,
        vk::StencilFaceFlags  faceMask,
        vk::StencilOp         failOp,
        vk::StencilOp         passOp,
        vk::StencilOp         depthFailOp,
        vk::CompareOp         compareOp) const
    {
        vk::CmdSetStencilOpEXT(commandBuffer, faceMask, failop, passOp, depthFailOp, compareOp, getDispatch());
    }

    void cmdSetStencilTestEnable(
        vk::CommandBuffer commandBuffer,
        bool              stencilTestEnable) const
    {
       vk::CmdSetStencilTestEnableEXT(commandBuffer, stencilTestEnable, getDispatch());
    }

    void cmdSetViewportWithCount(
        vk::CommandBuffer    commandBuffer,
        uint32_t             viewportCount,
        const vk::Viewport*  pViewports) const
    {
        vk::CmdSetViewportWithCountEXT(commandBuffer, viewportCount, pViewports, getDispatch());
    }
};

template<>
struct DeviceExtensionTraits<"VK_EXT_inline_uniform_block"> {
    using CppType = InlineUniformBlockDeviceExtension;
};

class InlineUniformBlockDeviceExtension: public StaticDeviceExtension<"VK_EXT_inline_uniform_block"> {};

template<>
struct DeviceExtensionTraits<"VK_EXT_pipeline_creation_cache_control"> {
    using CppType = PipelineCreationCacheControlDeviceExtension;
};

class PipelineCreationCacheControlDeviceExtension: public StaticDeviceExtension<"VK_EXT_pipeline_creation_cache_control"> {};

template<>
struct DeviceExtensionTraits<"VK_EXT_pipeline_creation_feedback"> {
    using CppType = PipelineCreationFeedbackDeviceExtension;
};

class PipelineCreationFeedbackDeviceExtension: public StaticDeviceExtension<"VK_EXT_pipeline_creation_feedback"> {};

template<>
struct DeviceExtensionTraits<"VK_EXT_texel_buffer_alignment"> {
    using CppType = TexelBufferAlignmentDeviceExtension;
};

class TexelBufferAlignmentDeviceExtension: public StaticDeviceExtension<"VK_EXT_texel_buffer_alignment"> {};

template<>
struct DeviceExtensionTraits<"VK_EXT_tooling_info"> {
    using CppType = ToolingInfoDeviceExtension;
};

class ToolingInfoDeviceExtension: public StaticDeviceExtension<"VK_EXT_tooling_info"> {

    using PropertyList = std::vector<vk::PhysicalDeviceToolPropertiesEXT>;
    using ResultType = std::pair<Result, PropertyList>;

    ResultType getPhysicalDeviceToolProperties(vk::PhysicalDevice physicalDevice) const {
       Result result;
       PropertyList properties;
       uint32_t count = 0;

       if(_getPhysicalDeviceToolProperties(physicalDevice, &count, nullptr)) {
           properties.resize(count);
           result = _getPhysicalDeviceToolProperties(physicalDevice, count, properties.data());
       }
       return std::pair(result, std::move(properties));
    }

    Result logPhysicalDeviceToolProperties(PhysicalDevice* pPhysicalDevice) const {
        //might want to check if that bitch even supports the extension...
        auto* pLog = getDevice()->getInstance()->getLog();
        pLog->info("Tool Properties for Physical Device: %s", pPhysicalDevice->getName());
        pLog->push();
        auto& [result, props] = getPhysicalDeviceToolProperties(pPhysicalDevice);
        if(!result) {
            pLog->error("Query Failed: %s", result.toString().c_str());
        } else {
            for(int p = 0; p < props.size(); ++p) {
                const auto* pTool = &props[p];
                pLog->info("Tool[%d]: %s", p, pTool->name);
                pLog->push();
                pLog->info("Version:     %s", pTool->version);
                pLog->info("Purposes:    %s", vk::to_string(pTool->purposes).c_str());
                pLog->info("Description: %s", pTool->description);
                pLog->info("Layer:       %s", pTool->layer);
                pLog->pop();
            }
        }
        pLog->pop();
        return result;
    }

private:
    Result _getPhysicalDeviceToolProperties(vk::PhysicalDevice physicalDevice,
                                     uint32_t* pToolCount,
                                     vk::PhysicalDeviceToolPropertiesEXT* pToolProperties) const
    {
        return vk::GetPhysicalDeviceToolProperties(physicalDevice, pToolCount, pToolProperties, getDispatch());
    }
};

template<>
struct DeviceExtensionTraits<"VK_EXT_validation_features"> {
    using CppType = ValidationFeaturesDeviceExtension;
};

class ValidationFeaturesDeviceExtension: public StaticDeviceExtension<"VK_EXT_validation_features"> {
public:
    using DeviceCreateInfoPNext = vk::ValidationFeaturesEXT;

    ValidationFeaturesDeviceExtension(Device* pDevice=nullptr, vk::ValidationFeaturesEXT features=cDefaultValidationFeatures):
        StaticDeviceExtension<"VK_EXT_validation_features">::StaticDeviceExtension(pDevice),
        m_validationFeatures(std::move(features))
    {}

    static const constexpr vk::ValidationFeatureEnableEXT cDefaultEnableFeatures[] = {
        vk::ValidationFeatureEnableEXT::eGpuAssisted,
        vk::ValidationFeatureEnableEXT::eGpuAssistedReserveBindingSlot,
        vk::ValidationFeatureEnableEXT::eBestPractices,
        vk::ValidationFeatureEnableEXT::eDebugPrintf
    };

    static const constexpr vk::ValidationFeatureDisableEXT cDefaultDisableFeatures[] = {};

    static const constexpr vk::ValidationFeaturesEXT cDefaultValidationFeatures = {
        m_cDefaultEnableFeatures,
        m_cDefaultDisableFeatures
    };

private:

    vk::ValidationFeaturesEXT m_validationFeatures;
};

template<>
struct DeviceExtensionTraits<"VK_EXT_validation_cache"> {
    using CppType = ValidationCacheDeviceExtension;
};

class ValidationCacheDeviceExtension: public StaticDeviceExtension<"VK_EXT_validation_cache"> {};



template<>
struct DeviceExtensionTraits<"VK_EXT_validation_cache"> {
    using CppType = ValidationCacheDeviceExtension;
};

class ValidationCacheDeviceExtension: public StaticDeviceExtension<"VK_EXT_validation_cache"> {};


#if 0
In order to visibly display your content on iOS or macOS, you must enable the VK_MVK_ios_surface or VK_MVK_macos_surface extension, and use the functions defined for those extensions to create a Vulkan rendering surface on iOS or macOS, respectively.

You can enable each of these extensions by defining the VK_USE_PLATFORM_IOS_MVK or VK_USE_PLATFORM_MACOS_MVK guard macro in your compiler build settings. See the description of the mvk_vulkan.h file below for a convenient way to enable these extensions automatically.
#endif

template<>
struct DeviceExtensionTraits<"VK_MVK_ios_surface"> {
    using CppType = MolteniOSSurfaceDeviceExtension;
};

class MolteniOSSurfaceDeviceExtension: public StaticDeviceExtension<"VK_MVK_ios_surface"> {};

template<>
struct DeviceExtensionTraits<"VK_MVK_macos_surface"> {
    using CppType = MoltenMacOSSurfaceDeviceExtension;
};

class MoltenMacOSSurfaceDeviceExtension: public StaticDeviceExtension<"VK_MVK_macos_surface"> {};


template<>
struct DeviceExtensionTraits<VK_MVK_MOLTENVK_EXTENSION_NAME> {
    using CppType = MoltenVkDeviceExtension;
};

class MoltenVkDeviceExtension: public StaticDeviceExtension<VK_MVK_MOLTENVK_EXTENSION_NAME> {
public:

    static constexpr Version    cVersion            = MVK_VERSION;
    static constexpr uint32_t   cSpecVersion        = VK_MVK_MOLTENVK_SPEC_VERSION;
    static constexpr size_t     cVersionBufferSize  = 256;

    template<typename Struct>
    class VariableLengthStruct: public elysian::renderer::VariableLengthStruct<Struct>
    {
    public:
        using Parent = elysian::renderer::VariableLengthStruct<Struct>;
        using Parent::VariableLengthStruct;
        using Parent::operator =;

        size_t getCopiedSize(void) const { return m_copiedSize; }
        void   setCopiedSize(size_t size) {  m_copiedSize = size; }
        bool   isPopulated(void) const { return isValid() && getCopiedSize(); }
    private:
        size_t m_copiedSize = 0;
    };

    MoltenVkDeviceExtension(Instance* pInstance) {
        _queryVersionStrings();
        _queryConfiguration();
    }

    const char* getMoltenVersionString(void) const { return m_moltenVersionString; }
    const char* getVulkanVersionString(void) const { return m_vulkanVersionString; }

    ResultValue<MVKConfiguration> getConfiguration(void) const {
        Result result = vk::Result::eSuccess;
        if(!m_config.isPopulated()) {
            result.clear();
        }
        return std::pair(result, m_config);
    }

    Result setConfiguration(const MVKConfiguration& config) {
        m_config = config;
        return _queryVariableLengthGeneric(m_config,  vkSetMoltenVKConfigurationMVK, *getInstance()).first;
    }

    ResultValue<MVKPhysicalDeviceMetalFeatures> getPhysicalDeviceMetalFeatures(PhysicalDevice* pPhysicalDev) const {
        VariableLengthStruct<MVKPhysicalDeviceMetalFeatures> features;
        return _queryVariableLengthGeneric(features, vkGetPhysicalDeviceMetalFeaturesMVK, *physicalDev);
    }

    ResultValue<MVKPerformanceStatistics> getDevicePerformanceStatistics(Device* pDevice)  const {
        VariableLengthStruct<MVKPerformanceStatistics> statistics;
        return _queryVariableLengthGeneric(statistics, vkGetPerformanceStatisticsMVK, *pDevice);
    }

    void setShaderWorkGroupSize(ShaderModule* pShaderModule, uint32_t x, uint32_t y, uint32_t z) const {
        vkSetWorkgroupSizeMVK(*pShaderModule, x, y, z);
    }

    void log(DebugLog* pLog) const;
    void log(const  MVKConfiguration& config, DebugLog* pLog) const;
    void log(const MVKPhysicalDeviceMetalFeatures& features, DebugLog* pLog) const;
    void log(const MVKPerformanceStatistics& stats, DebugLog* pLog) const;

protected:

    void _queryVersionStrings(void) const {
        vkGetVersionStringsMVK(m_moltenVersionString, sizeof(m_moltenVersionString),
                               m_vulkanVersionString, sizeof(m_vulkanVersionString));
    }

    Result _queryConfiguration(void) {
        return _queryVariableLengthGeneric(m_config, vkGetMoltenVKConfigurationMVK, *getInstance()).first;
    }

    template<typename Struct, typename Func, typename Handle>
    ResultValue<T> _queryVariableLengthGeneric(VariableLengthStruct<Struct>& variableStruct,
                                               Func&& function,
                                               const Handle& handle)
    {
        size_t expectedSize = 0;
        Result result = function(handle, nullptr, &expectedSize);
        if(result) {
            variableStruct.resize(expectedSize);
            result = function(handle, variableStruct, &expectedSize);
            variableStruct.setCopiedSize(size);
        }
        return std::pair(result, variableStruct);
    }

    VKAPI_ATTR VkResult VKAPI_CALL vkGetMoltenVKConfigurationMVK(
        VkInstance                                  instance,
        MVKConfiguration*                           pConfiguration,
        size_t*                                     pConfigurationSize);

    VKAPI_ATTR VkResult VKAPI_CALL vkSetMoltenVKConfigurationMVK(
        VkInstance                                  instance,
        const MVKConfiguration*                     pConfiguration,
        size_t*                                     pConfigurationSize);

    VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceMetalFeaturesMVK(
        VkPhysicalDevice                            physicalDevice,
        MVKPhysicalDeviceMetalFeatures*             pMetalFeatures,
        size_t*                                     pMetalFeaturesSize);

    VKAPI_ATTR VkResult VKAPI_CALL vkGetPerformanceStatisticsMVK(
        VkDevice                                    device,
        MVKPerformanceStatistics*            		pPerf,
        size_t*                                     pPerfSize);


    VKAPI_ATTR void VKAPI_CALL vkGetVersionStringsMVK(
        char*                                       pMoltenVersionStringBuffer,
        uint32_t                                    moltenVersionStringBufferLength,
        char*                                       pVulkanVersionStringBuffer,
        uint32_t                                    vulkanVersionStringBufferLength);

    VKAPI_ATTR void VKAPI_CALL vkSetWorkgroupSizeMVK(
        VkShaderModule                              shaderModule,
        uint32_t                                    x,
        uint32_t                                    y,
        uint32_t                                    z);

private:
    VariableLengthStruct<MVKConfiguration>  m_config;

    char               m_moltenVersionString[cVersionBufferSize]    = '\0';
    char               m_vulkanVersionString[cVersionBufferSize]    = '\0';

};














#endif // ELYSIAN_RENDERER_EXTENSIONS_HPP
