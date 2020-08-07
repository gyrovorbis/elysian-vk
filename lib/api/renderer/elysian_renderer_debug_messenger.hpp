#ifndef ELYSIAN_RENDERER_DEBUG_MESSENGER_HPP
#define ELYSIAN_RENDERER_DEBUG_MESSENGER_HPP


#include <functional>

namespace elysian::renderer {
// set up extension on Instance create info
// create and destroy DebugUtilsMessengerEXT on Instance
// pass create flags to instance creation/deletion

using DebugUtilsMessengerLogger =
    std::function<void(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)>;


class DebugUtilsMessengerEXT {
public:
    class CreateInfo: public VkDebugUtilsMessengerCreateInfoEXT {
        CreateInfo(VkDebugUtilsMessageSeverityFlagsEXT messageSeverity =
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                    VkDebugUtilsMessageTypeFlagsEXT messageType =
                        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT):
            VkDebugUtilsMessengerCreateInfoEXT({
                VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                nullptr,
                0,
                messageSeverity,
                messageType,
                &debugCallback,
                nullptr //populated later!
            })
        {}
    };

    struct Initializer {
        CreateInfo info;
        DebugUtilsMessengerLogger logger;
        Instance* pInstance;
    };

    DebugUtilsMessengerEXT(Initializer initializer);
    ~DebugUtilsMessengerEXT(void);

    const DebugUtilsMessengerLogger& getLogger(void) const { return m_logger; }
    const CreateInfo& getCreateInfo(void) const;

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

    static VKAPI_ATTR
    VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                      void* pUserData);

private:
    CreateInfo                  m_createInfo;
    VkDebugUtilsMessengerEXT    m_handle = VK_NULL_HANDLE;
    DebugUtilsMessengerLogger   m_logger;
    Instance*                   m_pInstance = nullptr;
    Result                      m_result;
};

inline DebugUtilsMessengerEXT::DebugUtilsMessengerEXT(Initializer initializer):
    m_createInfo(std::move(initializer.info)),
    m_logger(std::move(initializer.logger)),
    m_pInstance(initializer.pInstance)
{
    initializer.info.pUserData = &logger;
    m_result = createDebugUtilsMessengerEXT(m_pInstance, &m_createInfo, nullptr, &m_handle);
}

inline DebugUtilsMessengerEXT::~DebugUtilsMessengerEXT(void) {
    destroyDebugUtilsMessengerEXT(m_pInstance, m_handle, nullptr);
}

inline const DebugUtilsMessengerLogger& DebugUtilsMessengerEXT::getLogger(void) const { return m_logger; }
inline auto DebugUtilsMessengerEXT::getCreateInfo(void) const -> const CreateInfo& { return m_createInfo; }

inline VkResult DebugUtilsMessengerEXT::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const
VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = static_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance,
                                                                "vkCreateDebugUtilsMessengerEXT"));
    if (func) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }else{
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

inline void DebugUtilsMessengerEXT::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = static_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance,
                                                                                       "vkDestroyDebugUtilsMessengerEXT"));
    if (func) {
        func(instance, debugMessenger, pAllocator);
    }
}

VKAPI_ATTR
VkBool32 VKAPI_CALL DebugUtilsMessengerEXT::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                  void* pUserData)
{
    auto logger = reinterpret_cast<DebugUtilsMessengerEXT*>(pUserData)->getLogger();
    if(logger) {
        logger(messageSeverity, messageType, pCallbackData);
    } else {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}



#endif // ELYSIAN_RENDERER_DEBUG_MESSENGER_HPP
