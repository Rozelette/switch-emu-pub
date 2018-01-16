#pragma once
#include <stdint.h>

#include "hle\kernel\types\KAutoObject.h"
#include "hle\kernel\types\KHandleEntry.h"
#include "hle\kernel\handle.h"

namespace kernel::handleman {

handle_t createHandle(ObjectPtr<KAutoObject> object);

template<typename T>
handle_t createHandle(ObjectPtr<T> object) {
    return createHandle(kernel::object_pointer_cast<KAutoObject>(object));
}

// TODO is there a way to have safer access without casting afterwards?
ObjectPtr<KAutoObject> lookupObject(handle_t handle);

bool closeHandle(handle_t handle);

} // namespace kernel::handleman
