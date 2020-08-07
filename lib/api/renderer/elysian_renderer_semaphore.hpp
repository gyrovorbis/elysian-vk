#ifndef ELYSIAN_RENDERER_SEMAPHORE_HPP
#define ELYSIAN_RENDERER_SEMAPHORE_HPP

namespace elysian::renderer {

class Semaphore {
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
                ~Semaphore(void);

    operator    VkSemaphore() const;

    Result      getResult(void) const;
    bool        isValid(void) const;
    VkSemaphore getHandle(void) const;
    auto        getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;


private:
    VkSemaphore     m_handle = VK_INVALID_HANDLE;
    std::shared_ptr<const CreateInfo>
                    m_pCreateInfo;
    const Device*   m_pDevice = nullptr;
};


class Fence {
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
                    ~Fence(void);

    operator        VkFence() const;

    Result          getResult(void) const;
    bool            isValid(void) const;
    VkFence         getHandle(void) const;
    auto            getCreateInfo(void) const -> std::shared_ptr<const CreateInfo>;

private:

    VkFence         m_handle = VK_INVALID_HANDLE;
    Result          m_result;
    std::shared_ptr<const CreateInfo>
                    m_pInfo;
    const Device*   m_pDevice = nullptr;

};


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
inline bool Semaphore::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }
inline VkSemaphore Semaphore::getHandle(void) const { return m_handle; }
inline std::shared_ptr<const Semaphore::CreateInfo> Semaphore::getCreateInfo(void) const { return m_pCreateInfo; }


inline Fence::Fence(Initializer initializer):
    m_pInfo(std::move(initializer.pInfo)),
    m_pDevice(initializer.pDevice)
{
    m_result = vkCreateFence(m_pDevice, m_pInfo.get(), nullptr, &m_handle);
}

inline Fence::~Fence(void) {
    vkDestroyFence(m_pDevice, nullptr, getHandle());
}

inline Result Fence::getResult(void) const { return m_result; }
inline bool Fence::isValid(void) const { return getResult() && getHandle() != VK_INVALID_HANDLE; }
inline VkFence Fence::getHandle(void) const { return m_handle; }
inline std::shared_ptr<const Fence::CreateInfo> Fence::getCreateInfo(void) const { return m_pInfo; }


}

#endif // ELYSIAN_RENDERER_SEMAPHORE_HPP
