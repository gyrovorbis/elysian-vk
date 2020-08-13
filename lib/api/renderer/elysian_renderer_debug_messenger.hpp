#ifndef ELYSIAN_RENDERER_DEBUG_MESSENGER_HPP
#define ELYSIAN_RENDERER_DEBUG_MESSENGER_HPP


#include <functional>

namespace elysian::renderer {
// set up extension on Instance create info
// create and destroy DebugUtilsMessengerEXT on Instance
// pass create flags to instance creation/deletion

//this is just the top-level shit, shouldn't be used directly
//instance uses this,
#if 0
Renderer* pRenderer = new Renderer(new Renderer::Initializer{
                                       new QtLog(),
                                       nullptr, //allocator
                                       new InstanceInitializer {
                                           InstanceCreateInfo = {
                                               layers = {},
                                               extensions = {}
                                           },
                                           debugMessages = new DebugCreateInfo { //nullptr then no debuggage
                                               source = BLAH,
                                               severity = BLAH
                                                callback = nullptr; //use parent
                                           }
                                       }
                                       //device shit
                                   });

#endif
using DebugUtilsMessengerLogger =
    std::function<void(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)>;

class DebugUtilsMessengerEXTCreateInfo: public VkDebugUtilsMessengerCreateInfoEXT {
public:
    DebugUtilsMessengerEXTCreateInfo(VkDebugUtilsMessageSeverityFlagsEXT messageSeverity =
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                VkDebugUtilsMessageTypeFlagsEXT messageType =
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                PFN_vkDebugUtilsMessengerCallbackEXT callback=nullptr,
                void* pUserData=nullptr);

    DebugUtilsMessengerEXTCreateInfo(VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
        DebugUtilsMessengerLogger logger):
        DebugUtilsMessengerEXTCreateInfo(messageSeverity, messageType, nullptr, nullptr)
    {
        setLogger(std::move(logger));
    }

    void setLogger(DebugUtilsMessengerLogger logger) {
        m_pLogger = std::move(logger);
        pfnUserCallback = &DebugUtilsMessengerEXTCreateInfo::debugCallback;
        pUserData = this;
    }

    DebugUtilsMessengerLogger getLogger(void) const { return m_pLogger; }

protected:
    static VKAPI_ATTR
    VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                      void* pUserData);

private:
    DebugUtilsMessengerLogger m_pLogger;
};

struct DebugUtilsMessengerEXTInitializer {
    DebugUtilsMessengerEXTCreateInfo info;
};


class DebugUtilsMessengerEXT {
public:

    DebugUtilsMessengerEXT(DebugUtilsMessengerEXTCreateInfo* pInfo, Instance* pInstance);
    ~DebugUtilsMessengerEXT(void);

    const DebugUtilsMessengerLogger& getLogger(void) const;
    const DebugUtilsMessengerEXTCreateInfo& getCreateInfo(void) const;

    bool isValid(void) const;
    Result getResult(void) const;

protected:

    static VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                 const VkAllocationCallbacks* pAllocator,
                                                 VkDebugUtilsMessengerEXT* pDebugMessenger);

    static void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                              VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks* pAllocator);

private:
    DebugUtilsMessengerEXTCreateInfo*                  m_pCreateInfo;
    VkDebugUtilsMessengerEXT    m_handle = VK_NULL_HANDLE;
    Instance*                   m_pInstance = nullptr;
    Result                      m_result;
};

inline DebugUtilsMessengerEXT::DebugUtilsMessengerEXT(DebugUtilsMessengerEXTCreateInfo* pInfo, Instance* pInstance):
    m_pCreateInfo(pInfo),
    m_pInstance(pInstance)
{
    m_result = createDebugUtilsMessengerEXT(m_pInstance->getHandle(), m_pCreateInfo, nullptr, &m_handle);
}

inline DebugUtilsMessengerEXT::~DebugUtilsMessengerEXT(void) {
    destroyDebugUtilsMessengerEXT(m_pInstance->getHandle(), m_handle, nullptr);
}

inline auto DebugUtilsMessengerEXT::getCreateInfo(void) const -> const DebugUtilsMessengerEXTCreateInfo& { return *m_pCreateInfo; }

inline VkResult DebugUtilsMessengerEXT::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance,
                                                                "vkCreateDebugUtilsMessengerEXT"));
    if (func) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }else{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

inline void DebugUtilsMessengerEXT::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance,
                                                                                       "vkDestroyDebugUtilsMessengerEXT"));
    if (func) {
        func(instance, debugMessenger, pAllocator);
    }
}


inline DebugUtilsMessengerEXTCreateInfo::DebugUtilsMessengerEXTCreateInfo(VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                PFN_vkDebugUtilsMessengerCallbackEXT callback,
                void* pUserData):
        VkDebugUtilsMessengerCreateInfoEXT({
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            nullptr,
            0,
            messageSeverity,
            messageType,
            callback,
            pUserData
        })
    {}

}



#endif // ELYSIAN_RENDERER_DEBUG_MESSENGER_HPP
