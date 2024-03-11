#include <zephyr/kernel.h>
#include "uart.h"
#include "IS2020.h"
#include "BM83.h"
#include "time.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(BM83);
#define DEVICE_NAME "Magneta"
bool device_was_paired = true;

IS2020 BM83;

bool bm83_is_enabled = false;
void bm83_enable_device()
{
  bm83_virtual_power_on();
}
void bm83_set_enable()
{
  bm83_is_enabled = true;
}
void bm83_disable_device()
{
  uart_set_disconnect_to_bm83();
  BM83.resetModule();
  bm83_is_enabled = false;
  BM83.btmState = 0;
}

void bm83_setup_audio()
{
  // BM83.SET_BASS();
  // BM83.enable_i2s();
  // vTaskDelay(pdMS_TO_TICKS(200));
  // BM83.enable_rx_mode();
  // vTaskDelay(pdMS_TO_TICKS(200));
  // BM83.change_sample_rate_to_441();
  // vTaskDelay(pdMS_TO_TICKS(200));

  // BM83.set_hand_free_profile();
  // vTaskDelay(pdMS_TO_TICKS(200));

  // BM83.unblock_stream();
  // vTaskDelay(pdMS_TO_TICKS(200));
}

void bm83_setup_device_name()
{
  BM83.changeDeviceName(DEVICE_NAME);
}

void bm83_enable_pair_mode()
{
  BM83.mmiAction(0x00, 0x5d);
}
void bm83_reset()
{
  BM83.btmState = 0;
  BM83.eepromToDefaults(0);
  bm83_setup();
  bm83_enable_pair_mode();
}

void bm83_virtual_power_on()
{
  BM83.mmiAction(0x00, 0x51);
  k_sleep(K_MSEC(100));
  BM83.mmiAction(0x00, 0x52);
}

void bm83_virtual_power_off()
{
  BM83.btmState = 0;
  BM83.mmiAction(0x00, 0x51);
  k_sleep(K_MSEC(1000));
  BM83.mmiAction(0x00, 0x52);
}

void bm83_decode_event()
{
  BM83.getNextEventFromBt();
}

void bm83_setup()
{
  LOG_INF("SETUP");
  BM83.btmState = 0;
  bm83_virtual_power_on();
  bm83_setup_audio();
  bm83_setup_device_name();
  bm83_virtual_power_on();
  BM83.enable_ble();
}

void iso2083_make_call(char phoneNumber[19])
{
  /*
  0x00 Idle
  0x01 voice dial
  0x02 incoming call
  0x03 outgoing call
  0x04 call active
  0x05 a call active with a call waiting
  0x06 a call active with a call hold
  */
  if (BM83.callStatus[1] == 0x00)
  {
    BM83.makeCall(0, phoneNumber);
  }
  else if (BM83.callStatus[1] == 0x02)
  {
    BM83.acceptIncomingCall(0);
  }
  else if (BM83.callStatus[1] == 0x01 || BM83.callStatus[1] == 0x03 || BM83.callStatus[1] == 0x04 || BM83.callStatus[1] == 0x05 || BM83.callStatus[1] == 0x06)
  {
    BM83.forceEndActiveCall(0);
    BM83.callStatus[1] = 0x00;
  }
}

void bm83_init()
{

  while (!uart_connected_to_bm83())
  {
    bm83_virtual_power_on();
    k_sleep(K_MSEC(500));
  }
  bm83_setup();
  LOG_ERR("BM83 setup");
}

void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size)
{

  for (size_t i = 0; i < data_size; i++)
  {
    printk("%x", data[i]);
  }

  printk("\n");
}