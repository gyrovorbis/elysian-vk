#include <renderer/elysian_renderer_instance.hpp>
#include <renderer/elysian_renderer.hpp>
#include <renderer/elysian_renderer_debug_messenger.hpp>
#include <renderer/elysian_renderer_debug_log.hpp>

namespace elysian::renderer {


Instance::Instance(InstanceInitializer initializer, Renderer* pRenderer):
    m_pRenderer(pRenderer) {
#if 1
    insertRequiredLayers(initializer);
    insertRequiredExtensions(initializer);
    // Required to enable logging for create/destroy instance methods.
    if(initializer.pDebugCreateInfo) {
        if(!initializer.pDebugCreateInfo->pfnUserCallback) {
            initializer.pDebugCreateInfo->setLogger([=](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData){
                m_pRenderer->getLog()->debugUtilsMessage(messageSeverity, messageType, pCallbackData);
            });
        }
        initializer.info.pNext = initializer.pDebugCreateInfo;
    }
#endif

    pRenderer->getLog()->verbose("Creating Instance");
    pRenderer->getLog()->push();
    VkInstance instance;
    m_result = vkCreateInstance(&initializer.info, nullptr, &instance);
    setHandle(instance);
    //setObjectName("INSTANCEY");

    if(!m_result) {
        pRenderer->getLog()->error("Failed: %s", m_result.toString());
    }

#if 1
    if(initializer.pDebugCreateInfo) {
        m_dbgMessengerEXT = std::make_unique<DebugUtilsMessengerEXT>(initializer.pDebugCreateInfo, this);
    }
#endif


    pRenderer->getLog()->pop();
}

PFN_vkVoidFunction Instance::getProcAddr(const char* pName) const {
    return vkGetInstanceProcAddr(getHandle(), pName);
}

void Instance::insertRequiredExtensions(InstanceInitializer &initializer) const {
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

    checkInsertExtension(initializer.pDebugCreateInfo, "VK_EXT_debug_utils");
}

void Instance::insertRequiredLayers(InstanceInitializer &initializer) const {

}

const DebugUtilsMessengerEXT* Instance::getDebugUtilsMessengerEXT(void) const {
//    return m_dbgMessengerEXT.get();
    return nullptr;
}

Instance::~Instance(void) {
    m_dbgMessengerEXT.reset(nullptr);
    m_pRenderer->getLog()->verbose("Destroying Instance");
    vkDestroyInstance(getHandle(), nullptr);
}

bool Instance::isValid(void) const {
    return HandleObject<VkInstance, VK_OBJECT_TYPE_INSTANCE>::isValid() && getResult().succeeded();
}

Result Instance::getResult(void) const { return m_result; }

InstanceExtensionProperties::InstanceExtensionProperties(const char* pLayerName) {
    uint32_t extensionCount = 0;
    m_result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
   if(m_result) {
       m_extensionProperties.resize(extensionCount);
       m_result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_extensionProperties.data());
   }
}

void InstanceExtensionProperties::log(DebugLog* pLog) const {
    pLog->verbose("Instance Extension Properties");
    pLog->push();

    if(isValid()) {
        for(int p = 0; p < m_extensionProperties.size(); ++p) {
            pLog->verbose("[%d]: %s", p, m_extensionProperties[p].extensionName);
            pLog->push();
            pLog->verbose("specVersion: %s", Version(m_extensionProperties[p].specVersion).toString().c_str());
            pLog->pop();
        }

    } else {
        pLog->error("Query Failed: %s", m_result.toString());
    }

    pLog->pop();
}

bool InstanceExtensionProperties::isValid(void) const {
    return m_result.succeeded();
}

Result InstanceExtensionProperties::getResult(void) const { return m_result; }

InstanceLayerProperties::InstanceLayerProperties(void) {
    uint32_t layerCount;
    m_result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if(m_result) {
        m_layerProperties.resize(layerCount);
        m_result = vkEnumerateInstanceLayerProperties(&layerCount, m_layerProperties.data());
    }
}

bool InstanceLayerProperties::isValid(void) const {
    return m_result.succeeded();
}

Result InstanceLayerProperties::getResult(void) const { return m_result; }


void InstanceLayerProperties::log(DebugLog* pLog) const {
    pLog->verbose("Instance Layer Properties");
    pLog->push();

    if(isValid()) {
        for(int p = 0; p < m_layerProperties.size(); ++p) {
            pLog->verbose("[%d]: %s", p, m_layerProperties[p].layerName);
            pLog->push();
            pLog->verbose("specVersion: %s", Version(m_layerProperties[p].specVersion).toString().c_str());
            pLog->verbose("implementationVersion: %s", Version(m_layerProperties[p].implementationVersion).toString().c_str());
            pLog->verbose("description: %s", m_layerProperties[p].description);
            pLog->pop();
        }

    } else {
        pLog->error("Query Failed: %s", m_result.toString());
    }

    pLog->pop();
}

InstanceCreateInfo::InstanceCreateInfo(ApplicationInfo info, std::vector<const char*> layers, std::vector<const char*> extensions):
    m_appInfo(std::move(info)),
    m_layers(std::move(layers)),
    m_extensions(std::move(extensions)),
    VkInstanceCreateInfo({
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,  //flags
        &m_appInfo,
        static_cast<uint32_t>(layers.size()),
        layers.data(),
        static_cast<uint32_t>(extensions.size()),
        extensions.data()
    })
{}

const std::vector<const char*>& InstanceCreateInfo::getLayers(void) const { return m_layers; }
const std::vector<const char*>& InstanceCreateInfo::getExtensions(void) const { return m_extensions; }

void InstanceCreateInfo::addExtension(const char* pName) {
    m_extensions.push_back(pName);
    enabledExtensionCount = m_extensions.size();
    ppEnabledExtensionNames = m_extensions.data();
}

void InstanceCreateInfo::addLayer(const char* pName) {
    m_layers.push_back(pName);
    enabledLayerCount = m_layers.size();
    ppEnabledLayerNames = m_layers.data();
}

VKAPI_ATTR
VkBool32 VKAPI_CALL DebugUtilsMessengerEXTCreateInfo::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                  void* pUserData)
{
    auto logger = reinterpret_cast<DebugUtilsMessengerEXTCreateInfo*>(pUserData)->getLogger();
    if(logger) {
        logger(messageSeverity, messageType, pCallbackData);
    }
    return VK_FALSE;
}

void Instance::submitMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData) {
    vkSubmitDebugUtilsMessageEXT(getHandle(), messageSeverity, messageTypes, pCallbackData);
}


}
