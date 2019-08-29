#include "svc_dispatcher_device.h"
#include "svc_handlers_device.h"

void svc_dispatcher_device(uint32_t *svc_args) {
    uint32_t svc_number = ((char *) svc_args[6])[-2];

    /* if (svc_number == SVC_1) { */
    /*     svc_number_1_handler(svc_args[0]); */
    /* } else if (SVC_2) { */
    /*     svc_number_2_handler(svc_args[1]); */
    /* } */

    // We should never reach here.
    // TODO Panic!
}
