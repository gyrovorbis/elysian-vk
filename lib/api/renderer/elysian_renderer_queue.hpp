#ifndef ELYSIAN_RENDERER_QUEUE_HPP
#define ELYSIAN_RENDERER_QUEUE_HPP

#include "elysian_renderer_result.hpp"

namespace elysian::renderer {

class QueueGroupProperties {
public:
    Properties(const char* pName, uint32_t familyIndex):
        m_name(pName),
        m_familyIndex(familyIndex)
    {}

    const char* getName(void) const { return m_name.c_str(); }
    uint32_t getFamilyIndex(void) const { return m_familyIndex; }
private:
    std::string m_name = "None";
    uint32_t m_familyIndex;
};

class QueueGroup: public QueueGroupProperties {
public:

    struct CreateInfo {
        QueueGroupProperties properties;
        VkDeviceQueueCreateFlags flags;
        std::vector<Queue::Properties> queueProperties;
    };

    struct Initializer {
        CreateInfo info;
        Device* pDevice = nullptr;
    };

    QueueGroup(Initializer initializer, Renderer& renderer):
        QueueGroupProperties(std::move(initializer.info.properties))
    {
        for(int q = 0; q < initializer.info.queueProperties.size(); ++q) {
            m_queues.push_back(Queue({std::move(initializer.info.queueProperties[q]),
                                      q,
                                      this,
                                      initializer.pDevice
                               }, renderer));
        }
    }

    Queue* getQueue(int index) const;
    Queue* getQueue(const char* pName) const;
    Queue* getQueueWithClosestPriority(float priority);
    const auto getQueues(void) const -> std::vector<Queue>& { return m_queues; }

private:
    std::vector<Queue> m_queues;
};


struct QueueProperties {
public:
    QueueProperties(const char* pName, float priority):
        m_name(pName),
        m_priority(priority)
    {}

    const char* getName(void) const { return m_name.c_str(); }
    float getPriority(void) const { return m_priority; }

protected:
    std::string m_name = "None";
    float m_priority = 1.0f;
};

class Queue: public QueueProperties  {
public:

    class SubmitInfo: public VkSubmitInfo {
        SubmitInfo(std::vector<VkCommandBuffer>  cmdBuffers,
                   std::vector<VkSemaphore>      waitSemaphores,
                   std::vector<VkPipelineStageFlags>    waitDstStageMasks,
                   std::vector<VkSemaphore>      signalSemaphores):
            VkSubmitInfo({
                VK_STRUCTURE_TYPE_SUBMIT_INFO,
                nullptr,
                         /*
                 uint32_t                       waitSemaphoreCount;
                 const VkSemaphore*             pWaitSemaphores;
                 const VkPipelineStageFlags*    pWaitDstStageMask;
                 uint32_t                       commandBufferCount;
                 const VkCommandBuffer*         pCommandBuffers;
                 uint32_t                       signalSemaphoreCount;
                 const VkSemaphore*             pSignalSemaphores;
                                  */
            })
        {}

    };

    struct Initializer {
        QueueProperties properties;
        int arrayIndex;
        const QueueGroup& group;
        const Device& device;
    };

    Queue(Initializer initializer, Renderer& renderer):
        QueueProperties(std::move(initializer.properties)),
        m_queueIndex(initializer.arrayIndex)
    {
        vkGetDeviceQueue(initializer.device, initializer.group.getFamilyIndex(), getQueueIndex(), &m_queue);
    }

    bool isValid(void) const;

    uint32_t getQueueIndex(void) const;
    const QueueGroup& getQueueGroup(void) const { return m_group; }

    Result waitIdle(void) const;
    Result submit(std::vector<SubmitInfo> submitInfo, VkFence fence=VK_INVALID_HANDLE) const;

private:
    uint32_t m_queueIndex;
    const QueueGroup& m_group;
    VkDeviceQueue m_queue = VK_INVALID_HANDLE;
};



inline Result Queue::waitIdle(void) const {
    return vkQueueWaitIdle(this);
}
inline Result Queue::submit(std::vector<SubmitInfo> submitInfo, VkFence fence) const {
    return vkQueueSubmit(this, submitInfo.size(), submitInfo.data(), fence);
}


}

#endif // ELYSIAN_RENDERER_QUEUE_HPP
