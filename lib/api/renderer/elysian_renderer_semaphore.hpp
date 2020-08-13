#ifndef ELYSIAN_RENDERER_SEMAPHORE_HPP
#define ELYSIAN_RENDERER_SEMAPHORE_HPP

#include "elysian_renderer_object.hpp"

namespace elysian::renderer {

class Semaphore: public HandleObject<VkSemaphore, VK_OBJECT_TYPE_SEMAPHORE> {
public:
    class CreateInfo: public VkSemaphoreCreateInfo {
    public:
        CreateInfo(VkSemaphoreCreateFlags flags=0): //flags reserved for future use...
            VkSemaphoreCreateInfo({
                VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                nullptr,
                flags
            })
        {}
    };

    struct Initializer {
        std::shared_ptr<const CreateInfo> pCreateInfo;
        const Device* pDevice;
    };

                Semaphore(Initializer Initializer);
    virtual     ~Semaphore(void);

    Result      getResult(void) const;
    auto        getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;

#if 0
    VkResult
      vkGetSemaphoreCounterValue( VkDevice device, VkSemaphore semaphore, uint64_t * pValue ) const VULKAN_HPP_NOEXCEPT
    {
      return ::vkGetSemaphoreCounterValue( device, semaphore, pValue );
    }

    VkResult vkSignalSemaphore(
        VkDevice                                    device,
        const VkSemaphoreSignalInfo*                pSignalInfo);

    // Provided by VK_VERSION_1_2
    VkResult vkWaitSemaphores(
        VkDevice                                    device,
        const VkSemaphoreWaitInfo*                  pWaitInfo,
        uint64_t                                    timeout);

#endif


private:
    std::shared_ptr<const CreateInfo>
                    m_pCreateInfo;
    const Device*   m_pDevice = nullptr;
    Result          m_result;
};


class Fence: public HandleObject<VkFence, VK_OBJECT_TYPE_FENCE> {
public:

    class CreateInfo: public VkFenceCreateInfo {
    public:
        CreateInfo(VkFenceCreateFlags flags):
            VkFenceCreateInfo({
                VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                nullptr,
                flags
            })
        {}
    };

    struct Initializer {
        std::shared_ptr<const CreateInfo> pInfo;
        const Device* pDevice;
    };

                    Fence(Initializer Initializer);
    virtual         ~Fence(void);

    Result          getResult(void) const;
    Result          getStatus(void) const;
    auto            getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;

private:

    Result          m_result;
    std::shared_ptr<const CreateInfo>
                    m_pInfo;
    const Device*   m_pDevice = nullptr;

};

class Event: public HandleObject<VkEvent, VK_OBJECT_TYPE_EVENT> {
public:
                    Event(const Device* pDevice);
    virtual         ~Event(void);

    Result          getResult(void) const;

    Result          getStatus(void) const;
    Result          set(void) const;
    Result          reset(void) const;

private:
    const Device*   m_pDevice = nullptr;
    Result          m_result;
};

inline Event::Event(const Device *pDevice) :
    m_pDevice(pDevice)
{
    const auto info = VkEventCreateInfo {
        VK_STRUCTURE_TYPE_EVENT_CREATE_INFO, nullptr, 0
    };
    VkEvent event = VK_NULL_HANDLE;
    m_result =  vkCreateEvent(m_pDevice->getHandle(),
                &info,
                nullptr,
                &event);
    setHandle(event);
}

inline Event::~Event(void) {
    vkDestroyEvent(m_pDevice->getHandle(), getHandle(), nullptr);
}

inline Result Event::getResult(void) const { return m_result; }

inline Result Event::getStatus(void) const {
    return vkGetEventStatus(m_pDevice->getHandle(), getHandle());
}

inline Result Event::set(void) const {
    return vkSetEvent(m_pDevice->getHandle(), getHandle());
}

inline Result Event::reset(void) const {
    return vkResetEvent(m_pDevice->getHandle(), getHandle());
}

inline Semaphore::Semaphore(Initializer initializer):
    m_pCreateInfo(std::move(initializer.pCreateInfo)),
    m_pDevice(initializer.pDevice)
{
    m_result = vkCreateSemaphore(m_pDevice, m_pCreateInfo.get(), nullptr, &m_handle);
}

inline Semaphore::~Semaphore(void) {
    vkDestroySemaphore(m_pDevice, nullptr, getHandle());
}

inline Result Semaphore::getResult(void) const { return m_result; }
inline std::shared_ptr<const Semaphore::CreateInfo> Semaphore::getCreateInfo(void) const { return m_pCreateInfo; }


inline Fence::Fence(Initializer initializer):
    m_pInfo(std::move(initializer.pInfo)),
    m_pDevice(initializer.pDevice)
{
    m_result = vkCreateFence(m_pDevice, m_pInfo.get(), nullptr, &m_handle);
}

inline Result Fence::getStatus(void) const {
    return vkGetFenceStatus(m_pDevice->getHandle(), getHandle());
}

inline Fence::~Fence(void) {
    vkDestroyFence(m_pDevice, nullptr, getHandle());
}

inline Result Fence::getResult(void) const { return m_result; }
inline std::shared_ptr<const Fence::CreateInfo> Fence::getCreateInfo(void) const { return m_pInfo; }


}

#endif // ELYSIAN_RENDERER_SEMAPHORE_HPP
