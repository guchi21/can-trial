#include "wrapper.h"
#include "can_driver_irq.h"

void picocan_wrap_ind_irq() {

    candrv_ind_irq();
}