#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <inttypes.h>
#include "button.h"
#include "iso2083.h"
#define MODULE button
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(MODULE);
#define SLEEP_TIME_MS 1
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
                                                              {0});
static struct gpio_callback button_cb_data;
bool button_pressed = false;
static uint64_t button_press_time_in_sec = 0;
char phoneNumber[19] = {'0', '8', '7', '8', '9', '1', '2', '9', '4', '7', 0, 0, 0, 0, 0, 0, 0, 0, 0};

void button_processed(uint64_t press_time_ms)
{
  if (press_time_ms >= BUTTON_PRESS_VERY_LONG_TIME_MS)
  {
    bm83_enable_pair_mode();
  }
  else if (press_time_ms <= BUTTON_PRESS_LONG_TIME_MS)
  {

    iso2083_make_call(phoneNumber);
  }
}

void button_pressed_cb(const struct device *dev, struct gpio_callback *cb,
                       uint32_t pins)
{

  if (button_pressed == true)
  {
    button_pressed = false;

    button_processed((k_uptime_get() - button_press_time_in_sec) / 1000);
  }
  else
  {
    button_pressed = true;
    button_press_time_in_sec = k_uptime_get();
  }
}

int button_init()
{
  int ret = 0;

  if (!device_is_ready(button.port))
  {
    LOG_ERR("Error: button device %s is not ready",
            button.port->name);
    return -1;
  }

  ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
  if (ret != 0)
  {
    LOG_ERR("Error %d: failed to configure %s pin %d",
            ret, button.port->name, button.pin);
    return -2;
  }

  ret = gpio_pin_interrupt_configure_dt(&button,
                                        GPIO_INT_EDGE_BOTH);
  if (ret != 0)
  {
    LOG_ERR("Error %d: failed to configure interrupt on %s pin %d",
            ret, button.port->name, button.pin);
    return -3;
  }

  gpio_init_callback(&button_cb_data, button_pressed_cb, BIT(button.pin));
  gpio_add_callback(button.port, &button_cb_data);

  return 0;
}
