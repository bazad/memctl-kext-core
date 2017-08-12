/*
 * memctl-kext-core
 * core/src/core.c
 * Brandon Azad
 *
 * A libmemctl core for macOS that uses the KernelTask.kext kernel extension to get the kernel task
 * port.
 */
#include "memctl/core.h"
#include "memctl/memctl_error.h"

#include <IOKit/IOKitLib.h>

static const char *service_name = "KernelTask";
static const uint32_t selector  = 0x6d656d63;

bool core_load() {
	io_service_t service = IOServiceGetMatchingService(kIOMasterPortDefault,
			IOServiceMatching(service_name));
	if (service == IO_OBJECT_NULL) {
		error_core("could not find service %s", service_name);
		return false;
	}
	io_connect_t connect;
	kern_return_t kr = IOServiceOpen(service, mach_task_self(), 0, &connect);
	IOObjectRelease(service);
	if (kr != KERN_SUCCESS) {
		error_core("could not open connection to service %s: %x", service_name, kr);
		return false;
	}
	uint64_t output[1];
	uint32_t count = 1;
	kr = IOConnectCallMethod(connect, selector,
			NULL, 0, NULL, 0,
			output, &count, NULL, NULL);
	IOServiceClose(connect);
	if (kr != KERN_SUCCESS) {
		error_core("%s: IOConnectCallMethod failed: %x", service_name, kr);
		return false;
	}
	kernel_task = output[0];
	return true;
}
