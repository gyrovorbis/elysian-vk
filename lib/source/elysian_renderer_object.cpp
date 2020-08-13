#include <renderer/elysian_renderer_object.hpp>


namespace elysian::renderer {

Result Object::setObjectName(const char *pName) {
    Result result;
    m_name = pName;
    // Provided by VK_EXT_debug_utils
    const auto info = VkDebugUtilsObjectNameInfoEXT {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        nullptr,
        getObjectType(),
        getObjectHandle(),
        m_name.c_str()
    };
    if(getDevice()) {
        result = vkSetDebugUtilsObjectNameEXT(nullptr, &info);
    }
    return result;
}

Result Object::setObjectTag(uint64_t tagName, size_t tagSize, const void *pTag) {
    Result result;
    m_tag.reset(new Tag { tagName, tagSize, pTag } );
    const auto info = VkDebugUtilsObjectTagInfoEXT {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT,
        nullptr,
        getObjectType(),
        getObjectHandle(),
        tagName,
        tagSize,
        pTag
    };
    if(getDevice()) {
        result = vkSetDebugUtilsObjectTagEXT(nullptr, &info);
    }
    return result;
}

}
