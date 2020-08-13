#ifndef ELYSIAN_RENDERER_COMMAND_HPP
#define ELYSIAN_RENDERER_COMMAND_HPP

#include "elysian_renderer_object.hpp"
#include <vector>

namespace elysian::renderer {

class CommandBufferGroup;

class CommandPoolCreateInfo: public VkCommandPoolCreateInfo {
public:
    CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags=0):
        VkCommandPoolCreateInfo({
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            nullptr,
            flags,
            queueFamilyIndex
        })
    {}
};

class CommandPool: public HandleObject<VkCommandPool, VK_OBJECT_TYPE_COMMAND_POOL> {
public:

                CommandPool(const Device* pDevice, const CommandPoolCreateInfo* pCreateInfo);
    virtual     ~CommandPool(void);

    Result      reset(VkCommandPoolResetFlags flags) const;
        // Provided by VK_VERSION_1_1
    void        trim(VkCommandPoolTrimFlags flags) const;

    Result      getResult(void) const;

    CommandBufferGroup* createGroup(VkCommandBufferLevel level, uint32_t commandBufferCount);
private:
    const Device*     m_pDevice = nullptr;
    Result      m_result;
};

// don't know how to take state from pending back to executable?
// can validate cmdExecuteCommands for subcommands
// can validate draw commands without begin/end render pass
// can validate compute in non-compute shit
// count draw commands/verts/shit? Count binds and shit?

// SOME OF THIS SHIT REQUIRES SPECIFIC VULKAN DEVICE VERSIONS!!!
class CommandBuffer: public HandleObject<VkCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER> {
public:

    enum class State: uint8_t {
        Initial,
        Recording,
        Executable,
        Pending,
        Invalid
    };

    CommandBuffer(VkCommandBuffer vkBuffer, const CommandBufferGroup* pParentgroup);

    auto            getGroup(void) const -> const CommandBufferGroup*;
    uint32_t        getCommandCount(void) const;

    State           getState(void) const;
    Result          getResult(void) const;

    Result begin(const VkCommandBufferBeginInfo& info);
    Result end(void);
    Result reset(VkCommandBufferResetFlags flags);

    // Actual commands to be enqueued

    // Debugging
    void cmdBeginDebugUtilsLabel(const char* pLabelName, float r=0.0f, float g=0.0f, float b=0.0f, float a=0.0f) const;
    void cmdEndDebugUtilsLabel(void) const;
    void cmdInsertDebugUtilsLabel(const char* pLabelName, float r=0.0f, float g=0.0f, float b=0.0f, float a=0.0f) const;

    void cmdExecuteCommands(uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) const;
#if 0
    This one better be a PRIMARY buffer too...
    If any element of pCommandBuffers was not recorded with the VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT flag, and it was recorded into any other primary command buffer which is currently in the executable or recording state, that primary command buffer becomes invalid.
#endif

    void cmdSetDeviceMask(uint32_t deviceMask) const; //better all be present within VkCommandGroupBeginInfo substruct!

    // Compute
    void cmdDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
    void cmdDispatchIndirect(VkBuffer buffer, VkDeviceSize offset);

    void cmdBeginRenderPass(const VkRenderPassBeginInfo& info, VkSubpassContents subpassContents);
    void cmdEndRenderPass(void);

    // Binding
    void cmdBindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);
    void cmdBindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const;
    void cmdBindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) const;
    void cmdBindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) const;

    // Drawing
    void cmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
    void cmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
    void cmdDrawIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) const;
    void cmdDrawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride) const;

    // Dynamic State
    void cmdSetLineWidth(float lineWidth) const;
    void cmdSetBlendConstants(const float blendConstants[4]) const;
    void cmdSetDepthBias(float constantFactor, float clamp, float slopeFactor) const;
    void cmdSetDepthBounds(float minBounds, float maxBounds) const;

    // Query Pools
    void cmdBeginQuery(VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags) const;
    void cmdEndQuery(VkQueryPool queryPool, uint32_t query) const;
    void cmdCopyQueryPoolResults(VkQueryPool queryPool,
                                 uint32_t firstQuery,
                                 uint32_t queryCount,
                                 VkBuffer dstBuffer,
                                 VkDeviceSize dstOffset,
                                 VkDeviceSize stride,
                                 VkQueryResultFlags flags) const;
    void cmdResetQueryPool(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) const;
    void cmdWriteTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query) const;

    // Events
    void cmdSetEvent(VkEvent event, VkPipelineStageFlags stageFlags) const;
    void cmdResetEvent(VkEvent event, VkPipelineStageFlags stageFlags) const;


private:
    const CommandBufferGroup* m_pGroup      = nullptr;
    Result                  m_result;
    State                   m_state     = State::Initial;
    uint32_t                m_cmdCount  = 0;
    VkRenderPassBeginInfo   m_renderPassBeginInfo; //used for validation during recording
};

class CommandBufferAllocateInfo: public VkCommandBufferAllocateInfo {
public:
    CommandBufferAllocateInfo(VkCommandPool          pool,
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

// Rename to Group, vulkan already has (descriptor) sets!
class CommandBufferGroup {
public:
                        CommandBufferGroup(const Device* pDevice, const CommandBufferAllocateInfo* pInfo);
                        ~CommandBufferGroup(void);

    Result               getResult(void) const;
    bool                 isValid(void) const;
    const CommandPool*    getCommandPool(void) const;

    const CommandBuffer* getBuffer(uint32_t index=0) const;
    auto                 getBuffers(void) const -> const std::vector<CommandBuffer>&;

private:
    const Device*               m_pDevice   = nullptr;
    VkCommandPool               m_pool      = VK_NULL_HANDLE;
    std::vector<CommandBuffer>  m_buffers;
    Result                      m_result;
};

inline CommandBuffer::CommandBuffer(VkCommandBuffer vkBuffer, const CommandBufferGroup* pGroup):
    HandleObject<VkCommandBuffer, VK_OBJECT_TYPE_COMMAND_BUFFER>(nullptr, vkBuffer, nullptr),
    m_pGroup(pGroup)
{
    assert(vkBuffer != VK_NULL_HANDLE);
}

inline Result CommandBuffer::getResult(void) const { return m_result; }
inline auto CommandBuffer::getState(void) const -> State { return m_state; }
inline const CommandBufferGroup* CommandBuffer::getGroup(void) const { return m_pGroup; }
inline uint32_t CommandBuffer::getCommandCount(void) const { return m_cmdCount; }

inline Result CommandBuffer::begin(const VkCommandBufferBeginInfo& info) {
    assert(getState() != State::Recording &&
                getState() != State::Pending);
    //assert(getState() == State::Initial ||
      //          getGroup()->getCommandPool()->flags & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    m_result = vkBeginCommandBuffer(getHandle(), &info);
    m_state = m_result? State::Recording : State::Invalid;
}

inline Result CommandBuffer::end(void) {
    assert(getState() == State::Recording);
    m_result = vkEndCommandBuffer(getHandle());
    assert(m_result);
    m_state = m_result? State::Executable : State::Invalid;
}

inline Result CommandBuffer::reset(VkCommandBufferResetFlags flags) {
    assert(getState() != State::Pending);
    m_result = vkResetCommandBuffer(getHandle(), flags);
    assert(m_result);
    m_state = m_result? State::Initial : State::Invalid;
    m_cmdCount = 0;
}

inline void CommandBuffer::cmdSetDeviceMask(uint32_t deviceMask) const {
    assert(getState() == State::Recording);
    vkCmdSetDeviceMask(getHandle(), deviceMask);
}

inline void CommandBuffer::cmdDispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    assert(getState() == State::Recording);
    vkCmdDispatch(getHandle(), groupCountX, groupCountY, groupCountZ);
    ++m_cmdCount;
}

inline void CommandBuffer::cmdBeginRenderPass(const VkRenderPassBeginInfo& info, VkSubpassContents subpassContents) {
    assert(getState() == State::Recording);
    vkCmdBeginRenderPass(getHandle(), &info, subpassContents);
    memcpy(&m_renderPassBeginInfo, &info, sizeof(VkRenderPassBeginInfo));
    ++m_cmdCount;
}

inline void CommandBuffer::cmdEndRenderPass(void) {
    assert(getState() == State::Recording);
    vkCmdEndRenderPass(getHandle());
    ++m_cmdCount;
}

inline void CommandBuffer::cmdBindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline) {
    assert(getState() == State::Recording && pipeline != VK_NULL_HANDLE);
    vkCmdBindPipeline(getHandle(), bindPoint, pipeline);
    ++m_cmdCount;
}

inline void CommandBuffer::cmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) {
    assert(getState() == State::Recording);
    vkCmdDraw(getHandle(), vertexCount, instanceCount, firstVertex, firstInstance);
    ++m_cmdCount;
}

inline void CommandBuffer::cmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    assert(getState() == State::Recording);
    vkCmdDrawIndexed(getHandle(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    ++m_cmdCount;
}

inline void CommandBuffer::cmdBeginDebugUtilsLabel(const char* pLabelName, float r, float g, float b, float a) const {
    const auto label = VkDebugUtilsLabelEXT {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        nullptr,
        pLabelName,
        { r, g, b, a }
    };
    vkCmdBeginDebugUtilsLabelEXT(getHandle(), &label);
}

inline void CommandBuffer::cmdEndDebugUtilsLabel(void) const {
    vkCmdEndDebugUtilsLabelEXT(getHandle());
}

inline void CommandBuffer::cmdInsertDebugUtilsLabel(const char* pLabelName, float r, float g, float b, float a) const {
    const auto label = VkDebugUtilsLabelEXT {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        nullptr,
        pLabelName,
        { r, g, b, a }
    };
    vkCmdInsertDebugUtilsLabelEXT(getHandle(), &label);
}

inline void CommandBuffer::cmdSetLineWidth(float lineWidth) const {
    vkCmdSetLineWidth(getHandle(), lineWidth);
}

inline void CommandBuffer::cmdSetBlendConstants(const float blendConstants[4]) const {
    vkCmdSetBlendConstants(getHandle(), blendConstants);
}


inline void CommandBuffer::cmdSetDepthBias(float constantFactor, float clamp, float slopeFactor) const {
    vkCmdSetDepthBias(getHandle(), constantFactor, clamp, slopeFactor);
}

inline void CommandBuffer::cmdSetDepthBounds(float minBounds, float maxBounds) const {
    vkCmdSetDepthBounds(getHandle(), minBounds, maxBounds);
}

inline void CommandBuffer::cmdBeginQuery(VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags) const {
    vkCmdBeginQuery(getHandle(), queryPool, query, flags);
}

inline void CommandBuffer::cmdEndQuery(VkQueryPool queryPool, uint32_t query) const {
    vkCmdEndQuery(getHandle(), queryPool, query);
}

inline void CommandBuffer::cmdCopyQueryPoolResults(VkQueryPool queryPool,
                                                    uint32_t firstQuery,
                                                    uint32_t queryCount,
                                                    VkBuffer dstBuffer,
                                                    VkDeviceSize dstOffset,
                                                    VkDeviceSize stride,
                                                    VkQueryResultFlags flags) const
{
    vkCmdCopyQueryPoolResults(getHandle(), queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

inline void CommandBuffer::cmdResetQueryPool(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) const {
    vkCmdResetQueryPool(getHandle(), queryPool, firstQuery, queryCount);
}

inline void CommandBuffer::cmdWriteTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query) const {
    vkCmdWriteTimestamp(getHandle(), pipelineStage, queryPool, query);
}

inline void CommandBuffer::cmdSetEvent(VkEvent event, VkPipelineStageFlags stageFlags) const {
    vkCmdSetEvent(getHandle(), event, stageFlags);
}

inline void CommandBuffer::cmdResetEvent(VkEvent event, VkPipelineStageFlags stageFlags) const {
    vkCmdResetEvent(getHandle(), event, stageFlags);
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
void vkCmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors);

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





#endif


inline const CommandBuffer* CommandBufferGroup::getBuffer(uint32_t index) const {
    assert(index < m_buffers.size());
    return &m_buffers[index];
}

inline auto CommandBufferGroup::getBuffers(void) const -> const std::vector<CommandBuffer>& {
    return m_buffers;
}


}

#endif // ELYSIAN_RENDERER_COMMAND_HPP
