#include "kernel.h"
#include "timeout.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_assert.h"
#include "device_init.h"
#include "em_core.h"
#include <stdint.h>
#include <stdbool.h>

extern uint32_t __Vectors;
extern uint32_t __StackTop;
void Default_Handler(void);
void sys_tick_isr();

typedef union {
    void (*pFunc)(void);
    void *topOfStack;
} tVectorEntry;

// All 37 entries of a RAM-based interrupt vector table.
// Instruct gcc to put it in the vtable section, which will be placed
// by the linker at the start of RAM.
tVectorEntry __attribute__((section ("vtable"))) ram_vector_table[] = {
    /* Cortex-M Exception Handlers */
    { .topOfStack = &__StackTop },              /* Initial Stack Pointer */
    { Reset_Handler             },              /* Reset Handler */
    { NMI_Handler               },              /* NMI Handler */
    { HardFault_Handler         },              /* Hard Fault Handler */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { SVC_Handler               },              /* SVCall Handler */
    { Default_Handler           },              /* Reserved */
    { Default_Handler           },              /* Reserved */
    { PendSV_Handler            },              /* PendSV Handler */
    { sys_tick_isr              },              /* SysTick Handler */

    /* External interrupts */

    { DMA_IRQHandler            },              /* 0 */
    { GPIO_EVEN_IRQHandler      },              /* 1 */
    { TIMER0_IRQHandler         },              /* 2 */
    { ACMP0_IRQHandler          },              /* 3 */
    { ADC0_IRQHandler           },              /* 4 */
    { I2C0_IRQHandler           },              /* 5 */
    { GPIO_ODD_IRQHandler       },              /* 6 */
    { TIMER1_IRQHandler         },              /* 7 */
    { USART1_RX_IRQHandler      },              /* 8 */
    { USART1_TX_IRQHandler      },              /* 9 */
    { LEUART0_IRQHandler        },              /* 10 */
    { PCNT0_IRQHandler          },              /* 11 */
    { RTC_IRQHandler            },              /* 12 */
    { CMU_IRQHandler            },              /* 13 */
    { VCMP_IRQHandler           },              /* 14 */
    { MSC_IRQHandler            },              /* 15 */
    { AES_IRQHandler            },              /* 16 */
    { USART0_RX_IRQHandler      },              /* 17 */
    { USART0_TX_IRQHandler      },              /* 18 */
    { USB_IRQHandler            },              /* 19 */
    { TIMER2_IRQHandler         },              /* 20 */
};

// This is the ISR for SysTick once the kernel is initialized.
// For now, it's called every 1 ms.
void sys_tick_isr() {
    tick_announce(1); // Anounce that one tick has passed.
}

void prepare_kernel() {
    /* Setup SysTick Timer for 1 msec interrupts  */
    if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
        EFM_ASSERT(false);
        while (true) {
        }
    }

    // Enable clock for GPIO module.
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    CORE_InitNvicVectorTable(
            &__Vectors,
            sizeof(ram_vector_table) / sizeof(ram_vector_table[0]),
            (uint32_t*) &ram_vector_table,
            sizeof(ram_vector_table) / sizeof(ram_vector_table[0]),
            &Default_Handler,
            false
            );

    NVIC_SetPriority(GPIO_ODD_IRQn, 1);
    NVIC_SetPriority(GPIO_EVEN_IRQn, 1);

    // Device-specific init stuff.
    device_init();
}
