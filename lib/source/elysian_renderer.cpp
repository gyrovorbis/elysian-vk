#include <renderer/elysian_renderer.hpp>

namespace elysian::renderer {

Renderer::~Renderer(void) {

}

bool Renderer::initialize(const Initializer &initializer) {
    bool success = true;
    setAllocator(initializer.pAllocator);

    m_pInstanceLayerProperties = new InstanceLayerProperties();
    success &= m_pInstanceLayerProperties->isValid();
    m_pInstanceExtensionProperties = new InstanceExtensionProperties();
    success &= m_pInstanceExtensionProperties->isValid();

    m_pInstance = new Instance(initializer.instanceInitializer, getAllocator());
    success &= m_pInstance->isValid();

    success &= queryPhysicalDevices();

    for(auto&& devInitializer : initializer.deviceInitializers) {
        Device* pDevice = createDevice(devInitializer);
        success &= pDevice->isValid();
    }

    return success;
}

bool Renderer::createInstance(const Instance::Initializer& initializer) {
    bool success = true;
    success &= m_pInstanceLayerProperties->supportsLayers(initializer.info.getLayers());
    success &= m_pInstanceExtensionProperties->supportsExtensions(initializer.info.getExtensions());
    m_pInstance = new Instance(initializer, getAllocator());
    return m_pInstance->isValid();
}

bool Renderer::createDevice(const Device::Initializer& initializer) {
    Device* pDevice = new Device(initializer, getAllocator());
    m_devices.push_back(pDevice);
    return pDevice->isValid();
}

void Renderer::setAllocator(const Allocator *pAllocator) {
    m_pAllocator.reset(new Allocator(pAllocator));
}

bool Renderer::queryPhysicalDevices(void) const {
    bool success = true;
    uint32_t deviceCount = 0;
    Result result = vkEnumeratePhysicalDevices(getInstance(), &deviceCount, nullptr);
    success &= result;
    m_pPhysicalDevices = new std::vector<PhysicalDevice>(deviceCount);
    result = vkEnumeratePhysicalDevices(getInstance(), &deviceCount, m_pPhysicalDevices->data());
    success &= result;
    return success;
}

}
