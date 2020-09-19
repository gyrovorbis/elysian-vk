#ifndef ELYSIAN_RENDERER_OBJECT_HPP
#define ELYSIAN_RENDERER_OBJECT_HPP

#include <vulkan/vulkan.hpp>
#include <string>
#include "elysian_renderer_result.hpp"

namespace elysian::renderer {

class Device;

class Object {
public:
    virtual                     ~Object(void) = default;
    virtual vk::StructureType   getObjectType(void) const = 0;
    virtual const char*         getObjectTypeName(void) const = 0;
    virtual uint64_t            getObjectHandle(void) const = 0;
};

template<typename Handle>
class HandleObject: public Handle, public Object {
public:

    HandleObject(Handle handle):
        Handle(std::move(handle))
    {}

    virtual vk::StructureType getObjectType(void) const override {
        return Handle::structureType;
    }

    virtual const char* getObjectTypeName(void) const override {
        vk::to_string(getObjectType()).c_str();
    }

    virtual uint64_t getObjectHandle(void) const override {
        return reinterpret_cast<uint64_t>(m_handle);
    }
};

template<typename Handle>
class DeviceOwnedHandleObject: public HandleObject<Handle> {
public:

    DeviceOwnedHandleObject(Device* pDevice, Handle handle):
        HandleObject(std::move(handle)),
        m_pDevice(pDevice)
    {}

    Device*         getDevice(void) const { return m_pDevice; }
    const char*     getObjectName(void) const;

    Result          setObjectName(const char* pName);
    Result          setObjectTag(uint64_t tagName, size_t tagSize, const void* pTag);

private:
    std::string m_name;
    Device*     m_pDevice = nullptr;
}


class Object {
public:

    struct Tag {
        uint64_t    name;
        size_t      size;
        const void* pData;
    };

                            Object(Device* pDevice   = nullptr,
                                   uint64_t handle   = VK_NULL_HANDLE,
                                   const char* pName = nullptr);
                            Object(Object&& rhs);
                            Object(const Object& rhs);

    virtual                 ~Object(void) = default;


    const char*             getObjectName(void) const;
    const Tag*              getObjectTag(void) const;

    Device*                 getDevice(void) const;
    virtual bool            isValid(void) const;

protected:
    void                    setDevice(Device* pDevice);
    void                    setObjectHandle(uint64_t handle);
    Result                  setObjectName(const char* pName);
    Result                  setObjectTag(uint64_t tagName, size_t tagSize, const void* pTag);

    uint64_t    m_handle    = VK_NULL_HANDLE;

private:
    std::unique_ptr<const Tag>
                m_tag       = nullptr;
    std::string m_name;
    Device*     m_pDevice   = nullptr;
};

inline Object::Object(Device *pDevice, uint64_t handle, const char *pName):
    m_pDevice(pDevice),
    m_handle(handle),
    m_name(pName? pName : "")
{

}

inline Object::Object(const Object &rhs):
    m_tag(rhs.m_tag? std::make_unique<const Tag>(*rhs.m_tag.get()) : nullptr),
    m_handle(rhs.m_handle),
    m_name(rhs.m_name),
    m_pDevice(rhs.m_pDevice)
{}

inline Object::Object(Object &&rhs):
    m_tag(std::move(rhs.m_tag)),
    m_handle(rhs.m_handle),
    m_name(std::move(rhs.m_name)),
    m_pDevice(rhs.m_pDevice)
{}
inline uint64_t Object::getObjectHandle(void) const { return m_handle; }
inline const char* Object::getObjectName(void) const { return m_name.c_str(); }
inline const Object::Tag* Object::getObjectTag(void) const { return m_tag.get(); }
inline bool Object::isValid(void) const { return getObjectHandle() != VK_NULL_HANDLE; }

inline void Object::setDevice(Device *pDevice) { m_pDevice = pDevice; }
inline Device* Object::getDevice(void) const { return m_pDevice; }

inline void Object::setObjectHandle(uint64_t handle) {
    m_handle = handle;
}

template<typename HandleType, VkObjectType ObjectType>
class HandleObject: public Object {
public:
    using Handle = HandleType;

                            HandleObject(Device* pDevice   = nullptr,
                                         HandleType handle = VK_NULL_HANDLE,
                                         const char* pName = nullptr);

    operator                HandleType() const;
    operator                HandleType*();

    HandleType              getHandle(void) const;
    virtual VkObjectType    getObjectType(void) const override final;
    virtual const char*     getObjectTypeName(void) const override final;

protected:
    void                    setHandle(HandleType handle);
};

template<typename HandleType, VkObjectType ObjectType>
inline HandleObject<HandleType, ObjectType>::HandleObject(Device* pDevice, HandleType handle, const char* pName):
    Object(pDevice, reinterpret_cast<uint64_t>(handle), pName)
{}
template<typename HandleType, VkObjectType ObjectType>
inline HandleType HandleObject<HandleType, ObjectType>::getHandle(void) const { return reinterpret_cast<HandleType>(getObjectHandle()); }
template<typename HandleType, VkObjectType ObjectType>
inline VkObjectType HandleObject<HandleType, ObjectType>::getObjectType(void) const { return ObjectType; }
template<typename HandleType, VkObjectType ObjectType>
inline const char* HandleObject<HandleType, ObjectType>::getObjectTypeName(void) const { return "NIL"; }
template<typename HandleType, VkObjectType ObjectType>
inline HandleObject<HandleType, ObjectType>::operator HandleType() const { return getHandle(); }
template<typename HandleType, VkObjectType ObjectType>
inline HandleObject<HandleType, ObjectType>::operator HandleType*() { return reinterpret_cast<HandleType*>(&m_handle); }
template<typename HandleType, VkObjectType ObjectType>
inline void HandleObject<HandleType, ObjectType>::setHandle(HandleType handle) { setObjectHandle(reinterpret_cast<uint64_t>(handle)); }
}

#endif // ELYSIAN_RENDERER_OBJECT_HPP
