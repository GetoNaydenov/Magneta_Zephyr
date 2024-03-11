#include "app.h"
#include "board.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

void main(){

  LOG_INF("MAGNETA FW ver: %d.%d.%d", FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_PATCH);
  app_start();
}