#ifndef ELYSIAN_RENDERER_SHADER_HPP
#define ELYSIAN_RENDERER_SHADER_HPP

#include "renderer/elysian_renderer_object.hpp"

namespace elysian {

class ShaderBinary {
public:
    uint32_t*   getData(void) const;
    size_t      getSize(void) const;
private:
    uint32_t*   m_pData  = nullptr;
    size_t      m_size   = 0;
};


class ShaderBinaryFile: public ShaderBinary {
public:

    const char* getFilePath(void) const;
private:
    std::string     m_filePath;
};

class ShaderModule: public HandleObject {

};

//cache pool size?
//mark certain shit persistent?
class ShaderModuleCache {
public:
    ShaderModule* acquireModule(const char* pFilePath);

    void reset(void) const;
private:
    std::map<std::string, ShaderBinaryFile> m_binaries;
};

class ShaderModule: public VkShaderModuleCreateInfo {
#if 0
    // Provided by VK_VERSION_1_0
    typedef struct VkShaderModuleCreateInfo {
        VkStructureType              sType;
        const void*                  pNext;
        VkShaderModuleCreateFlags    flags;
        size_t                       codeSize;
        const uint32_t*              pCode;
    } VkShaderModuleCreateInfo;
#endif
public:
    ShaderModule(size_t codeSize, const uint32_t* pCode):
        VkShaderModuleCreateInfo({
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            nullptr,
            0,
            codeSize,
            pCode
        })
    {}
    ~ShaderModule(void);

    size_t getCodeSize(void) const;
    const char* getFilePath(void) const;
private:
    std::string m_filePath;
};

class ShaderModuleCache {
public:
    const ShaderModule* fetchModule(const char* pFilePath);
private:
    std::vector<ShaderModule> m_modules;
};




}

#endif // ELYSIAN_RENDERER_SHADER_HPP
