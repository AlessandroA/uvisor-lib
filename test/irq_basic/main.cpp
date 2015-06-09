#include "mbed/test_env.h"
#include "uvisor-lib/uvisor-lib.h"

/* main box ACLs */
static const UvisorBoxAclItem g_main_acl[] = {
    {MCG,                  sizeof(*MCG),       UVISOR_TACLDEF_PERIPH},
    {UART0,                sizeof(*UART0),     UVISOR_TACLDEF_PERIPH},
    {PIT,                  sizeof(*PIT),       UVISOR_TACLDEF_PERIPH},
    {SIM,                  sizeof(*SIM),       UVISOR_TACLDEF_PERIPH},
    {PORTB,                sizeof(*PORTB),     UVISOR_TACLDEF_PERIPH},
};

/* enable uvisor */
UVISOR_SET_MODE_ACL(2, g_main_acl);

#define TEST_IRQn 42
volatile int g_flag;

void test_handler(void)
{
    g_flag++;
}

int main(void)
{
    MBED_HOSTTEST_TIMEOUT(5);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(IRQ_BASIC);
    MBED_HOSTTEST_START("MBED_A1");

    g_flag = 0;

    /* test setting a ISR */
    uvisor_isr_set(TEST_IRQn, (uint32_t) &test_handler, 0);
    if(uvisor_isr_get(TEST_IRQn) != (uint32_t) &test_handler)
    {
        MBED_HOSTTEST_RESULT(0);
    }

    /* test enabling an IRQ */
    uvisor_irq_enable(TEST_IRQn);
    uvisor_irq_set_pending(TEST_IRQn);
    __DSB();
    __ISB();
    if(g_flag != 1)
    {
        MBED_HOSTTEST_RESULT(0);
    }

    /* test disabling an IRQ */
    uvisor_irq_disable(TEST_IRQn);
    uvisor_irq_set_pending(TEST_IRQn);
    __DSB();
    __ISB();
    if(g_flag != 1)
    {
        MBED_HOSTTEST_RESULT(0);
    }

    /* if the test got here, everything is fine */
    MBED_HOSTTEST_RESULT(1);
}
