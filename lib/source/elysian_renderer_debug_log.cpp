#include <renderer/elysian_renderer_debug_log.hpp>
#include <cassert>

namespace elysian::renderer {

void DebugLog::pop(unsigned count) {
    for(unsigned c = 0; c < count; ++c) pop();
}

void DebugLog::log(Source source, Severity severity,  DebugLogStringContextWrapper pFmt, ...) {
    va_list args;
    va_start(args, pFmt);
    write(source, severity, pFmt, args, pFmt);
    va_end(args);
}
void DebugLog::info(DebugLogStringContextWrapper pFmt, ...) {
    va_list args;
    va_start(args, pFmt);
    write(Source::Renderer, Severity::Info, pFmt, args, pFmt);
    va_end(args);
}
void DebugLog::warn(DebugLogStringContextWrapper pFmt, ...) {
    va_list args;
    va_start(args, pFmt);
    write(Source::Renderer, Severity::Warning, pFmt, args, pFmt);
    va_end(args);
}
void DebugLog::verbose(DebugLogStringContextWrapper pFmt, ...) {
    va_list args;
    va_start(args, pFmt);
    write(Source::Renderer, Severity::Verbose, pFmt, args, pFmt);
    va_end(args);
}
void DebugLog::error(DebugLogStringContextWrapper pFmt, ...) {
    va_list args;
    va_start(args, pFmt);
    write(Source::Renderer, Severity::Error, pFmt, args, pFmt);
    va_end(args);
}

void DebugLog::debugUtilsMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{

    DebugLog::Source src;
    DebugLog::Severity severity;

    if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        severity = DebugLog::Severity::Error;
    } else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        severity = DebugLog::Severity::Warning;
    } else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        severity = DebugLog::Severity::Info;
    } else if(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {

    } else {
        assert(false);
    }

    if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        src = DebugLog::Source::Driver_General;
    } else if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        src = DebugLog::Source::Driver_Validation;
    } else if(messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        src = DebugLog::Source::Driver_Performance;
    }

    log(src, severity, "%s", pCallbackData->pMessage);
}


}
