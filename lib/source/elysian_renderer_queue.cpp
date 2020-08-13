#include <renderer/elysian_renderer_queue.hpp>
#include <renderer/elysian_renderer_device.hpp>
#include <renderer/elysian_renderer_debug_log.hpp>

namespace elysian::renderer {

Queue::Queue(Initializer initializer, Renderer& renderer):
    QueueProperties(initializer.properties),
    m_queueIndex(initializer.arrayIndex),
    m_group(initializer.group)
{
    VkQueue queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(initializer.device.getHandle(), initializer.group.getFamilyIndex(), getQueueIndex(), &queue);
    setHandle(queue);
}

void Queue::log(DebugLog *pLog) const {
    pLog->verbose("Name: %s", getName());
    pLog->verbose("Priority: %f", getPriority());
}

void QueueGroup::log(DebugLog* pLog) const {
    pLog->verbose("Name: %s", getName());
    pLog->verbose("Family Index: %u", getFamilyIndex());
    pLog->verbose("Queues");
    pLog->push();
    for(int q = 0; q < m_queues.size(); ++q) {
        pLog->verbose("Queue[%d]", q);
        pLog->push();
        m_queues[q].log(pLog);
        pLog->pop();
    }
    pLog->pop();
}

}
