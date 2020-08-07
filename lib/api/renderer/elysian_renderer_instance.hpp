#ifndef ELYSIAN_RENDER_INSTANCE_HPP
#define ELYSIAN_RENDER_INSTANCE_HPP

namespace elysian::render {

class Allocator;
class DebugUtilsMessengerEXT;
class DebugUtilsMessengerEXT::Initializer;

class InstanceExtensionProperties {
public:
    InstanceExtensionProperties(const char* pLayerName=nullptr);

    auto& getProperties(void) const -> const std::vector<VkExtensionProperties> { return m_extensionProperties; }
    const VkExtensionProperties* findProperty(const char* pName) const;
    bool supportsExtensions(std::vector<const char*> pNames) const;
    const char* getLayerName(void) const;
    Result getResult(void) const;
    bool isValid(void) const;

private:
    std::string m_layerName;
    std::vector<VkExtensionProperties> m_extensionProperties;
    Result m_result;
};

class InstanceLayerProperties {
public:
    InstanceLayerProperties(void);

    auto& getProperties(void) const -> const std::vector<VkLayerProperties> { return m_layerProperties; }
    const VkLayerProperties* findProperty(const char* pName) const;
    bool supportsLayers(std::vector<const char*> pNames) const;
    Result getResult(void) const;
    bool isValid(void) const;

private:
    std::vector<VkLayerProperties> m_layerProperties;
    Result m_result;
};

class Version {
public:
    constexpr Version(uint16_t major, uint16_t minor, uint16_t patch):
        Version(VK_MAKE_VERSION(major, minor, patch)) {}

    constexpr Version(uint32_t version): m_value(version) {}

    operator uint32_t() const { return getValue(); }

    uint32_t getValue(void) const { return m_value; }
    uint16_t getMajor(void) const;
    uint16_t getMinor(void) const;
    uint16_t getPatch(void) const;

private:
    uint32_t m_value = 0;
};

class ApplicationInfo: public VkApplicationInfo {
public:
    ApplicationInfo(const char* pApplicationName = "None",
                    Version applicationVersion   = 0,
                    const char* pEngineName      = "Elysian Shadows",
                    Version pEngineVersion       = 0):
        VkApplicationInfo({
            pApplicationName,
            applicationVersion,
            pEngineName,
            pEngineVersion
        }) {}
};

class Instance {
    friend class Renderer;
public:

    class CreateInfo: public VkInstanceCreateInfo {
    public:

        CreateInfo(ApplicationInfo info={}, std::vector<const char*> layers={}, std::vector<const char*> extensions={});

        const std::vector<const char*>& getLayers(void) const;
        const std::vector<const char*>& getExtensions(void) const;

        void addExtension(const char* pName);
        void addLayer(const char* pName);

    private:
        ApplicationInfo          m_appInfo;
        std::vector<const char*> m_layers;
        std::vector<const char*> m_extensions;

    };

    struct Initializer {
        CreateInfo info;
        std::optional<DebugMessengerUtilsEXT::Initializer> debugMessageEXTInitializer;
    };

    Instance(Initializer initializer, const Allocator* pAllocator=nullptr);
    ~Instance(void);

    const DebugUtilsMessengerEXT* getDebugUtilsMessengerEXT(void) const;

    bool isValid(void);
    Result getResult(void) const;

protected:
    Instance(const CreateInfo& info, Renderer& renderer);

    void insertRequiredExtensions(Initializer& initializer) const;
    void insertRequiredLayers(Initializer& initializer) const;


private:
    std::unique_ptr<DebugUtilsMessengerEXT> m_dbgMessengerEXT;
    VkInstance                              m_instance = VK_NULL_HANDLE;
    Result                                  m_result;
};

inline Instance::Instance(Initializer initializer) {
    insertRequiredLayers(initializer);
    insertRequiredExtensions(initializer);
    // Required to enable logging for create/destroy instance methods.
    if(initializer.debugMessageEXTInitializer) {
        initializer.info.pNext = &initializer.debugMessageEXTInitializer.value().info;
    }

    m_result = vkCreateInstance(&initializer.info, nullptr, &m_instance);

    if(initializer.debugMessageEXTInitializer) {
        initializer.debugMessageEXTInitializer.value().pInstance = this;
        m_dbgMessengerEXT = new DebugUtilsMessengerEXT(initializer.debugMessageEXTInitializer.value());
    }
}

inline void Instance::insertRequiredExtensions(Initializer &initializer) const {
    auto checkInsertExtension = [&](bool conditional, const char* pExtension) {
        if(conditional) {
            bool foundExtension = false;
            for(auto&& ext : initializer.info.getExtensions()) {
                if(strcmp(ext, pExtension)) {
                    foundExtension = true;
                    break;
                }
            }

            if(!foundExtension) {
                initializer.info.addExtension(pExtension);
            }
        }
    };

    checkInsertExtension(initializer.debugMessageEXTInitializer, "VK_EXT_debug_utils");
}

inline void Instance::insertRequiredLayers(Initializer &initializer) const {

}

inline const DebugUtilsMessengerEXT* Instance::getDebugUtilsMessengerEXT(void) const { return m_dbgMessengerEXT; }

inline Instance::~Instance(void) {
    vkDestroyInstance(m_instance, nullptr);
}

inline InstanceExtensionProperties::InstanceExtensionProperties(const char* pLayerName=nullptr) {
    uint32_t extensionCount = 0;
    m_result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
   if(m_result) {
       m_extensionProperties.reserve(extensionCount);
       m_result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
   }
}

inline InstanceLayerProperties::InstanceLayerProperties(void) const {
    uint32_t layerCount;
    m_result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if(m_result) {
        m_layerProperties.reserve(layerCount);
        m_result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    }
}

inline Instance::CreateInfo::CreateInfo(ApplicationInfo info, std::vector<const char*> layers, std::vector<const char*> extensions):
    m_appInfo(std::move(info)),
    m_layers(std::move(layers)),
    m_extensions(std::move(extensions)),
    VkInstanceCreateInfo({
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,  //flags
        &info,
        layers.size(),
        layers.data(),
        extensions.size(),
        extensions.data()
    })
{}

inline const std::vector<const char*>& Instance::CreateInfo::getLayers(void) const { return m_layers; }
inline const std::vector<const char*>& Instance::CreateInfo::getExtensions(void) const { return m_extensions; }

inline void Instance::CreateInfo::addExtension(const char* pName) {
    m_extensions.push_back(pName);
    enabledExtensionCount = m_extensions.size();
    ppEnabledExtensionNames = m_extensions.data();
}

inline void Instance::CreateInfo::addLayer(const char* pName) {
    m_layers.push_back(pName);
    enabledLayerCount = m_layers.size();
    ppEnabledLayerNames = m_layers.data();
}


}

#endif // ELYSIAN_RENDER_INSTANCE_HPP
