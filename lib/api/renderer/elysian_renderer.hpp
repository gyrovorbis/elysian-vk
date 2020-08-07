#ifndef ELYSIAN_RENDERER_HPP
#define ELYSIAN_RENDERER_HPP

#include <initializer_list>
#include <vector>



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

class Instance;
class InstanceInitializer;
class Device;
class DeviceInitializer;
class PhysicalDevice;
class Allocator;
class ShaderModuleCache;
class InstanceLayerProperties;
class InstanceExtensionProperties;

class DynamicSettings {
public:
    //multisampling, anti aliasing, mip mapping, shadow map size,
    //depth/color buffer size
};

// Wrap to debugExt shit
class DebugLogIface {
public:
    enum class Severity {
        Verbose,
        Info,
        Warning,
        Error
    };

    enum class Source {
        Renderer,
        Driver_General,
        Driver_Validation,
        Driver_Performance
    };

    virtual void push(void) = 0;
    virtual void pop(void) = 0;
    virtual void write(Source source, Severity severity, const char* pBuffer) = 0;

    void pop(unsigned count);

    void logf(Source source, Severity severity, const char* pFmt, ...);
    void log(Source source, Severity severity, const char *pFmt, va_list vaList);
};

//Lowest-level render shit, not really used directly by game (Use RenderSystem instead!)
//Need to take a top-level log for writing shit independently of validation layers...
class Renderer {
    public:

        struct Initializer {
            const Allocator* pAllocator = nullptr;
            const InstanceInitializer* pInstanceInitializer;
            std::initializer_list<const DeviceInitializer*> deviceInitializers;
        };

        Renderer(Initializer initializer);
        ~Renderer(void);

        bool isValid(void) const; //m_apiInstance created successfully

        void setAllocator(const Allocator* pAllocator);
        const Allocator& getAllocator(void) const;

        const Instance* getInstance(void) const;
        const InstanceLayerProperties* getInstanceLayerProperties(void) const;
        const InstanceExtensionProperties* getInstanceExtensionProperties(void) const;

        const PhysicalDevice* getPhysicalDevice(int index) const;

        const Device* getDevice(int index) const;
        const Device* getDevice(const char* pName) const;

        //PhysicalDevice* selectPhysicalDevice(const PhysicalDeviceSelector& selector) const;
        //select memory heap/type and command pool/type

    private:
        bool initialize(const Initializer& initializer);
        bool queryInstanceLayerProperties(void);
        bool queryInstnaceExtensionProperties(void);
        bool createInstance(const InstanceInitializer& initializer);
        bool queryPhysicalDevices(void);
        Device* createDevice(const DeviceInitializer& initializer);

        //Debug log level
        std::unique_ptr<Allocator>                      m_pAllocator;

        std::unique_ptr<InstanceExtensionProperties>    m_pInstanceExtensionProperties;
        std::unique_ptr<InstanceLayerProperties>        m_pInstanceLayerProperties;
        std::unique_ptr<Instance>                       m_pInstance;

        std::unique_ptr<std::vector<PhysicalDevice>> 	m_pPhysicalDevices;
        std::vector<std::unique_ptr<Device*>>           m_devices;

        std::unique_ptr<DynamicSettings>				m_pDynamicSettings;
        //pipeline cache
        std::unique_ptr<ShaderModuleCache>              m_pShaderModuleCache;

        friend std::ostream& operator<<(std::ostream& os, const Renderer& render);

};


} // namespace elysian::renderer

#endif // ELYSIAN_RENDERER_HPP
