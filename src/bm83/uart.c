#include <stdio.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <errno.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include "zephyr/drivers/uart.h"
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/drivers/gpio.h>
#include <string.h>
#include <stdio.h>
#include "BM83.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(bm83_uart);
#define UART_BUF_SIZE 1024
#define UART_TX_TIMEOUT_MS 100
#define UART_RX_TIMEOUT_MS 1000
#define BUF_SIZE (124)
#define MAX_LEN_MSG BUF_SIZE

#define MY_STACK_SIZE 10240
#define THREAD_PRIORITY K_PRIO_COOP(5)

struct k_thread uart_process_thread;
K_THREAD_STACK_DEFINE(uart_process_thread_stack_area, MY_STACK_SIZE);
void uart_start_thread();
void uart_stop_thrad();
k_tid_t process_tid;

K_SEM_DEFINE(rx_disabled, 0, 1);

bool currently_active_buffer = 1;

static const struct device *dev_uart;

uint8_t uart_double_buffer[2][UART_BUF_SIZE];
uint8_t *uart_buf_next = uart_double_buffer[1];

#define MSG_SIZE 1

K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 1000, 4);

uint8_t rx_data[MAX_LEN_MSG];
uint16_t rx_size;

uint8_t read_data[MAX_LEN_MSG];
uint16_t read_data_size;

bool bm83_sended_msg = false;
bool uart_connected_to_bm83()
{
    return bm83_sended_msg;
}
void uart_set_disconnect_to_bm83()
{
    bm83_sended_msg = false;
}

uint16_t uart_get_read_data_size()
{
    return read_data_size;
}
int uart_write_data(uint8_t *data);

void uart_rx_tx_while()
{

    while (1)
    {
        uint8_t result[100];
        uint8_t result_size = 0;
        uint8_t need_size = 100;
        bool can_fix = false;

        while (1)
        {
            uint8_t uart_buf;
            k_msgq_get(&uart_msgq, &uart_buf, K_FOREVER);

            bm83_sended_msg = true;
            bm83_set_enable();
            if (uart_buf == 0xaa)
            {
                can_fix = true;
            }
            if (can_fix)
            {
                result[result_size] = uart_buf;
                // memcpy(result[result_size],uart_buf[0],1);
                // memchr(result[result_size], (char)uart_buf[0], 1);
                // LOG_INF("size %d uart_rx  %x result %x", result_size, uart_buf, result[result_size]);

                result_size++;
                if (result_size > 3)
                {
                    need_size = 0;
                    need_size += 4;
                    need_size += result[2];
                    if (result_size >= need_size)
                    {
                        break;
                    }
                }
            }
        }
        BM83_LOG_HEXDUMP(result, result_size);
        rx_size = result_size;
        memcpy(rx_data, result_size, rx_size);
        rx_data[rx_size] = 0;
        LOG_ERR("Read %d bytes: EVENT", rx_size);
        if (result[3] != 0x00)
        {
            uint8_t ack_command[7];
            ack_command[0] = (0xaa);
            ack_command[1] = (0x00);
            ack_command[2] = (0x02);
            uint8_t checkSum = (0x02);
            ack_command[3] = (0x14);
            checkSum += (0x14);
            ack_command[4] = result[3];
            checkSum += result[3];
            ack_command[5] = (0x100 - checkSum);
            ack_command[6] = '\0';
            uart_write_data(ack_command);
        }
    }
}

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{

    uint8_t c;

    if (!uart_irq_update(dev_uart))
    {
        LOG_ERR("uart_irq_update ");
        return;
    }

    while (uart_irq_rx_ready(dev_uart))
    {
        int size = uart_fifo_read(dev_uart, &c, 1);
        /* if queue is full, message is silently dropped */
        k_msgq_put(&uart_msgq, &c, K_NO_WAIT);
    }
}

void uart_init()
{
    dev_uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

    if (!device_is_ready(dev_uart))
    {
        LOG_ERR("uart is not ready");
    }
    uart_irq_callback_user_data_set(dev_uart, uart_cb, NULL);
    uart_irq_rx_enable(dev_uart);
    uart_start_thread();
}
int uart_write_data(uint8_t *data)
{
    const int len = 3 + data[2] + 1;
    for (int i = 0; i < len; i++)
    {
        uart_poll_out(dev_uart, data[i]);
    }
    LOG_ERR("send Len: %d Write:", len);
    BM83_LOG_HEXDUMP(data, len);
    return len;
}

uint16_t uart_buffer_size()
{
    return rx_size;
}

uint8_t *uart_read_data()
{

    uint8_t result_size = 1;
    read_data[0] = rx_data[0];

    uint8_t unreaded_rx_data[MAX_LEN_MSG];
    uint8_t unreaded_rx_data_size = 0;

    for (size_t i = 1; i < rx_size; i++)
    {
        if (rx_data[i] == 0xaa)
        {
            read_data[i] = 0;
            break;
        }
        read_data[i] = rx_data[i];
        result_size++;
    }
    read_data_size = result_size;
    // LOG_ERR( "read_data_size: %d rx_size: %d ", read_data_size, rx_size);
    // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) read_data, result_size, ESP_LOG_INFO);

    for (size_t i = result_size; i < rx_size; i++)
    {
        unreaded_rx_data[unreaded_rx_data_size] = rx_data[i];
        unreaded_rx_data_size++;
    }
    unreaded_rx_data[unreaded_rx_data_size] = 0;

    // LOG_ERR( "unreaded_data_size: %d rx_size: %d ", read_data_size, rx_size);
    // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) unreaded_rx_data, unreaded_rx_data_size, ESP_LOG_INFO);

    rx_size -= result_size;

    memcpy(rx_data, unreaded_rx_data, unreaded_rx_data_size);
    // LOG_ERR( "rx_size: %d rx_data:", rx_size);
    // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) rx_data, rx_size, ESP_LOG_INFO);

    return read_data;
}

void uart_start_thread()
{
    process_tid = k_thread_create(&uart_process_thread, uart_process_thread_stack_area,
                                  K_THREAD_STACK_SIZEOF(uart_process_thread_stack_area),
                                  (k_thread_entry_t)uart_rx_tx_while,
                                  NULL, NULL, NULL,
                                  THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_start(&uart_process_thread);
}

void uart_stop_thrad()
{
    k_thread_abort(process_tid);
}