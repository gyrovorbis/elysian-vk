#ifndef ELYSIAN_RENDERER_ALLOCATOR_HPP
#define ELYSIAN_RENDERER_ALLOCATOR_HPP

namespace elysian::renderer {

//AMD has an open-source implementation?
class Allocator: public VkAllocationCallbacks {
public:
    Allocator(void):
        VkAllocationCallbacks({
            this,
            &Allocator::allocationCallback,
            &Allocator::reallocationCallback,
            &Allocator::freeCallback,
            &Allocator::internalAllocationNotificationCallback,
            &Allocator::internalFreeNotificationCallback
        })
    {}

    operator const VkAllocationCallbacks*() {
        return nullptr;
    }

    bool isDefault(void) const { return true; }

    void* allocate(size_t  size,
        size_t  alignment,
        VkSystemAllocationScope allocationScope) {
        return nullptr;
    }

    void* reallocate(void* pOriginal,
        size_t size,
        size_t alignment,
        VkSystemAllocationScope allocationScope) {
        return nullptr;
    }

    void free(void* pMemory) {}

    void internalAllocateNotification(size_t size,
        VkInternalAllocationType allocationType,
                                      VkSystemAllocationScope allocationScope) {}

    void internalFreeNotification(size_t  size,
        VkInternalAllocationType allocationType,
                                  VkSystemAllocationScope allocationScope) {}

protected:
    void* allocationCallback(void* pUserData,
        size_t  size,
        size_t  alignment,
        VkSystemAllocationScope allocationScope);

    void* reallocationCallback(void* pUserData,
        void* pOriginal,
        size_t size,
        size_t alignment,
        VkSystemAllocationScope allocationScope);

    void freeCallback(void* pUserData, void* pMemory);

    void internalAllocationNotificationCallback(void* pUserData,
        size_t size,
        VkInternalAllocationType allocationType,
        VkSystemAllocationScope allocationScope);

    void internalFreeNotificationCallback(void* pUserData,
        size_t  size,
        VkInternalAllocationType allocationType,
        VkSystemAllocationScope allocationScope);
private:


};

}

#endif // ELYSIAN_RENDERER_ALLOCATOR_HPP
