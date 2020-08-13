#include <renderer/elysian_renderer_command.hpp>
#include <renderer/elysian_renderer_device.hpp>

namespace elysian::renderer {

Result CommandPool::reset(VkCommandPoolResetFlags flags) const {
    return vkResetCommandPool(m_pDevice->getHandle(), getHandle(), flags);
}

void CommandPool::trim(VkCommandPoolTrimFlags flags) const {
    // Provided by VK_VERSION_1_1
    vkTrimCommandPool(m_pDevice->getHandle(), getHandle(), flags);
}

CommandPool::CommandPool(const Device* pDevice, const CommandPoolCreateInfo* pInfo):
    m_pDevice(pDevice)
{
    VkCommandPool pool = VK_NULL_HANDLE;
    m_result = vkCreateCommandPool(m_pDevice->getHandle(), pInfo, nullptr, &pool);
    setHandle(pool);
}

CommandPool::~CommandPool(void) {
    vkDestroyCommandPool(m_pDevice->getHandle(), getHandle(), nullptr);
}

CommandBufferGroup::CommandBufferGroup(const Device* pDevice, const CommandBufferAllocateInfo* pInfo):
    m_pDevice(pDevice),
    m_pool(pInfo->commandPool)
{
    std::vector<VkCommandBuffer> vkBuffers(pInfo->commandBufferCount, VK_NULL_HANDLE);
    m_buffers.reserve(pInfo->commandBufferCount);
    m_result = vkAllocateCommandBuffers(m_pDevice->getHandle(), pInfo, vkBuffers.data());

    for(int b = 0; b < vkBuffers.size(); ++b) {
        m_buffers.emplace_back(vkBuffers[b], this);
    }
}

CommandBufferGroup::~CommandBufferGroup(void) {
    std::vector<VkCommandBuffer> vkBuffers;
    vkBuffers.reserve(m_buffers.size());
    for(auto&& buff : m_buffers) {
        vkBuffers.push_back(buff.getHandle());
    }

    vkFreeCommandBuffers(m_pDevice->getHandle(),
                         m_pool,
                         vkBuffers.size(),
                         vkBuffers.data());
}

CommandBufferGroup* CommandPool::createGroup(VkCommandBufferLevel level, uint32_t commandBufferCount) {
    const auto info = CommandBufferAllocateInfo(getHandle(), level, commandBufferCount);
    auto* pBufferGroup = new CommandBufferGroup(m_pDevice, &info);
    return pBufferGroup;
}

}
