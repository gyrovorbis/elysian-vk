#ifndef ELYSIAN_RENDERER_COMMAND_HPP
#define ELYSIAN_RENDERER_COMMAND_HPP

namespace elysian::renderer {

class CommandPool {
public:

    class CreateInfo: public VkCommandPoolCreateInfo {
    public:
        CreateInfo(VkCommandPoolCreateFlags flags, uint32_t queueFamilyIndex):
            VkCommandPoolCreateInfo({
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                nullptr,
                flags,
                queueFamilyIndex
            })
        {}
    };

    class Initializer {
        std::string name;
        CreateInfo  info;
        Device*     pDevice;
        Allocator*  pAllocator = nullptr;
    };

    CommandPool(Initializer initializer);
    ~CommandPool(void);

    uint32_t    getQueueFamilyIndex(void) const;
    const char* getName(void) const;
    VkCommandPoolCreateFlags getFlags(void) const;

    Result      getResult(void) const;
    bool        isValid(void) const;
private:
    std::string     m_name;
    VkCommandPool   m_pool;
    Result          m_result;
};

// don't know how to take state from pending back to executable?
// can validate cmdExecuteCommands for subcommands
// can validate draw commands without begin/end render pass
// can validate compute in non-compute shit
// count draw commands/verts/shit? Count binds and shit?
class CommandBuffer {
public:

    enum class State: uint8_t {
        Initial,
        Recording,
        Executable,
        Pending,
        Invalid
    };

    CommandBuffer(VkCommandBuffer vkBuffer, std::string name, const CommandBufferSet* pParentSet);

    VkCommandBuffer getHandle(void) const;
    const char*     getName(void) const;
    auto            getSet(void) const -> const CommandBufferSet*;
    uint32_t        getCommandCount(void) const;

    State           getState(void) const;
    Result          getResult(void) const;
    bool            isValid(void) const;

    Result begin(const VkCommandBufferBeginInfo& info) const;
    Result end(void) const;
    Result reset(VkCommandBufferResetFlags flags);

    // Actual commands to be enqueued

    void cmdExecuteCommands(uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) const;
#if 0
    This one better be a PRIMARY buffer too...
    If any element of pCommandBuffers was not recorded with the VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT flag, and it was recorded into any other primary command buffer which is currently in the executable or recording state, that primary command buffer becomes invalid.
#endif

    // Compute
    void cmdDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
    void cmdDispatchIndirect(VkBuffer buffer, VkDeviceSize offset);

    void cmdBeginRenderPass(const VkRenderPassBeginInfo& info, VkSubpassContents subpassContents) const;
    void cmdEndRenderPass(void) const;

    void cmdBindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const;
    void cmdBindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const;
    void cmdBindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) const;
    void cmdBindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) const;

    void cmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const;
    void cmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const;
    void cmdDrawIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) const;
    void cmdDrawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) const;

private:
    VkCommandBuffer         m_handle    = VK_INVALID_HANDLE;
    std::string             m_name;
    const CommandBufferSet* m_pSet      = nullptr;
    Result                  m_result;
    State                   m_state     = State::Initial;
    uint32_t                m_cmdCount  = 0;
    VkRenderPassBeginInfo   m_renderPassBeginInfo; //used for validation during recording
};

// Rename to Group, vulkan already has (descriptor) sets!
class CommandBufferSet {
public:
    class AllocateInfo: public VkCommandBufferAllocateInfo {
    public:
        AllocateInfo(VkCommandPool          pool,
                     VkCommandBufferLevel   level,
                     uint32_t               commandBufferCount):
            VkCommandBufferAllocateInfo({
                VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                nullptr,
                pool,
                level,
                commandBufferCount
            })
        {}
    };

    struct Initializer {
        std::string              name;
        const Device*            pDevice;
        const CommandPool*       pPool;
        AllocateInfo             allocInfo;
        std::vector<std::string> bufferNames = {};
    };

                        CommandBufferSet(Initializer initializer);
                        ~CommandBufferSet(void);

    Result               getResult(void) const;
    bool                 isValid(void) const;
    const char*          getName(void) const;
    const CommandPool    getCommandPool(void) const;

    const CommandBuffer* getBuffer(uint32_t index=0) const;
    auto                 getBuffers(void) const -> const std::vector<CommandBuffer>&;

private:
    Initializer                 m_initializer;
    std::vector<CommandBuffer>  m_buffers;
    Result                      m_result;
};

inline CommandBuffer::CommandBuffer(VkCommandBuffer vkBuffer, std::string name):
    m_handle(vkBuffer),
    m_name(std::move(name))
{
    std::assert(vkBuffer != VK_INVALID_HANDLE);
}

inline bool CommandBuffer::isValid(void) const { return getHandle() != VK_INVALID_HANDLE && getResult() && getState() != State::Invalid; }
inline VkCommandBuffer CommandBuffer::getHandle(void) const { return m_handle; }
inline const char* CommandBuffer::getName(void) const { return m_name.c_str(); }
inline Result CommandBuffer::getResult(void) const { return m_result; }
inline auto CommandBuffer::getState(void) const { return m_state; }
inline const CommandBufferSet* CommandBuffer::getSet(void) const { return m_pSet; }
inline uint32_t CommandBuffer::getCommandCount(void) const { return m_cmdCount; }

inline Result CommandBuffer::begin(const VkCommandBufferBeginInfo& info) const {
    std::assert(getState() != State::Recording &&
                getState() != Statr::Pending);
    std::assert(getState() == State::Initial ||
                getSet()->getCommandPool()->getFlags() & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    m_result = vkBeginCommandBuffer(getHandle(), &info);
    m_state = m_result? State::Recording : State::Invalid;
}

inline Result CommandBuffer::end(void) const {
    std::assert(getState() == State::Recording);
    m_result = vEndCommandBuffer(getHandle());
    std::assert(m_result);
    m_state = m_result? State::Executable : State::Invalid;
}

inline Result CommandBuffer::reset(VkCommandBufferResetFlags flags) {
    std::assert(getState() != State::Pending);
    m_result = vkResetCommandBuffer(getHandle(), flags);
    std::assert(m_result);
    m_state = m_result? State::Initial : State::Invalid;
    m_cmdCount = 0;
}

inline void CommandBuffer::cmdDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    std::assert(getState() == State::Recording);
    vkCmdDispatch(getHandle(), groupCountX, groupCountY, groupCountZ);
    ++m_cmdCount;
}

inline void CommandBuffer::cmdBeginRenderPass(const VkRenderPassBeginInfo& info, VkSubpassContents subpassContents) const {
    std::assert(getState() == State::Recording);
    vkCmdBeginRenderPass(getHandle(), &info, subpassContents);
    memcpy(&m_renderPassBeginInfo, &info, sizeof(VkRenderPassBeginInfo));
    ++m_cmdCount;
}

inline void CommandBuffer::cmdEndRenderPass(void) const {
    std::assert(getState() == State::Recording);
    vkCmdEndRenderPass(getHandle());
    ++m_cmdCount;
}

inline void CommandBuffer::cmdBindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline) const {
    std::assert(getState() == State::Recording && pipeline != VK_INVALID_HANDLE);
    vkCmdBindPipeline(getHandle(), bindPoint, pipeline);
    ++m_cmdCount;
}

inline void CommandBuffer::cmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {
    std::assert(getState() == State::Recording);
    vkCmdDraw(getHandle(), vertexCount, instanceCount, firstVertex, firstInstance);
    ++m_cmdCount;
}

inline void CommandBuffer::cmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const {
    std::assert(getState() == State::Recording);
    vkCmdDrawIndexed(getHandle(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    ++m_cmdCount;
}

#if 0
// Provided by VK_VERSION_1_0
void vkCmdPipelineBarrier(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    VkDependencyFlags                           dependencyFlags,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers);

void vkCmdBeginQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags);

// Provided by VK_VERSION_1_0
void vkCmdEndQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query);

// Provided by VK_VERSION_1_0
void vkCmdCopyQueryPoolResults(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags);

// Provided by VK_VERSION_1_0
void vkCmdResetQueryPool(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount);


// Provided by VK_VERSION_1_0
void vkCmdBlitImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageBlit*                          pRegions,
    VkFilter                                    filter);

// Provided by VK_VERSION_1_0
void vkCmdClearAttachments(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkClearAttachment*                    pAttachments,
    uint32_t                                    rectCount,
    const VkClearRect*                          pRects);

// Provided by VK_VERSION_1_0
void vkCmdClearColorImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearColorValue*                    pColor,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges);

// Provided by VK_VERSION_1_0
void vkCmdClearDepthStencilImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearDepthStencilValue*             pDepthStencil,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges);

// Provided by VK_VERSION_1_0
void vkCmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions);

// Provided by VK_VERSION_1_0
void vkCmdCopyBufferToImage(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions);

// Provided by VK_VERSION_1_0
void vkCmdCopyImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageCopy*                          pRegions);

// Provided by VK_VERSION_1_0
void vkCmdCopyImageToBuffer(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions);

// Provided by VK_VERSION_1_0
void vkCmdFillBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                size,
    uint32_t                                    data);

// Provided by VK_VERSION_1_0
void vkCmdPushConstants(
    VkCommandBuffer                             commandBuffer,
    VkPipelineLayout                            layout,
    VkShaderStageFlags                          stageFlags,
    uint32_t                                    offset,
    uint32_t                                    size,
    const void*                                 pValues);

// Provided by VK_VERSION_1_0
void vkCmdResetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask);

// Provided by VK_VERSION_1_0
void vkCmdSetBlendConstants(
    VkCommandBuffer                             commandBuffer,
    const float                                 blendConstants[4]);

// Provided by VK_VERSION_1_0
void vkCmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors);

// Provided by VK_VERSION_1_0
void vkCmdSetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask);

// Provided by VK_VERSION_1_0
void vkCmdSetDepthBias(
    VkCommandBuffer                             commandBuffer,
    float                                       depthBiasConstantFactor,
    float                                       depthBiasClamp,
    float                                       depthBiasSlopeFactor);

// Provided by VK_VERSION_1_0
void vkCmdSetDepthBounds(
    VkCommandBuffer                             commandBuffer,
    float                                       minDepthBounds,
    float                                       maxDepthBounds);

// Provided by VK_VERSION_1_0
void vkCmdSetLineWidth(
    VkCommandBuffer                             commandBuffer,
    float                                       lineWidth);

// Provided by VK_VERSION_1_0
void vkCmdSetStencilCompareMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    compareMask);

// Provided by VK_VERSION_1_0
void vkCmdSetStencilReference(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    reference);

// Provided by VK_VERSION_1_0
void vkCmdSetStencilWriteMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    writeMask);

// Provided by VK_VERSION_1_0
void vkCmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports);

// Provided by VK_VERSION_1_0
void vkCmdUpdateBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                dataSize,
    const void*                                 pData);

// Provided by VK_VERSION_1_0
void vkCmdWaitEvents(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers);

// Provided by VK_VERSION_1_0
void vkCmdWriteTimestamp(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query);

#endif

inline CommandPool::CommandPool(Initializer initializer):
    m_name(std::move(initializer.name))
{
    m_result = vkCreateCommandPool(initializer.pDevice, &initializer.info, initializer.pAllocator, &m_pool);
}

inline CommandPool::~CommandPool(void) {
    vkDestroyCommandPool(initializer.pDevice, m_pool, nullptr);
}


inline CommandBufferSet::CommandBufferSet(Initializer initializer):
    m_initiliazer(std::move(initializer))
{
    std::vector<VkCommandBuffer> vkBuffers(m_initializer.allocInfo.commandBufferCount, VK_INVALID_HANDLE);
    m_buffers.reserve(m_initializer.allocInfo.commandBufferCount);
    const Result result = vkAllocateCommandBuffers(m_initializer.pDevice, &m_initializer.allocInfo, vkBuffers.data());

    for(int b = 0; b < vkBuffers.size(); ++b) {
        m_buffers.emplace_back(vkBuffefs[b],
                               ((b < m_initializer.bufferNames.size())?
                                   std::move(m_initializer.bufferNames[b]) :
                                   ""),
                               this);
    }
}

inline CommandBufferSet::~CommandBufferSet(void) {
    std::vector<VkCommandBuffer> vkBuffers(m_buffers.size());
    for(auto&& buff : m_buffers) {
        vkBuffers.push_back(buff.getHandle());
    }

    vkFreeCommandBuffers(m_initializer.pDevice,
                         m_initializer.allocInfo.
                         m_initializer.allocInfo.commandPool,
                         vkBuffers.data());
}

inline const CommandBuffer* CommandBufferSet::getBuffer(uint32_t index=0) const {
    std::assert(index < m_buffers.size());
    return &m_buffers[index];
}

inline auto CommandBufferSet::getBuffers(void) const -> const std::vector<CommandBuffer>& {
    return m_buffers;
}


}

#endif // ELYSIAN_RENDERER_COMMAND_HPP
