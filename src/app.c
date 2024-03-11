#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include "bma400_service.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "iso2083.h"
#include "button.h"
LOG_MODULE_REGISTER(app);

#define MY_STACK_SIZE 8192
K_THREAD_STACK_DEFINE(my_stack_area1, MY_STACK_SIZE);
struct k_thread bm400_process_thread;

static const struct gpio_dt_spec bma400_interrupt = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), bma400int1_gpios);
static const struct gpio_dt_spec bm83_interrupt = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), bm83int1_gpios);

void bm83_interrupted(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  LOG_INF("BM83 INTERUPT");
}

void bm400_interrupted(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
  LOG_INF("BMA400 INTERUPT");
}

void enable_bm83()
{
  struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

  gpio_pin_configure(gpio_dev, 2, GPIO_OUTPUT_HIGH);
  k_sleep(K_MSEC(100));
  gpio_pin_configure(gpio_dev, 2, GPIO_OUTPUT_LOW);
}

void app_start()
{
  static struct gpio_callback bma400_interrupt_cb_data;
  LOG_INF("DEVICE START");
  gpio_pin_interrupt_configure_dt(&bma400_interrupt, GPIO_INT_EDGE_FALLING);
  gpio_init_callback(&bma400_interrupt_cb_data, bm400_interrupted, BIT(bma400_interrupt.pin));
  gpio_add_callback(bma400_interrupt.port, &bma400_interrupt_cb_data);

  static struct gpio_callback bm83_interrupt_cb_data;

  gpio_pin_interrupt_configure_dt(&bm83_interrupt, GPIO_INT_EDGE_FALLING);
  gpio_init_callback(&bm83_interrupt_cb_data, bm83_interrupted, BIT(bm83_interrupt.pin));
  gpio_add_callback(bm83_interrupt.port, &bm83_interrupt_cb_data);

  LOG_INF("INterupt ready");
  uart_init();
  enable_bm83();
  bm83_init();
  button_init();
  LOG_INF("BM83 START");
  // bma400_service_init();

  while (1)
  {

    LOG_INF("Hello");
    k_sleep(K_MSEC(1000));
  }
}
