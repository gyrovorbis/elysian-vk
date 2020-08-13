#ifndef ELYSIAN_RENDERER_HPP
#define ELYSIAN_RENDERER_HPP

#include <initializer_list>
#include <vector>

#include "elysian_renderer_result.hpp"


#if 0

TODUZ
1) All heavy value-type shit needs move semantics!






// GENERIC CONTAINER CONVERTER SHIT

std::vector<const WrapperType*> -> std::vector<WrappedType>


class ContainerUnwrapper {
public:
    template<template<typename> typename C,
             typename Wrapped,
             typename Wrapper = C::value_type>
    static auto unwrap(const C<Wrapper>& wrappers) {
        C<Wrapped> wrapped;
        wrapped.reserve(wrappers.size());
        for(const auto&& wrapper : wrappers) {
            wrapped.push_back(Converter(wrapper));
        }
        return wrapped;
    }

}


std::vector<CommandBuffer> buffers -> std::vector<VkBuffer>
ContainerUnwrapper::unwrap<(buffers)

#endif

namespace elysian::renderer {

class DebugLog;
class Instance;
class InstanceInitializer;
class Device;
class DeviceCreateInfo;
class PhysicalDevice;
class PhysicalDeviceGroup;
class Allocator;
class ShaderModuleCache;
class InstanceLayerProperties;
class InstanceExtensionProperties;

class DynamicSettings {
public:
    //multisampling, anti aliasing, mip mapping, shadow map size,
    //depth/color buffer size
};

//Lowest-level render shit, not really used directly by game (Use RenderSystem instead!)
//Need to take a top-level log for writing shit independently of validation layers...
class Renderer {
    public:

        struct Initializer {
            DebugLog*       pLog = nullptr;
            const Allocator* pAllocator = nullptr;
            const InstanceInitializer* pInstanceInitializer;
            //std::initializer_list<const DeviceInitializer*> deviceInitializers;
        };

        Renderer(Initializer* initializer);
        ~Renderer(void);

        static std::pair<Result, Version> getInstanceVersion(void);

        bool isValid(void) const; //m_apiInstance created successfully

        void setAllocator(const Allocator* pAllocator);
        const Allocator& getAllocator(void) const;

        const Instance* getInstance(void) const;
        const InstanceLayerProperties* getInstanceLayerProperties(void) const;
        const InstanceExtensionProperties* getInstanceExtensionProperties(void) const;

        const PhysicalDevice* getPhysicalDevice(int index) const;

        const Device* getDevice(int index) const;
        const Device* getDevice(const char* pName) const;
        Device* createDevice(const char* pName, const PhysicalDevice* pDevice, std::shared_ptr<const DeviceCreateInfo> pCreateInfo);

        DebugLog* getLog(void) const;

        //PhysicalDevice* selectPhysicalDevice(const PhysicalDeviceSelector& selector) const;
        //select memory heap/type and command pool/type



    private:
        bool initialize(const Initializer& initializer);
        bool queryInstanceLayerProperties(void);
        bool queryInstanceExtensionProperties(void);
        bool createInstance(const InstanceInitializer& initializer);
        bool queryPhysicalDevices(void);
        bool queryPhysicalDeviceGroups(void);

        //Debug log level
        //std::unique_ptr<Allocator>                      m_pAllocator;

        DebugLog*                                       m_pLog = nullptr;

        std::unique_ptr<InstanceExtensionProperties>    m_pInstanceExtensionProperties;
        std::unique_ptr<InstanceLayerProperties>        m_pInstanceLayerProperties;
        std::unique_ptr<Instance>                       m_pInstance;

        std::unique_ptr<std::vector<PhysicalDevice>> 	m_pPhysicalDevices;
        std::unique_ptr<std::vector<PhysicalDeviceGroup>>
                                                        m_pPhysicalDeviceGroups;
        std::unique_ptr<std::vector<Device>>            m_pDevices;

        std::unique_ptr<DynamicSettings>				m_pDynamicSettings;
        //pipeline cache
        //std::unique_ptr<ShaderModuleCache>              m_pShaderModuleCache;

        friend std::ostream& operator<<(std::ostream& os, const Renderer& render);


};

inline DebugLog* Renderer::getLog(void) const { return m_pLog; }
inline const Instance* Renderer::getInstance(void) const { return m_pInstance.get(); }


} // namespace elysian::renderer

#endif // ELYSIAN_RENDERER_HPP
