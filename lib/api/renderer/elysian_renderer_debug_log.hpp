#ifndef ELYSIAN_MESSENGER_DEBUG_LOG_HPP
#define ELYSIAN_MESSENGER_DEBUG_LOG_HPP

#include <vulkan/vulkan.h>
#include <cstdarg>
#include <source_location>

namespace elysian::renderer {
// requires at least C++11
const std::string vstrprintf(const std::string sFormat, ...) {

    const char * const zcFormat = sFormat.c_str();

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, sFormat);

    // reliably acquire the size from a copy of
    // the variable argument array
    // and a functionally reliable call
    // to mock the formatting
    va_list vaCopy;
    va_copy(vaCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaCopy);
    va_end(vaCopy);

    // return a formatted string without
    // risking memory mismanagement
    // and without assuming any compiler
    // or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc.data(), zc.size());
}



class DebugLogStringContextWrapper {
public:
    DebugLogContextWrapper(const char* pFmt, std::source_location srcLoc = std::source_location::current()):
        m_pFmt(pFmt),
        m_srcLoc(std::move(srcLoc))
    {}

    operator const char*() const { return m_pFmt; }
    operator const std::source_location&() const { return m_srcLoc; }
private:
    const char* m_pFmt = nullptr;
    std::source_location m_srcLoc;
};


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
        ElysianVk,
        Driver_General,
        Driver_Validation,
        Driver_Performance
    };

    virtual void push(void) = 0;
    virtual void pop(void) = 0;
    virtual void write(Source source, Severity severity, const std::source_location& sourceLoc, va_list args, const char* pBuffer) = 0;

    virtual void debugUtilsMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);

    void pop(unsigned count);

    void log(Source source, Severity severity, DebugLogStringContextWrapper pFmt, ...);
    void verbose(DebugLogStringContextWrapper pFmt, ...);
    void info(DebugLogStringContextWrapper pFmt, ...);
    void warn(DebugLogStringContextWrapper pFmt, ...);
    void error(DebugLogStringContextWrapper pFmt, ...);
};
}

#endif // ELYSIAN_MESSENGER_DEBUG_LOG_HPP
