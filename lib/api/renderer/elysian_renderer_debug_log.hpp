#ifndef ELYSIAN_MESSENGER_DEBUG_LOG_HPP
#define ELYSIAN_MESSENGER_DEBUG_LOG_HPP

#include <vulkan/vulkan.h>
#include <cstdarg>

namespace elysian::renderer {

// Wrap to debugExt shit
class DebugLog {
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
    virtual void write(Source source, Severity severity, va_list args, const char* pBuffer) = 0;

    virtual void debugUtilsMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);

    void pop(unsigned count);

    void log(Source source, Severity severity, const char* pFmt, ...);
    void verbose(const char* pFmt, ...);
    void info(const char* pFmt, ...);
    void warn(const char* pFmt, ...);
    void error(const char* pFmt, ...);
};
}

#endif // ELYSIAN_MESSENGER_DEBUG_LOG_HPP
