#include <renderer/elysian_renderer.hpp>
#include <renderer/elysian_renderer_instance.hpp>
//#include <renderer/elysian_renderer_allocator.hpp>
#include <renderer/elysian_renderer_physical_device.hpp>
#include <renderer/elysian_renderer_device.hpp>
#include <renderer/elysian_renderer_debug_log.hpp>

namespace elysian::renderer {


std::pair<Result, Version> Renderer::getInstanceVersion(void) {
    uint32_t version = 0;
    Result result = vkEnumerateInstanceVersion(&version);
    return { result, version };
}

Renderer::Renderer(Initializer* initializer):
    m_pLog(initializer->pLog),
    m_pDevices(std::make_unique<std::vector<Device>>())
{
    initialize(*initializer);
}

Renderer::~Renderer(void) {

}

bool Renderer::initialize(const Initializer &initializer) {
    bool success = true;
    setAllocator(initializer.pAllocator);

    getLog()->verbose("VkRenderer::initializing");
    getLog()->push();

    m_pInstanceLayerProperties = std::make_unique<InstanceLayerProperties>();
    m_pInstanceLayerProperties->log(getLog());
    success &= m_pInstanceLayerProperties->isValid();

    m_pInstanceExtensionProperties = std::make_unique<InstanceExtensionProperties>();
    success &= m_pInstanceExtensionProperties->isValid();
    m_pInstanceExtensionProperties->log(getLog());


    m_pInstance = std::make_unique<Instance>(*initializer.pInstanceInitializer, this);
   success &= m_pInstance->isValid();

    success &= queryPhysicalDevices();
    success &= queryPhysicalDeviceGroups();

    getLog()->pop();

    return success;
}

bool Renderer::createInstance(const InstanceInitializer& initializer) {
#if 0
    bool success = true;
    success &= m_pInstanceLayerProperties->supportsLayers(initializer.info.getLayers());
    success &= m_pInstanceExtensionProperties->supportsExtensions(initializer.info.getExtensions());
    m_pInstance = new Instance(initializer, getAllocator());
    return m_pInstance->isValid();
#endif
}


void Renderer::setAllocator(const Allocator *pAllocator) {
#if 0
    m_pAllocator.reset(new Allocator(pAllocator));
#endif
}

const PhysicalDevice* Renderer::getPhysicalDevice(int index) const {
    const PhysicalDevice* pDev = nullptr;
    if(m_pPhysicalDevices && index < m_pPhysicalDevices->size()) {
        pDev = &(*m_pPhysicalDevices)[index];
    }
    return pDev;
}

bool Renderer::queryPhysicalDevices(void) {
    bool success = true;
    uint32_t deviceCount = 0;
    std::vector<VkPhysicalDevice> vkDevices;

    getLog()->verbose("Querying Physical Devices");
    getLog()->push();

    Result result = vkEnumeratePhysicalDevices(m_pInstance->getHandle(), &deviceCount, nullptr);
    success &= result;
    if(result) {

        vkDevices.resize(deviceCount);

        result = vkEnumeratePhysicalDevices(m_pInstance->getHandle(), &deviceCount, vkDevices.data());
        success &= result;
        if(result) {
            m_pPhysicalDevices = std::make_unique<std::vector<PhysicalDevice>>();
            for(uint32_t d = 0; d < deviceCount; ++d) {
                m_pPhysicalDevices->emplace_back(vkDevices[d]);
                getLog()->verbose("Physical Device[%d]", d);
                getLog()->push();
                m_pPhysicalDevices->back().log(getLog());
                getLog()->pop();
            }

        } else {
            getLog()->error("Failed to query device handles: %s", result.toString());
        }
    } else {
        getLog()->error("Failed to get device count: %s", result.toString());
    }

    getLog()->pop();

    return success;
}

bool Renderer::queryPhysicalDeviceGroups(void) {
    uint32_t count = 0;

    getLog()->verbose("Querying PhysicalDeviceGroups");
    getLog()->push();

    std::vector<VkPhysicalDeviceGroupProperties> properties;
    Result result = vkEnumeratePhysicalDeviceGroups(getInstance()->getHandle(),
                                                    &count,
                                                    nullptr);
    if(result) {
        properties.resize(count);
        result = vkEnumeratePhysicalDeviceGroups(getInstance()->getHandle(),
                                                            &count,
                                                            properties.data());

        if(!result) {
            getLog()->error("Failed to query device groups: %s", result.toString());
        } else {

            m_pPhysicalDeviceGroups = std::make_unique<std::vector<PhysicalDeviceGroup>>();

            for(int g = 0; g < properties.size(); ++g) {
                m_pPhysicalDeviceGroups->emplace_back(properties[g], this);
                getLog()->verbose("PhysicalDeviceGroup[%d]", g);
                getLog()->push();
                m_pPhysicalDeviceGroups->back().log(getLog());
                getLog()->pop();
            }
        }

    } else {
        getLog()->error("Failed to query count: %s", result.toString());

    }

    getLog()->pop();

    return result.succeeded();

}

Device* Renderer::createDevice(const char* pName, const PhysicalDevice* pDevice, std::shared_ptr<const DeviceCreateInfo> pCreateInfo) {
    getLog()->verbose("Creating Device");
    getLog()->push();
    m_pDevices->emplace_back(pName, pDevice, std::move(pCreateInfo), this);
    m_pDevices->back().log(getLog());
    getLog()->pop();
    return &m_pDevices->back();
}

}
