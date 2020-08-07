#ifndef ELYSIAN_RENDERER_DESCRIPTOR_HPP
#define ELYSIAN_RENDERER_DESCRIPTOR_HPP

namespace elysian::renderer {

class DescriptorPool {
public:

    class CreateInfo: public VkDescriptorPoolCreateInfo {
    public:
        CreateInfo(VkDescriptorPoolCreateFlags          flags,
                   uint32_t                             maxSets,
                   std::vector<VkDescriptorPoolSize>    poolSizes):
            VkDescriptorPoolCreateInfo({
                VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                nullptr,
                flags,
                maxSets,
                poolSizes.size(),
                poolSizes.data()
            }),
            m_poolSizes(std::move(poolSizes))
        {}

        auto                getFlags(void) const -> VkDescriptorPoolCreateFlags;
        uint32_t            getMaxSets(void) const;
        auto                getPoolSizes(void) const -> const std::vector<VkDescriptorPoolSize>&;

    private:
        std::vector<VkDescriptorPoolSize>   m_poolSizes;
    };

    struct Initializer {
        std::string name;
        CreateInfo  info;
        Device*     pDevice;
    };

                        DescriptorPool(Initializer initializer);
                        ~DescriptorPool(void);

    operator            VkDescriptorPool() const;

    bool                isValid(void) const;
    Result              getResult(void) const;
    VkDescriptorPool    getHandle(void) const;

    const char*         getName(void) const;
    const CreateInfo&   getCreateInfo(void) const;

private:
    Initializer         m_initializer;
    VkDescriptorPool    m_handle = VK_INVALID_HANDLE;
    Result              m_result;
};

//======= DESCRIPTOR SETS ==========
// Provided by VK_VERSION_1_0



// Provided by VK_VERSION_1_0
typedef struct VkDescriptorSetAllocateInfo {
    VkStructureType                 sType;
    const void*                     pNext;
    VkDescriptorPool                descriptorPool;
    uint32_t                        descriptorSetCount;
    const VkDescriptorSetLayout*    pSetLayouts;
} VkDescriptorSetAllocateInfo;

// Provided by VK_VERSION_1_0
VkResult vkFreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets);

class DescriptorSet {
public:
                            DescriptorSet(VkDescriptorSet handle, const DescriptorSetLayout* pLayout);

    operator                VkDescriptorSet() const;

    auto                    getLayout(void) const -> const DescriptorSetLayout*;

    bool                    isValid(void) const;
    VkDescriptorSet         getHandle(void) const;

private:
    VkDescriptorSet             m_handle    = VK_INVALID_HANDLE;
    const DescriptorSetLayout*  m_pLayout   = nullptr;
};

inline DescriptorSet::DescriptorSet(VkDescriptorSet handle, const DescriptorSetLayout *pLayout):
    m_handle(handle),
    m_pLayout(pLayout)
{}

inline bool DescriptorSet::isValid(void) const { return getHandle() != VK_INVALID_HANDLE; }
inline const DescriptorSetLayout* DescriptorSet::getLayout(void) const { return m_pLayout; }
inline VkDescriptorSet DescriptorSet::getHandle(void) const { return m_handle; }


class DescriptorSetGroup {
public:
    class AllocateInfo: public VkDescriptorSetAllocateInfo {
    public:
        AllocateInfo(const DescriptorPool* pPool, std::vector<const DescriptorSetLayout*> layouts):
            m_pPool(pPool),
            m_layouts(std::move(layouts))
        {
            std::vector<VkDescriptorSetLayout> vkLayouts;
            for(const auto&& layout : m_layouts) {
                vkLayouts.push_back(layout->getHandle());
            }

            const auto tempInfo = VkDescriptorSetAllocateInfo {
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                nullptr,
                m_pPool->getHandle(),
                vkLayouts.size(),
                vkLayouts.data()
            };

            memcpy(static_cast<VkDescriptorSetAllocateInfo*>(this),
                   &tempInfo,
                   sizeof(VkDescriptorSetAllocateInfo));
        }

        auto getLayouts(void) const -> std::vector<const DescriptorSetLayout*>&;

    private:
        const DescriptorPool* m_pPool = nullptr;
        std::vector<const DescriptorSetLayout*> m_layouts;
    };

    struct Initializer {
        std::string name;
        std::shared_ptr<const AllocateInfo> pAllocInfo;
    };

                DescriptorSetGroup(Initializer initializer);
                ~DescriptorSetGroup(void);

    const char* getName(void) const;
    Result      getResult(void) const;
    bool        isValid(void) const;

    DescriptorSet* getSet(int index=0);
    DescriptorSet* getSet(const char* pLayoutName);

private:
    std::vector<DescriptorSet>          m_sets;
    std::string                         m_name;
    std::shared_ptr<const AllocateInfo> m_pAllocInfo;
    Result                              m_result;
};

inline DescriptorSetGroup::DescriptorSetGroup(const char *pName, std::shared_ptr<const AllocateInfo> pAllocInfo):
    m_name(pName),
    m_pAllocInfo(std::move(pAllocInfo))
{
    std::vector<VkDescriptorSet> sets(pAllocInfo->getLayoutCount());

    m_result = vkAllocateDescriptorSets(pAllocInfo->getPool()->getDevice(),
                                        pAllocInfo.get(),
                                        sets.data());

    for(int d = 0; d < sets.size(); ++d) {
        m_sets.emplace_back(sets[d], m_pAllocInfo->getLayouts()[d]);
    }
}

inline DescriptorSetGroup::~DescriptorSetGroup(void) {
    //MAYBE delete? Need a flag for this?
    //To delete with destructor versus waiting and only deleting shit
    //when DescriptorPool is destroyed?
}



//===== / DESCRIPTOR SETS ===========




class DescriptorSetLayoutBinding: public VkDescriptorSetLayoutBinding {
public:
    DescriptorSetLayoutBinding(const char*            pName,
                               uint32_t               binding,
                               VkDescriptorType       descriptorType, //uniform buffer, sampler?, etc
                               VkShaderStageFlags     stageFlags,
                               uint32_t               descriptorCount=1, //array size
                               std::vector<VkSampler> immutableSamplers):
        VkDescriptorSetLayoutBinding({
            binding,
            descriptorType,
            descriptorCount,
            stageFlags,
            immutableSamplers.data() //sketchy as fuck? Gets moved?
        }),
        m_immutableSamplers(std::move(immutableSamplers)),
        m_name(pName)
    {}

    const char* getName(void) const { return m_name; }
    uint32_t    getBinding(void) const;
    auto        getStageFlags(void) const -> VkShaderStageFlags;
    auto        getDescriptorType(void) const -> VkDescriptorType;
    uint32_t    getDescriptorCount(void) const;
    auto        getImmutableSamplers(void) const -> const std::vector<VkSampler>&;

private:
    std::vector<VkSampler> m_immutableSamplers;
    std::string            m_name;
};

class DescriptorSetLayout: public VkDescriptorSetLayoutCreateInfo {
#if 0
    typedef struct VkDescriptorSetLayoutCreateInfo {
        VkStructureType                        sType;
        const void*                            pNext;
        VkDescriptorSetLayoutCreateFlags       flags;
        uint32_t                               bindingCount;
        const VkDescriptorSetLayoutBinding*    pBindings;
    } VkDescriptorSetLayoutCreateInfo;
#endif
public:

    DescriptorSetLayout(const char*                             pName,
                        uint32_t                                set,
                        VkDescriptorSetLayoutCreateFlags        flags,
                        std::vector<DescriptorSetLayoutBinding> bindings):
        VkDescriptorSetLayoutCreateInfo({
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            nullptr,
            flags,
            bindings.size(),
            bindings.data() //not going to fucking work...
        }),
        m_name(pName),
        m_set(set)
    {}
    ~DescriptorSetLayout(void);

    const char*                 getName(void) const;
    uint32_t                    getSet(void) const;

    DescriptorSetLayoutBinding* getBinding(uint32_t binding) const;
    DescriptorSetLayoutBinding* getBinding(const char* pName) const;

private:
    uint32_t    m_set = 0;
    std::string m_name;

};

inline DescriptorPool::DescriptorPool(Initializer initializer):
    m_initializer(std::move(initializer))
{
    m_result = vkCreateDescriptorPool(initializer.pDevice,
                                      &initializer.info,
                                      nullptr,
                                      &m_handle);
}

inline DescriptorPool::~DescriptorPool(void) {
    vkDestroyDescriptorPool(m_initializer.pDevice, getHandle(), nullptr);
}

inline bool DescriptorPool::isValid(void) const {
    return m_result && getHandle() != VK_INVALID_HANDLE;
}

inline Result DescriptorPool::getResult(void) const {
    return m_result;
}

inline const char* DescriptorPool::getName(void) const { return m_initializer.name.c_str(); }
inline VkDescriptorPool DescriptorPool::getHandle(void) const { return m_handle; }
inline DescriptorPool::operator VkDescriptorPool() const { return getHandle(); }
inline auto DescriptorPool::getCreateInfo(void) const -> const CreateInfo& {
    return m_initializer.info;
}


}

#endif // ELYSIAN_RENDERER_DESCRIPTOR_HPP
