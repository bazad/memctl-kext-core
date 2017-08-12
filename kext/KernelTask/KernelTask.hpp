/*
 * memctl-kext-core
 * kext/KernelTask/KernelTask.hpp
 * Brandon Azad
 *
 * A kernel extension that grants clients a send right to the kernel task.
 */
#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>

class KernelTask : IOService {
    OSDeclareDefaultStructors(KernelTask);
public:
    virtual bool start(IOService *provider) override;
};

class KernelTaskUserClient : IOUserClient {
    OSDeclareDefaultStructors(KernelTaskUserClient);
public:
    virtual IOReturn externalMethod(uint32_t selector,
                                    IOExternalMethodArguments *arguments,
                                    IOExternalMethodDispatch *dispatch = NULL,
                                    OSObject *target = NULL,
                                    void *reference = NULL) override;
};
