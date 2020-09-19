#ifndef ELYSIAN_RENDER_INSTANCE_HPP
#define ELYSIAN_RENDER_INSTANCE_HPP

#include "elysian_renderer_result.hpp"
#include "elysian_renderer_object.hpp"
#include <string>
#include <optional>
#include <vector>

namespace elysian::renderer {

class Renderer;
class DebugLog;
class Allocator;
class DebugUtilsMessengerEXT;
class DebugUtilsMessengerEXTInitializer;
class DebugUtilsMessengerEXTCreateInfo;

class InstanceProcAddrCache: public ProcAddrCache {
public:
    InstanceProcAddrCache(Instance* pInstance):
        m_pInstance(pInstance) {}

    virtual PFN_vkVoidFunction getProcAddr(const char* pName) override {
        return vkGetInstanceProcAddr(m_pInstance->getHandle(), pName);
    }

private:
    Instance* m_pInstance = nullptr;
};

class InstanceExtensionProperties {
public:
    InstanceExtensionProperties(const char* pLayerName=nullptr);

    auto getProperties(void) const -> const std::vector<VkExtensionProperties>& { return m_extensionProperties; }
    const VkExtensionProperties* findProperty(const char* pName) const;
    bool supportsExtensions(std::vector<const char*> pNames) const;
    const char* getLayerName(void) const;
    Result getResult(void) const;
    bool isValid(void) const;
    void log(DebugLog* pLog) const;

private:
    std::string m_layerName;
    std::vector<VkExtensionProperties> m_extensionProperties;
    Result m_result;
};

class InstanceLayerProperties {
public:
    InstanceLayerProperties(void);

    auto getProperties(void) const -> const std::vector<VkLayerProperties>& { return m_layerProperties; }
    const VkLayerProperties* findProperty(const char* pName) const;
    bool supportsLayers(std::vector<const char*> pNames) const;
    Result getResult(void) const;
    bool isValid(void) const;

    void log(DebugLog* pLog) const;

private:
    std::vector<VkLayerProperties> m_layerProperties;
    Result m_result;
};

class ApplicationInfo: public VkApplicationInfo {
public:
    ApplicationInfo(const char* pApplicationName = "None",
                    Version applicationVersion   = 0,
                    const char* pEngineName      = "Elysian Shadows",
                    Version pEngineVersion       = 0,
                    Version apiVersion           = Version(1, 0, 0)):
        VkApplicationInfo({
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            pApplicationName,
            applicationVersion,
            pEngineName,
            pEngineVersion,
            apiVersion
        }) {}
};

class InstanceCreateInfo: public VkInstanceCreateInfo {
public:

    InstanceCreateInfo(ApplicationInfo info={}, std::vector<const char*> layers={}, std::vector<const char*> extensions={});

    const std::vector<const char*>& getLayers(void) const;
    const std::vector<const char*>& getExtensions(void) const;

    void addExtension(const char* pName);
    void addLayer(const char* pName);



private:
    ApplicationInfo          m_appInfo;
    std::vector<const char*> m_layers;
    std::vector<const char*> m_extensions;

};

struct InstanceInitializer {
    InstanceCreateInfo info;
    DebugUtilsMessengerEXTCreateInfo* pDebugCreateInfo;
};

class FileSystemProperties {
public:
    std::string getPath(const char* pSubDir);
    const char* getRootDir(void) const;
private:
    std::string m_rootDir;
};

// Default file path shit?
// Filesystem object type for loading and shit?
class Instance: public HandleObject<VkInstance, VK_OBJECT_TYPE_INSTANCE> {
    friend class Renderer;
public:

    Instance(InstanceInitializer initializer, Renderer* pRenderer);
    virtual ~Instance(void);

    virtual bool isValid(void) const override final;

    const DebugUtilsMessengerEXT* getDebugUtilsMessengerEXT(void) const;

    Result getResult(void) const;

    PFN_vkVoidFunction getProcAddr(const char* pName) const;

protected:
    Instance(const InstanceCreateInfo& info, Renderer& renderer);

    void insertRequiredExtensions(InstanceInitializer& initializer) const;
    void insertRequiredLayers(InstanceInitializer& initializer) const;

    void submitMessage(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);


private:
    std::unique_ptr<DebugUtilsMessengerEXT> m_dbgMessengerEXT;
    Renderer*                               m_pRenderer = nullptr;
    Result                                  m_result;
    ShaderModuleCache*                      m_pShaderModuleCache = nullptr;
    InstanceProcAddrCache m_procAddrCache; // start fetching everything through this bitch
};

}

#endif // ELYSIAN_RENDER_INSTANCE_HPP
