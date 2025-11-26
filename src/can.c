#include <can2040.h>
#include <hardware/regs/intctrl.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include <queue.h>
#include "pico/cyw43_arch.h"

static struct can2040 cbus;

bool on;

QueueHandle_t msgs;

static void can2040_cb(struct can2040 *cd, uint32_t notify, struct can2040_msg *msg)
{
    xQueueSendToBack(msgs, msg, portMAX_DELAY);
}

static void PIOx_IRQHandler(void)
{
    can2040_pio_irq_handler(&cbus);
}

void canbus_setup(void)
{
    uint32_t pio_num = 0;
    uint32_t sys_clock = 125000000, bitrate = 500000;
    uint32_t gpio_rx = 4, gpio_tx = 5;

    // Setup canbus
    can2040_setup(&cbus, pio_num);
    can2040_callback_config(&cbus, can2040_cb);

    // Enable irqs
    irq_set_exclusive_handler(PIO0_IRQ_0, PIOx_IRQHandler);
    irq_set_priority(PIO0_IRQ_0, PICO_DEFAULT_IRQ_PRIORITY - 1);
    irq_set_enabled(PIO0_IRQ_0, 1);

    // Start canbus
    can2040_start(&cbus, sys_clock, bitrate, gpio_rx, gpio_tx);
}

// --------------------------------------------
// RECEIVER TASK
// --------------------------------------------
void receiver_task(__unused void *params)
{
    printf("Receiver ready.\n");
    
    while(1)
    {
        struct can2040_msg msg;
        
        xQueueReceive(msgs, &msg, portMAX_DELAY);
        
        printf("Received Message: ");
        printf("%d %d %d %d\n",
        (msg.data[0]),
        (msg.data[1]),
        (msg.data[2]),
        (msg.data[3]));
    }
}

// --------------------------------------------
// TRANSMITTER TASK
// --------------------------------------------
void transmitter_task(__unused void *params)
{
    struct can2040_msg msg;

    while (true) {
        msg.id = 0;
        msg.dlc = 4;

        msg.data[0] = 1; 
        msg.data[1] = 2;
        msg.data[2] = 3;
        msg.data[3] = 4;

        printf("Sending CAN frame.\n");
        can2040_transmit(&cbus, &msg);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

// Visual identification reference that the code is runnung.
void blinker(__unused void *args)
{
    while (true) 
    {
        on = !on;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
        vTaskDelay(500);
    }
}

int main(void)
{
    stdio_init_all();

    sleep_ms(2000);
    
    cyw43_arch_init();

    msgs = xQueueCreate(100, sizeof(struct can2040_msg));
    canbus_setup();

    TaskHandle_t rec_task, transmit_task, blink;

    /*
    To run the code, comment out either the receive or transmit task,build and load it onto the first pico, then comment out the
    task you just loaded, and uncomment the other task, rebuild the code and load it onto the second pico.
    */
    //xTaskCreate(receiver_task, "ReceiverThread", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &rec_task);

    xTaskCreate(transmitter_task, "TransmitterThread", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &transmit_task);

    xTaskCreate(blinker, "BlinkThread", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, &blink);

    vTaskStartScheduler();
    return 0;
}