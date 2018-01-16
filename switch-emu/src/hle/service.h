#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include "hle\ipc.h"
#include "hle\error.h"
#include "log.h"

#define SERVICE_FUNC_STUB(func_name) \
uint32_t func_name(ipc::IpcInfo* info, ipc::IpcMessageCons* reply) { \
    logger::error("Unimplemented command called for %s: %s(%u)", getName().c_str(), #func_name, info->cmdId); \
    return ERR_SUCCESS; \
}

// TODO redo all of this once there is a better understanding of the IPC
// TODO better organization for the namespaces, especially as we implement all the interfaces...
namespace service {

void init();

class Interface;

// TODO what is the actual object the kernel uses? K{Client, Server, ""}Session?
struct InterfaceHandle : public KAutoObject {
    Interface* interface;
    bool isDomain;

    InterfaceHandle(Interface* interface_, bool isDomain_) : interface(interface_), isDomain(isDomain_) {}

    virtual ObjectType getType() {
        return ObjectType::INTERFACE;
    }
};

// TODO no
extern std::unordered_map<std::string, Interface*> gNamedPorts;

// TODO other things can handle sync requests (like files), there should be a parent interface for that
class Interface {

public:
    using InterfaceFunc = uint32_t(Interface::*)(ipc::IpcInfo*, ipc::IpcMessageCons*);

    struct FunctionEntry {
        uint32_t id;
        InterfaceFunc func;
        const char* name;
    };

public:
    virtual std::string getName() {
        return "UNKNOWN";
    }

    void handleSyncRequest(ipc::IpcInfo* info);

    handle_t connect();

protected:
    // TODO can this be avoided?
    void registerFunctions(const std::vector<FunctionEntry>& funcs);

private:
    std::vector<FunctionEntry> mFunctions;
};

void addNamedPort(Interface* interface, std::string name);
void addService(Interface* interface, std::string name);

} // namespace service
