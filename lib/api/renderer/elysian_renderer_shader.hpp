#ifndef ELYSIAN_RENDERER_SHADER_HPP
#define ELYSIAN_RENDERER_SHADER_HPP

namespace elysian {


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
