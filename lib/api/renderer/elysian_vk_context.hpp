#ifndef ELYSIAN_VK_CONTEXT_HPP
#define ELYSIAN_VK_CONTEXT_HPP

#include <string>
#include <stack>
#include <vector>

namespace elysian::vk {


class LogicalDevice;
class VmaAllocator;
class CommandPool;
class DescriptorPool;
class DebugLog;
class VmaAllocation;
class Instance;


//GET NAME ACCESSOR
class DeviceResource: public ChildObject<Context> {
public:
    enum class Type: uint8_t {
        Allocation,
        Buffer,
        BufferView,
        Image,
        ImageView,
        Sampler,
        Semaphore,
        Event,
        Fence,
        CommandBuffer,
        DescriptorSet,
        DescriptorSetLayout,
        DescriptorUpdateTemplate,
        FrameBuffer,
        ShaderModule,
        RenderPass,
        //aggregates
        CommandPool,
        DescriptorPool,
        QueryPool,
        //Pipeline Shit
        Count
    };

    virtual Type getType(void) const = 0;

    template<typename T>
    T* as(void) {
        if(getType() == T::DeviceResourceType) {
            return static_cast<T*>(this);
        } else {
            return nullptr;
        }
    }
};

template<typename H, DeviceResource::Type T>
class DeviceResourceHandleObject: public HandleObject<H>,
                      public DeviceResourceBase

{
    const static constexpr DeviceResource::Type DeviceResourceType = T;

     DeviceResource::Type getType(void) const override { return DeviceResourceType; }

};

// Create a global enum for resource type, then index everything by integer!
// Resource::getType()
struct ResourceTracker {

    struct AllocationMapping {
        VmaAllocation*          pAllocation;
        std::vector<Resource*>  references;
    };

    std::array<std::vector<DeviceResource*>,
        static_cast<int>(DeviceResource::Type::Count)>
        resources;

    std::map<VmaAllocation*, AllocationMapping*>
                                        allocationMappings;

    DeviceResource* findByName(const char* pName);
    //figure out shit by memory type...
};

/*
 * int main(int argc, char* argv[]) {
 *  auto* pInstance = Instance.create(createInfo);
 *
 *  auto* pDevice = pInstance->getDevice(0);
 *
 * Context ctx(pDevice, "Temp1");
 *
 * Buffer* pBuffer = ctx.createBuffer(blah);
 * Allocation* pAlloc = ctx.allocMemory(blah);
 * Image* pImage = ctx.createImage(blah);
 *
 *
 * pBuffer->bind(pAlloc);
 *
 * Context ctx2(ctx, "Temp2");
 *
 * Buffer buffy(ctx, createShit, blah);
 *
 * //Doing everything via context allows you to hide createFlags logic,
 * //inter-object communication/abstraction, etc
 *
 *
 *
 *
 *
 * }
 */

template<typename T>
class ArrayMapContainer {
public:
    size_t getSize(void) const;
    T* getEntry(int index) const;
    T* getEntry(const char* pName) const;

    void addEntry(T* pEntry);
    void emplace(const char* pName, )


    void removeEntry(T* pEntry);
    void remove(int index);
    void remove(const char* pName);





private:
    std::vector<T*> m_container;

};

struct ContextCreateInfo {
    std::vector<CommandPoolCreateInfo>      commandPoolInfos;
    std::vector<DescriptorPoolCreateInfo>   descriptorPoolInfos;
    std::vector<AllocatorPoolCreateInfo>    allocatorPoolInfos;
};

// #defines for tracking and stats and shit

//Inherit from context as a common thing? Similar to elysian::ComputeTask?
//DeviceProcAddr function pointer cache

class Context: public ChildObject<Context> {
public:
    struct InheritanceFlags {
        enum Value : uint8_t {
            None              = 0,
            AllocatorPool     = 1 << 0,
            CommandPool       = 1 << 1,
            DescriptorPool    = 1 << 2,
            QueryPool         = 1 << 3,
            PipelineCache     = 1 << 4,
            //PipelineCache
            //DeviceFunctionCache
            All               = ~None
        };
    };

    struct Stats { // overload +, += operators
        size_t count[static_cast<int>(DeviceResource::Type::Count)] = { 0 };
    };

                        Context(LogicalDevice* pDevice, ContextCreateInfo createInfo={}, const char* pName="Temp");
                        Context(Context* pParent, ContextCreateInfo createInfo={},
                                InheritanceFlags inheritanceFlags=InheritanceFlags::All, const char* pName="Temp");
                        ~Context(void); //check resources

    //Stats calculateStats(void) const;
    //Stats calculateStatsRecursive(void) const;
    //Stats getWatermark(void) const;
    //Stats getStatsTotals(void) const;

    const char*         getName(void) const;
    void                setName(const char* pName);

    Instance*           getInstance(void) const;
    LogicalDevice*      getDevice(void) const;
    Allocator*          getAllocator(void) const;
    InheritanceFlags    getInheritanceFlags(void) const;

    Context*            getChild(int index) const;
    Context*            getChild(const char* pName) const;

    auto                getResourceTracker(void) const -> const ResourceTracker&;

    //wrap all device functions

    void                log(DebugLog* pLog) const;

    // create/add commandPool/descriptorPool
    // set Allocator
    // add/create child context


    // ===== Free-floating shit ======
    Allocation* allocateMemory(VkMemoryRequirements memoryRequirements, VmaAllocationCreateInfo createInfo, const char* pName=nullptr);

    Buffer*     createBuffer(const vk::BufferCreateInfo& info, const VmaAllocationCreateInfo* pInfo=nullptr, const char* pName=nullptr);
    BufferView* createBufferView(Buffer* pBuffer, const vk::BufferViewCreateInfo info, const char* pName=nullptr);

    Image*      createImage(const vk::ImageCreateInfo& info, const VmaAllocationCreateInfo* pInfo=nullptr);
    ImageView*  createImageView(Image* pImage, const vk::ImageViewCreateInfo& info, const char* pName=nullptr);

    Sampler*    createSampler(const vk::SamplerCreateInfo& info, const char* pName=nullptr);

    Event*      createEvent(const vk::EventCreateInfo& info, const char* pName=nullptr);
    Semaphore*  createSemaphore(const vk::SemaphoreCreateInfo& info, const char* pName=nullptr);
    Fence*      createFence(const vk::FenceCreateInfo& info, const char* pName=nullptr);

    auto        createFramebuffer(const vk::FramebufferCreateInfo& info, const char* pName=nullptr) -> Framebuffer*;

    template<typename R>
    bool        releaseResource(R* pResource);

    // ==== Persistent shit =========
    auto        createCommandPool(const vk::CommandPoolCreateInfo& info, const char* pName) -> CommandPool*;
    auto        createDescriptorPool(const vk::DescriptorPoolCreateInfo& info, const char* pName) -> DescriptorPool*;
    auto        createAllocatorPool(const VmaPoolCreateInfo& info, const char* pName) -> AllocatorPool*;


    /* DEVICE API FUNCTIONALITY
     *  - only shit partaining to resource creation/management
     *

        createRenderPass()
        createShaderModule()
        createSwapChain()

     * allocateCommandBuffer()
     * allocateDescriptorSets()

        createDescriptorSetLayout()
        createPipelines()
        createPipelineLayout()
        createDescriptorUpdateTemplate()
        createPipelineCache()
        createQueryPool()
        */
    // === RECURSIVE/INHERITANCE LOOKUPS =====
    CommandPool*        getCommandPool(int virtualIndex) const;
    CommandPool*        getCommandPool(const char* pName) const;

    DescriptorPool*     getDescriptorPool(int virtualIndex) const;
    DescriptorPool*     getDescriptorPool(const char* pName) const;

    AllocatorPool*      getAllocatorPool(int virtualIndex) const;
    AllocatorPool*      getAllocatorPool(const char* pName) const;

protected:
    CommandPool*        _getCommandPool(int index) const;
    CommandPool*        _getCommandPool(const char* pName) const;
    DescriptorPool*     _getDescriptorPool(int index) const;
    DescriptorPool*     _getDescriptorPool(const char* pName) const;
    AllocatorPool*      _getAllocatorPool(int index) const;
    AllocatorPool*      _getAllocatorPool(const char* pName) const;

    template<typename T, typename F>
    T*                  getByIndexRecursive(int virtualIndex, F&& func) const;

private:
    ResourceTracker                 m_tracker;
    std::vector<CommandPool*>       m_commandPool;
    std::vector<DescriptorPool*>    m_descriptorPool;
    std::vector<AllocatorPool*>     m_allocatorPool;
    std::vector<Context*>           m_children;
    std::string                     m_name;
    LogicalDevice*                  m_pDevice           = nullptr;
    InheritanceFlags                m_inheritanceFlags  = InheritanceFlags::None;
    //query pool shit?
};

inline const char* Context::getName(void) const { return m_name.c_str(); }
inline void Context::setName(const char* pName) { m_name = pName; }

inline LogicalDevice* Context::getDevice(void) const { return m_pDevice; }
inline Instance* Context::getInstance(void) const { return m_pDevice->getInstance(); }
inline auto Context::getInheritanceFlags(void) const -> InheritanceFlags { return m_inheritanceFlags; }

inline Context* getChild(int index) const {
    assert(index < m_children.size());
    return m_children[index];
}

inline Context* getChild(const char* pName) const {
    Context* pChild = nullptr;
    for(auto* pCtx : m_children) {
        if(pCtx->getName() == pName) {
            pChild = pCtx;
            break;
        }
    }
    return pChild;
}


inline const ResourceTracker& Context::getResourceTracker(void) const { return m_tracker; }

inline VmaAllocator* Context::getAllocator(void) const {
    VmaAllocator* pAlloc = nullptr;
    if(m_pAllocator) {
        p_Alloc = m_pAllocator;
    } else if(getInheritanceFlags() & InheritanceFlags::Allocator) {
        pAlloc = getParent()->getAllocator();
    }
    return pAlloc;
}

inline AllocatorPool* Context::getAllocatorPool(int virtualIndex) const {
    return getByIndexRecursive(virtualIndex,
                               InheritanceFlags::AllocatorPool,
                                [](Context* pCtx, int index) {
                                    return pCtx->_getAllocatorPool(index);
                                },
                                [](Context* pCtx) {
                                    return pCtx->m_allocatorPool.size();
                                });
}
inline AllocatorPool* Context::getAllocatorPool(const char* pName) const {

}




template<typename FGet, typename FSize>
inline decltype(auto) Context::getByIndexRecursive(int virtualIndex, InheritanceFlags::Value inheritanceMask, FGet&& fnGet, FSize&& fnSize) const {
    std::stack<Context*> traversal;
    Context* pCurrent = this;
    while(pCurrent) {
        traversal.push(pCurrent);
        if(!(pCurrent->getInheritanceFlags() & inheritanceMask)) {
            break;
        }
        pCurrent = pCurrent->getParent();
    }

    int accumSize = 0;
    while(!traversal.empty() && (pCurrent = traversal.top())) {
        int curSize = fnSize(pCurrent);
        int curIndex = virtualIndex - accumSize;
        if(curIndex < curSize) {
            return fnGet(pCurrent, curIndex);
        } else {
            accumSize += curSize;
        }
        traversal.pop();
    }
    return nullptr;
}

// #define AlloationContext to be an empty struct with a variadically templated constructor when not debugging
// Static pool allocator that just uses base for a big-ass up-front allocation
// Per-frame allocator which recycles everything at the end of a frame

/* context shit:
 1) allocator name
 2) frame name
 3) type name
 4) source location
 5) last debug log message


  */
#if 0

int main() {

    Allocator alloc("Main");

   RAII AllocationGuard(&alloc);

    alloc.pushFrame("CurrentFrame");

    auto* pEntity = alloc.create<Entity>({"Entity_Name"}, "EntityName", 1, 2, 3, 4);

    alloc.destroy(pEntity);

    alloc.popFrame();








}



#endif

}

#endif // ELYSIAN_VK_CONTEXT_HPP
