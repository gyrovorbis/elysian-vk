#ifndef ELYSIAN_RENDERER_QUERY_HPP
#define ELYSIAN_RENDERER_QUERY_HPP

#include "elysian_renderer_object.hpp"

namespace elysian::renderer {


class QueryPoolCreateInfo: public VkQueryPoolCreateInfo {
public:
    QueryPoolCreateInfo(VkQueryPoolCreateFlags flags, VkQueryType queryType, uint32_t queryCount, VkQueryPipelineStatisticFlags pipelineStatistics):
        VkQueryPoolCreateInfo({
            VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
            nullptr,
            flags,
            queryType,
            queryCount,
            pipelineStatistics
        })
    {}
};

class QueryPool: public HandleObject<VkQueryPool, VK_OBJECT_TYPE_QUERY_POOL> {
public:

                QueryPool(const Device* pDevice, std::shared_ptr<const QueryPoolCreateInfo> pInfo);
    virtual     ~QueryPool(void);

    Result      getResult(void) const;

    Result      getResults(uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) const;
    void        reset(uint32_t firstQuery, uint32_t queryCount) const;

    auto        getCreateInfo(void) const -> std::shared_ptr<const QueryPoolCreateInfo>;
private:
    std::shared_ptr<const QueryPoolCreateInfo>
                    m_pInfo     = nullptr;
    const Device*   m_pDevice   = nullptr;
    Result          m_result;
};


inline QueryPool::QueryPool(const Device *pDevice, std::shared_ptr<const QueryPoolCreateInfo> pInfo):
    m_pInfo(std::move(pInfo)),
    m_pDevice(pDevice)
{
    VkQueryPool pool = VK_NULL_HANDLE;
    m_result = vkCreateQueryPool(m_pDevice->getHandle(), m_pInfo->get(), nullptr, &pool);
    setHandle(pool);
}

inline QueryPool::~QueryPool(void) {
    vkDestroyQueryPool(m_pDevice->getHandle(), getHandle(), nullptr);
}

inline std::shared_ptr<const QueryPoolCreateInfo> QueryPool::getCreateInfo(void) const {
    return m_pInfo;
}

inline Result QueryPool::getResult(void) const { return m_result; }

inline Result QueryPool::getResults(uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags) const {
    return vkGetQueryPoolResults(m_pDevice->getHandle(), getHandle(), firstQuery, queryCount, dataSize, pData, stride, flags);
}

inline void QueryPool::reset(uint32_t firstQuery, uint32_t queryCount) const {
    vkResetQueryPool(m_pDevice->getHandle(), getHandle(), firstQuery, queryCount);
}

}

#endif // ELYSIAN_RENDERER_QUERY_HPP
