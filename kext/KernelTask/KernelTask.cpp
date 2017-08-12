/*
 * memctl-kext-core
 * kext/KernelTask/KernelTask.cpp
 * Brandon Azad
 *
 * A kernel extension that grants clients a send right to the kernel task.
 */
#include "KernelTask.hpp"

#include <IOKit/IOLib.h>

// IPC definitions from ipc_kobject.h and kern_types.h

typedef natural_t ipc_kobject_type_t;
typedef vm_offset_t ipc_kobject_t;
#define IKOT_TASK 2

// Symbols derived from the kernel file on disk

#include "symbol_defines.h"

// Global variables

static ipc_space_t ipc_space_kernel;
static ipc_port_t (*ipc_port_alloc_special)(ipc_space_t space);
static void (*ipc_kobject_set)(ipc_port_t port, ipc_kobject_t kobject, ipc_kobject_type_t type);
static ipc_space_t (*get_task_ipcspace)(task_t task);
static mach_port_name_t (*ipc_port_copyout_send)(ipc_port_t sright, ipc_space_t space);

static uint8_t fake_kernel_task[0x1000];

// KernelTask

OSDefineMetaClassAndStructors(KernelTask, IOService)

bool KernelTask::start(IOService *provider)
{
    bool success = IOService::start(provider);
    if (success) {
        registerService();
    }
    // Get the kernel slide.
    uint64_t slide = (uint64_t)&kernel_task - SYMBOL__kernel_task;
    // Get the values of unexported symbols.
    ipc_space_kernel       = *(ipc_space_t *)(SYMBOL__ipc_space_kernel + slide);
    ipc_port_alloc_special = (ipc_port_t (*)(ipc_space_t))(SYMBOL__ipc_port_alloc_special + slide);
    ipc_kobject_set        = (void (*)(ipc_port_t, ipc_kobject_t, ipc_kobject_type_t))(SYMBOL__ipc_kobject_set + slide);
    get_task_ipcspace      = (ipc_space_t (*)(task_t))(SYMBOL__get_task_ipcspace + slide);
    ipc_port_copyout_send  = (mach_port_name_t (*)(ipc_port_t, ipc_space_t))(SYMBOL__ipc_port_copyout_send + slide);
    // Copy the kernel task into a fake kernel task.
    memcpy(fake_kernel_task, kernel_task, sizeof(fake_kernel_task));
    return success;
}

// KernelTaskUserClient

OSDefineMetaClassAndStructors(KernelTaskUserClient, IOUserClient)

IOReturn KernelTaskUserClient::externalMethod(uint32_t selector,
                                              IOExternalMethodArguments *arguments,
                                              IOExternalMethodDispatch *dispatch,
                                              OSObject *target,
                                              void *reference) {
    if (selector != 0x6d656d63 || arguments->scalarInputCount != 0 || arguments->scalarOutputCount != 1) {
        return kIOReturnBadArgument;
    }
    // Create a new send right to the fake kernel task.
    ipc_port_t new_sright = ipc_port_alloc_special(ipc_space_kernel);
    ipc_kobject_set(new_sright, (ipc_kobject_t)fake_kernel_task, IKOT_TASK);
    // Insert the send right into the current task. If the client exits the port will be cleaned up automatically.
    mach_port_name_t port = ipc_port_copyout_send(new_sright,
                                                  get_task_ipcspace(current_task()));
    if (port == MACH_PORT_NULL) {
        return kIOReturnError;
    }
    // Return the port name to the client.
    arguments->scalarOutput[0] = port;
    return kIOReturnSuccess;
}
