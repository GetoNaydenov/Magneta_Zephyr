#include "IS2020.h"
#include "MMI.h"
#include "Music.h"
#include "commands.h"
#include "events.h"
#include <cstdint>
#include <cstddef>
#include "uart.h"
#include "string.h"
#include "BM83.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(IS2020);

/*s
   Destructor
*/
IS2020::~IS2020()
{
}

IS2020::IS2020()
{
}

void IS2020::DBG(char * text)
{
  LOG_INF( "%s", text);
}

void IS2020::DBG_AVRCP(char * text)
{
  LOG_INF( "%s", text);
}

void IS2020::DBG_EVENTS(char * text)
{
  LOG_INF( "%s", text);
}

uint8_t IS2020::checkResponce(uint8_t eventId)
{
  IS2020::getNextEventFromBt();
  return true;
}

/*
 */
void IS2020::sendPacketInt(uint8_t cmd, uint8_t data)
{
  uint8_t result[7];
  // DBG((char*)"sending int: ");
  decodeCommand(cmd);
  result[0] = STARTBYTE;
  result[1] = (0x00);
  result[2] = (0x02);
  uint8_t checkSum = 0x02;
  result[3] = cmd;
  checkSum += (cmd);
  result[4] = data;
  checkSum += data;
  result[5] = (0x100 - checkSum);
  result[6] = '\0';
  uart_write_data(result);
}

void IS2020::sendPacket2Int(uint8_t cmd, uint8_t data, uint8_t data2)
{
  uint8_t result[7];
  decodeCommand(cmd);
  result[0] = STARTBYTE;
  result[1] = (0x00);
  result[2] = (0x03);
  uint8_t checkSum = 0x03;
  result[3] = cmd;
  checkSum += (cmd);
  result[4] = data;
  checkSum += data;
  result[5] = data2;
  checkSum += data2;
  result[6] = (0x100 - checkSum);
  uart_write_data(result);
}

/*

*/
void IS2020::sendPacketString(uint8_t cmd, char * str)
{
  decodeCommand(cmd);
  DBG((char*)"String: ");
  uint16_t packetSize = strlen(str) + 1; // length of string + cmd
  uint8_t result[packetSize + 4];
  result[0] = STARTBYTE;
  result[1] = (0x00);
  result[2] = (packetSize);
  uint8_t checkSum = packetSize;
  result[3] = cmd;
  checkSum += (cmd);
  uint8_t result_count = 4;
  for (uint16_t dataPos = 0; dataPos < packetSize - 1; dataPos++)
  {
    result[result_count] = (str[dataPos]);
    result_count++;
    // LOG_INF("%s",str[dataPos]);
    checkSum += str[dataPos];
  }
  result[result_count] = (0x100 - checkSum);

  LOG_ERR( "send pack STR Size: %d , result_count: %d result: %s", packetSize, result_count, result);
  // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) result, result_count, ESP_LOG_INFO);
  uart_write_data(result);
}
/*

*/
void IS2020::sendPacketArrayInt(uint16_t packetSize, uint8_t cmd, uint8_t deviceId, uint8_t data[])
{
  // DBG((char*)"sending array int: ");
  decodeCommand(cmd);
  uint8_t result[packetSize + 4];
  if (cmd == CMD_MMI_Action)
    IS2020::decodeMMI(data[3]);
  result[0] = STARTBYTE;
  result[1] = (packetSize >> 8);
  // DBG((char*)((packetSize >> 8), HEX));
  uint8_t checkSum = packetSize >> 8;
  result[2] = (packetSize & 0xFF);
  // DBG((char*)((packetSize & 0xFF), HEX));
  checkSum += (packetSize & 0xFF);
  result[3] = cmd;
  // LOG_INF("%x",(0x100 - checkSum));
  checkSum += (cmd);
  result[4] = deviceId;
  checkSum += (deviceId);
  uint8_t result_count = 5;
  for (uint16_t dataPos = 0; dataPos < packetSize - 2; dataPos++)
  {
    result[result_count] = data[dataPos];
    result_count++;
    // DBG((char*)&data[dataPos]);
    checkSum += data[dataPos];
  }
  result[result_count] = (0x100 - checkSum);
  LOG_ERR( "send pack INT Size: %d , result_count: %d result:", packetSize, result_count);
  BM83_LOG_HEXDUMP(result,result_count);
  uart_write_data(result);
}


void IS2020::sendPacketArrayChar(uint16_t packetSize, uint8_t cmd, uint8_t deviceId, char data[])
{
  DBG((char*)"sending array char: ");
  decodeCommand(cmd);
  uint8_t result[packetSize + 4];
  result[0] = STARTBYTE;
  result[1] = (packetSize >> 8);
  uint8_t checkSum = packetSize >> 8;
  result[2] = (packetSize & 0xFF);
  checkSum += (packetSize & 0xFF);
  result[3] = cmd;
  checkSum += (cmd);
  result[4] = deviceId;
  uint8_t result_count = 5;
  LOG_INF("deviceId: %x",deviceId);
  checkSum += (deviceId);
  for (uint16_t dataPos = 0; dataPos < packetSize - 2; dataPos++)
  {
    result[result_count] = data[dataPos];
    result_count++;
    // DBG((char*)&data[dataPos]);
    checkSum += data[dataPos];
  }
  result[result_count] = (0x100 - checkSum);
  LOG_ERR( "send pack INT Size: %d , result_count: %d result: ", packetSize, result_count);
  BM83_LOG_HEXDUMP(result,result_count);
  // LOG_INF("%x",(0x100 - checkSum));
  uart_write_data(result);
}

int IS2020::serialAvailable()
{
  return uart_buffer_size();
}

uint8_t IS2020::checkCkeckSum(int size, uint8_t data[])
{
  DBG_EVENTS((char*)"Event from module: ");
  decodeEvent(data[0]);

  uint8_t csum = (size >> 8);
  csum += (size & 0xFF);
  // csum += data[0];
  for (uint8_t i = 0; i < size; i++)
  { //
    csum += data[i];
    // LOG_INF("%x ,",data[i]);
  }

  if (data[size] == (0x100 - csum))
  {
    // DBG((char*)"Checksum OK");
    return true;
  }
  else
  {
    DBG((char*)"Bad checksum");
    return false;
  }
}

char * IS2020::connectionStatus(uint8_t deviceId)
{
  char * ConnectionStatus = (char*)"";
  /*
            Value Parameter Description
            0xXX 1 indicate connected
            0x01 Bit0 : A2DP profile signaling channel connected = b0000 0001
            0x02 Bit1 : A2DP profile stream channel connected = b0000 0010
            0x04 Bit2 : AVRCP profile connected = b0000 0100
            0x08 Bit3 : HF profile connected = b0000 1000
            0x10 Bit4 : SPP connected = b0001 0000
  */
  if (linkStatus[1 + deviceId] & (1 << A2DP_profile_signaling_channel_connected))
    ConnectionStatus = (char*)"A2DP profile signaling channel connected";
  if (linkStatus[1 + deviceId] & (1 << A2DP_profile_stream_channel_connected))
    ConnectionStatus = (char*)"A2DP profile stream channel connected";
  if (linkStatus[1 + deviceId] & (1 << AVRCP_profile_connected))
    ConnectionStatus = (char*)"AVRCP profile connected";
  if (linkStatus[1 + deviceId] & (1 << HF_profile_connected))
    ConnectionStatus = (char*)"HF profile connected";
  if (linkStatus[1 + deviceId] & (1 << SPP_connected))
    ConnectionStatus = (char*)"SPP connected";
  if (linkStatus[1 + deviceId] == 0)
    ConnectionStatus = (char*)"Disconnected";
  return ConnectionStatus;
}

char * IS2020::musicStatus(uint8_t deviceId)
{
  /*
            database1_play_status," SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "0x00:STOP
            0x01:PLAYING
            0x02:PAUSED
            0x03:FWD_SEEK
            0x04:REV_SEEK
            0x05:FAST_FWD
            0x06:REWIND
            0x07:WAIT_TO_PLAY
            0x08:WAIT_TO_PAUSE"
  */
  //  switch (linkStatus[3 + deviceId]) {
  switch (musicState[deviceId])
  {
  case 0x00:
    return (char*)"STOP";
    break;
  case 0x01:
    return (char*)"PLAYING";
    break;
  case 0x02:
    return (char*)"PAUSED";
    break;
  case 0x03:
    return (char*)"FWD SEEK";
    break;
  case 0x04:
    return (char*)"REV SEEK";
    break;
  case 0x05:
    return (char*)"FAST FWD";
    break;
  case 0x06:
    return (char*)"REWIND";
    break;
  case 0x07:
    return (char*)"WAIT_TO_PLAY";
    break;
  case 0x08:
    return (char*)"WAIT_TO_PAUSE";
    break;
  }
  return (char*)"Unknown";
}

char * IS2020::streamStatus(uint8_t deviceId)
{
  /*
             database1_stream_status," SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "1 indicate connected
            0x00: no stream
            0x01: stream on-going"
  */
  if (linkStatus[5 + deviceId])
    return (char*)"Streaming";
  return (char*)"No stream";
}

uint8_t IS2020::batteryLevel(uint8_t deviceId)
{
  return (currentBatteryLevel[deviceId] * 100) / maxBatteryLevel[deviceId];
}

char * IS2020::moduleState()
{
  /*
            Event Parameters: device_state  SIZE: 1 BYTE
            Value Parameter Description
            0x00  Power OFF state
            0x01  pairing state (discoverable mode)
            0x02  standby state
            0x03  Connected state with only HF profile connected
            0x04  Connected state with only A2DP profile connected
            0x05  Connected state with only SPP profile connected
            0x06  Connected state with multi-profile connected
  */
  switch (linkStatus[0])
  {
  case 0x00:
    return (char*)"Power OFF";
    break;
  case 0x01:
    return (char*)"pairing state (discoverable mode)";
    break;
  case 0x02:
    return (char*)"standby state";
    break;
  case 0x03:
    return (char*)"Connected state with only HF profile connected";
    break;
  case 0x04:
    return (char*)"Connected state with only A2DP profile connected";
    break;
  case 0x05:
    return (char*)"Connected state with only SPP profile connected";
    break;
  case 0x06:
    return (char*)"Connected state with HP and A2DP profile connected";
    break;
  }
  return (char*)"Default";
}

char * IS2020::btStatus()
{

  switch (btmState)
  {
  case 0x00:
    // return "Power OFF state";
    return (char*)"Disconected!";
    break;
  case 0x01:
    return (char*)"Pairing state (discoverable mode)";
    break;
  case 0x02:
    return (char*)"Power ON state";
    break;
  case 0x03:
    return (char*)"pairing successful";
    break;
  case 0x04:
    return (char*)"pairing fail";
    break;
  case 0x05:
    return (char*)"HF link established";
    break;
  case 0x06:
    return (char*)"A2DP link established";
    break;
  case 0x07:
    return (char*)"HF link disconnected";
    break;
  case 0x08:
    return (char*)"A2DP link disconnected";
    break;
  case 0x09:
    return (char*)"SCO link connected";
    break;
  case 0x0A:
    return (char*)"SCO link disconnected";
    break;
  case 0x0B:
    return (char*)"AVRCP link established";
    break;
  case 0x0C:
    return (char*)"AVRCP link disconnected";
    break;
  case 0x0D:
    return (char*)"Standard SPP connected";
    break;
  case 0x0E:
    return (char*)"Standard_SPP / iAP disconnected";
    break;
  case 0x0F:
    return (char*)"Standby state";
    break;
  case 0x10:
    return (char*)"iAP connected";
    break;
  case 0x11:
    return (char*)"ACL disconnected";
    break;
  default:
    char * tmp = (char*)"Unknown stat";
    // return ("unknown state" + (char)btmState);
    return (tmp + (char)btmState);
    break;
  }
}

void IS2020::setEventMask(uint32_t mask)
{
  EventMask |= mask;
  // Serial3.println(EventMask,HEX);
}

void IS2020::enableAllSettingEvent()
{
  EventMask = 0x00000000;
  setEventMask(EMB_BTM_battery_level);
  setEventMask(EMB_BTM_charging_status);
  setEventMask(EMB_BMT_reset_to_default_setting_OK);
  setEventMask(EMB_BTM_DAC_gain_level);
}

void IS2020::removeInfoAboutDevice(uint8_t deviceId)
{
  deviceName[deviceId] = 0;
  deviceInBandRingtone[deviceId] = 0;
  deviceIsIap[deviceId] = 0;
  deviceSupportAvrcpV13[deviceId] = 0;
  deviceHfAndA2dpGain[deviceId] = 0;
  deviceLineInGain[deviceId] = 0;
  maxBatteryLevel[deviceId] = 0;
  currentBatteryLevel[deviceId] = 0;

  btmStatusChanged = 1;
}
