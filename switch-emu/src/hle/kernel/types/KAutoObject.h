#pragma once
#include <stdint.h>

#include "structutils.h"

// TODO this is kinda made up, is there something else we can do?
enum class ObjectType : uint8_t {
    NO_TYPE = 0,
    INTERFACE = 1,
    THREAD = 2,
    PROCESS = 3,
    GPU_DEVICE = 4,
    TRANSFER_MEM = 5
};

class KAutoObject {
public:
    KAutoObject() : referenceCount(0) {}

    void incRef() { referenceCount++; }
    void decRef() { referenceCount--; }
    uint64_t getRefCount() { return referenceCount; }

    virtual ~KAutoObject() {}

    virtual ObjectType getType() {
        return ObjectType::NO_TYPE;
    }

private:
    uint64_t referenceCount;
};
CHECK_SIZE(KAutoObject, 0x10);

template<typename T>
class ObjectPtr {
public:
    ObjectPtr() : object(nullptr) {}

    explicit ObjectPtr(T* pointer) noexcept : object(pointer) {
        inc();
    }

    ObjectPtr(const ObjectPtr<T>& other) noexcept : object(other.object) {
        inc();
    }

    ObjectPtr(ObjectPtr<T>&& other) noexcept : object(other.object) {
        other.object = nullptr;
    }

    ~ObjectPtr() {
        dec();
    }

    ObjectPtr<T>& operator=(const ObjectPtr<T>& other) noexcept {
        dec();
        object = other.object;
        inc();
        return *this;
    }

    bool operator==(const ObjectPtr<T>& other) const {
        return object == other.object;
    }

    explicit operator bool() const {
        return object != nullptr;
    }

    T* get() const { return object; }

    T& operator*() const noexcept { return *get(); }
    T* operator->() const noexcept { return get(); }

private:
    void inc() {
        if (object != nullptr) {
            object->incRef();
        }
    }

    void dec() {
        if (object != nullptr) {
            object->decRef();

            if (object->getRefCount() == 0) {
                delete object;
                object = nullptr;
            }
        }
    }

    T* object;
};
CHECK_SIZE(ObjectPtr<KAutoObject>, 0x8);

namespace kernel {

template<typename TypeTo, typename TypeFrom>
ObjectPtr<TypeTo> object_pointer_cast(const ObjectPtr<TypeFrom>& ptr) {
    return ObjectPtr<TypeTo>(reinterpret_cast<TypeTo*>(ptr.get()));
}

} // namespace kernel
