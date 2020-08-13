#ifndef ELYSIAN_RENDERER_QUEUE_HPP
#define ELYSIAN_RENDERER_QUEUE_HPP

#include <string>
#include <vector>
#include "elysian_renderer_result.hpp"
#include "elysian_renderer_object.hpp"

namespace elysian::renderer {

class Queue;
class QueueGroup;
class Device;
class Renderer;
class DebugLog;

class QueueGroupProperties {
public:
    QueueGroupProperties(const char* pName, uint32_t familyIndex):
        m_name(pName),
        m_familyIndex(familyIndex)
    {}

    const char* getName(void) const { return m_name.c_str(); }
    uint32_t getFamilyIndex(void) const { return m_familyIndex; }
private:
    std::string m_name = "None";
    uint32_t m_familyIndex;
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
struct QueueGroupCreateInfo {
    QueueGroupProperties properties;
    VkDeviceQueueCreateFlags flags;
    std::vector<QueueProperties> queueProperties;
};

class QueueSubmitInfo: public VkSubmitInfo {
    QueueSubmitInfo(std::vector<VkCommandBuffer>  cmdBuffers,
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

class Queue:
        public HandleObject<VkQueue, VK_OBJECT_TYPE_QUEUE>,
        public QueueProperties
{
public:

    struct Initializer {
        QueueProperties properties;
        int arrayIndex;
        const QueueGroup& group;
        const Device& device;
    };

    Queue(Initializer initializer, Renderer& renderer);

    uint32_t getQueueIndex(void) const { return m_queueIndex; }
    const QueueGroup& getQueueGroup(void) const { return m_group; }

    Result waitIdle(void) const;
    Result submit(std::vector<SubmitInfo> submitInfo, VkFence fence=VK_NULL_HANDLE) const;

    void beginDebugUtilsLabel(const char* pLabelName, float r=0.0f, float g=0.0f, float b=0.0f, float a=0.0f) const;
    void endDebugUtilsLabel(void) const;
    void insertDebugUtilsLabel(const char* pLabelName, float r=0.0f, float g=0.0f, float b=0.0f, float a=0.0f) const;

    void log(DebugLog* pLog) const;

private:
    uint32_t m_queueIndex;
    const QueueGroup& m_group;
};

class QueueGroup: public QueueGroupProperties {
public:

    struct Initializer {
        QueueGroupCreateInfo info;
        Device* pDevice = nullptr;
    };

    QueueGroup(Initializer initializer, Renderer& renderer):
        QueueGroupProperties(initializer.info.properties)
    {
        for(int q = 0; q < initializer.info.queueProperties.size(); ++q) {
            m_queues.push_back(Queue(Queue::Initializer{initializer.info.queueProperties[q],
                                      q,
                                      *this,
                                      *initializer.pDevice
                               }, renderer));
        }
    }

    Queue* getQueue(int index) const;
    Queue* getQueue(const char* pName) const;
    Queue* getQueueWithClosestPriority(float priority);
    auto getQueues(void) const -> const std::vector<Queue>& { return m_queues; }

    void log(DebugLog* pLog) const;

private:
    std::vector<Queue> m_queues;
};


inline void Queue::beginDebugUtilsLabel(const char* pLabelName, float r, float g, float b, float a) const {
    const auto label = VkDebugUtilsLabelEXT {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        nullptr,
        pLabelName,
        { r, g, b, a }
    };
    vkQueueBeginDebugUtilsLabelEXT(getHandle(), &label);
}

inline void Queue::endDebugUtilsLabel(void) const {
    vkQueueEndDebugUtilsLabelEXT(getHandle());
}

inline void Queue::insertDebugUtilsLabel(const char* pLabelName, float r, float g, float b, float a) const {
    const auto label = VkDebugUtilsLabelEXT {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        nullptr,
        pLabelName,
        { r, g, b, a }
    };
    vkQueueInsertDebugUtilsLabelEXT(getHandle(), &label);
}


inline Result Queue::waitIdle(void) const {
    return vkQueueWaitIdle(getHandle());
}
inline Result Queue::submit(std::vector<SubmitInfo> submitInfo, VkFence fence) const {
    return vkQueueSubmit(getHandle(), submitInfo.size(), submitInfo.data(), fence);
}


}

#endif // ELYSIAN_RENDERER_QUEUE_HPP
