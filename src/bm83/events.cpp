#include "events.h"
#include <cstdint>
#include "uart.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(BM83_event);

uint8_t IS2020::decodeBtState()
{

  switch (IS2020::btmState)
  {
  case BTM_STATE_power_OFF:
  {
    DBG_EVENTS((char *)"Power OFF.");
  }
  break;
  case BTM_STATE_discoverable_mode:
  {
    DBG_EVENTS((char *)"Discoverable mode.");
  }
  break;
  case BTM_STATE_power_ON_state:
  {
    DBG_EVENTS((char *)"Power ON.");
  }
  break;
  case BTM_STATE_pairing_successful:
  {
    DBG_EVENTS((char *)"Pairing successful.");
  }
  break;
  case BTM_STATE_pairing_fail:
  {
    DBG_EVENTS((char *)"Pairing failed.");
  }
  break;
  case BTM_STATE_HF_link_established:
  {
    DBG_EVENTS((char *)"HF link established.");
  }
  break;
  case BTM_STATE_A2DP_link_established:
  {
    DBG_EVENTS((char *)"A2DP link established.");
  }
  break;
  case BTM_STATE_HF_link_disconnected:
  {
    DBG_EVENTS((char *)"HF link disconnected.");
  }
  break;
  case BTM_STATE_A2DP_link_disconnected:
  {
    DBG_EVENTS((char *)"AD2P link disconnected.");
  }
  break;
  case BTM_STATE_SCO_link_connected:
  {
    DBG_EVENTS((char *)"SCO link connected.");
  }
  break;
  case BTM_STATE_SCO_link_disconnected:
  {
    DBG_EVENTS((char *)"SCO link disconnected.");
  }
  break;
  case BTM_STATE_AVRCP_link_established:
  {
    DBG_EVENTS((char *)"AVRCP link established.");
  }
  break;
  case BTM_STATE_AVRCP_link_disconnected:
  {
    DBG_EVENTS((char *)"AVRCP link disconnected.");
  }
  break;
  case BTM_STATE_SPP_connected:
  {
    DBG_EVENTS((char *)"SSP link connected.");
  }
  break;
  case BTM_STATE_SPP_iAP_disconnected:
  {
    DBG_EVENTS((char *)"SCO iAP link connected.");
  }
  break;
  case BTM_STATE_standby:
  {
    DBG_EVENTS((char *)"BTM standby.");
  }
  break;
  case BTM_STATE_iAP_connected:
  {
    DBG_EVENTS((char *)"iAP connected.");
  }
  break;
  case BTM_STATE_ACL_disconnected:
  {
    DBG_EVENTS((char *)"ACL disconnected.");
  }

  case BTM_STATE_BTM_STATE_MAP_connected:
  {
    DBG_EVENTS((char *)"BTM_STATE_BTM_STATE_MAP_connected:");
  }
  break;
  case BTM_STATE_MAP_operation_forbidden:
  {
    DBG_EVENTS((char *)"BTM_STATE_MAP_operation_forbidden:");
  }
  break;
  case BTM_STATE_MAP_disconnected:
  {
    DBG_EVENTS((char *)"BTM_STATE_MAP_disconnected:");
  }
  break;
  case BTM_STATE_ACL_connected:
  {
    DBG_EVENTS((char *)"BTM_STATE_ACL_connected:");
  }
  break;
  case BTM_STATE_SPP_iAP_disconnected_no_other_Profile:
  {
    DBG_EVENTS((char *)"BTM_STATE_SPP_iAP_disconnected_no_other_Profile:");
  }
  break;
  case BTM_STATE_Link_back_ACL_BT_paging:
  {
    DBG_EVENTS((char *)"BTM_STATE_Link_back_ACL_BT_paging:");
  }
  break;
  case BTM_STATE_Inquiry_State:
  {
    DBG_EVENTS((char *)"BTM_STATE_Inquiry_State:");
  }
  break;
  case BTM_STATE_Current_audio_source_is_not_Aux_in_or_A2DP:
  {
    DBG_EVENTS((char *)"BTM_STATE_Current_audio_source_is_not_Aux_in_or_A2DP:");
  }
  break;
  case BTM_STATE_Current_audio_source_is_Aux_in:
  {
    DBG_EVENTS((char *)"BTM_STATE_Current_audio_source_is_Aux_in:");
  }
  break;
  case BTM_STATE_Current_audio_source_is_A2DP:
  {
    DBG_EVENTS((char *)"BTM_STATE_Current_audio_source_is_A2DP:");
  }
  break;

    break;
  }
  IS2020::btmStatusChanged = 0;
  return 0;
}

uint8_t IS2020::getNextEventFromBt()
{
  // delay(100);
  uint16_t data_size;
  if (uart_buffer_size() > 3)
  { // min is 3, 0x55 + packet size, then we can read in while loop
    // DBG_EVENTS((char*)"Get next Event from BT");

    uint8_t *data;
    data = uart_read_data();
    data_size = uart_get_read_data_size();
    // LOG_ERR( "GET DATA: size: %d", data_size);
    // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) data, data_size, ESP_LOG_INFO);
    if (data[0] == 0xAA)
    { // start of event

      uint16_t packetSize = data[2];
      // LOG_ERR( "packetSize: %d", packetSize);
      uint8_t event[packetSize + 1]; // data+cksum

      for (int i = 0; i < packetSize + 1; i++)
      {
        event[i] = data[i + 3];
      }
      // LOG_INF( "event:");
      // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) event, packetSize + 1, ESP_LOG_INFO);
      // read only if buffer has some data

      // if (checkCkeckSum(packetSize, event))
      // {

      switch (event[0])
      {
      /*
         Event Format: Event       Event Code  Event Parameters
                       Command ACK 0x00        cmd_id, status

         Description:

         Event Parameters:
            cmd_id       SIZE: 1 BYTE

            Value Parameter Description
            0xXX  the command id to ack

        status  SIZE: 1 BYTE
        Value Parameter Description
        0x00  command complete : BTM can handle this command.
        0x01  command disallow : BTM can not handle this command.
        0x02  unknow command
        0x03  parameters error
        0x04  "BTM is busy:
        This status is used for SPP data cannot be sent out in this moment because of ACL Tx buffer or RFCOMM credit issue. BTM will reply the ""Complete"" status once the SPP data can be processed.
        "
        0x05  "BTM memory is full:
        This status is used for SPP data cannot be sent out in this moment because of os heap memory is full. BTM will reply the ""Complete"" status once the SPP data can be processed. "
        others  RFD

      */
      case EVT_Command_ACK:
      {
        decodeCommandInEvents(event[1]);
        switch (event[2])
        {
        case 0x00:
          DBG_EVENTS((char *)"Command complete"); // : BTM can handle this command.");
          break;
        case 0x01:
          DBG_EVENTS((char *)"Command disallowed"); // : BTM can not handle this command.");
          if (event[1] == CMD_Vendor_AT_Command)
            allowedSendATcommands = 1;
          break;
        case 0x02:
          DBG_EVENTS((char *)"Unknow command");
          break;
        case 0x03:
          DBG_EVENTS((char *)"Parameters error");
          break;
        case 0x04:
          DBG_EVENTS((char *)"BTM is busy");
          break;
        case 0x05:
          DBG_EVENTS((char *)"BTM memory is full");
          break;
        }
        // decodeCommandInEvents(event[1]);
        // DBG_EVENTS((char*)" "+String(event[1], HEX));
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        BTM_Status  0x01  state, link_info

        Description:  This event is used to indicate about the BTM status. BTM will send it event while the status was changed.

        Event Parameters: state SIZE: 1 BYTE
        Value Parameter Description
        0x00  Power OFF state
        0x01  pairing state (discoverable mode)
        0x02  Power ON state
        0x03  pairing successful
        0x04  pairing fail
        0x05  HF link established
        0x06  A2DP link established
        0x07  HF link disconnected
        0x08  A2DP link disconnected
        0x09  SCO link connected
        0x0A  SCO link disconnected
        0x0B  AVRCP link established
        0x0C  AVRCP link disconnected
        0x0D  Standard SPP connected
        0x0E  Standard_SPP / iAP disconnected
        0x0F  Standby state
        0x10  iAP connected
        0x11  ACL disconnected

        link_info SIZE: 1 BYTE
        state:0x04
        Value Parameter Description
        0xXX  "For pairing not complete(0x04) case, this parameter indicate not completed reason
        0: time out
        1: fail

        "
        state:0x05,0x06,0x0B,0x0D
        Value Parameter Description
        0xXX  "this parameter indicate both link device and data base information.
        The format is shown as below:
        Bit7~4 : linked device id(0~7)
        Bit3~0 : linked data base(0 or 1)

        "
        state:0x07~0x08,0x0C,0x0E
        Value Parameter Description
        0xXX  "his parameter show the linked_data_base (0 or 1)


        "
        state:0x11
        Value Parameter Description
        0xXX  "0:disconnection
        1:link loss


        "

      */
      case EVT_BTM_Status:
      {
        // IS2020::btmStatusChanged = 1;
        IS2020::btmState = event[1];
        IS2020::btmLinkInfo = event[2];
        switch (btmState)
        {
        case BTM_STATE_power_OFF:
        {
          DBG_EVENTS((char *)"Power OFF.");
        }
        break;
        case BTM_STATE_discoverable_mode:
        {
          DBG_EVENTS((char *)"Discoverable mode.");
        }
        break;
        case BTM_STATE_power_ON_state:
        {
          DBG_EVENTS((char *)"Power ON.");
        }
        break;
        case BTM_STATE_pairing_successful:
        {
          DBG_EVENTS((char *)"Pairing successful.");
        }
        break;
        case BTM_STATE_pairing_fail:
        {
          DBG_EVENTS((char *)"Pairing failed.");
          //                    Serial.println("Pairing failed, reason: " + event[2]?"timeout":"fail");
          if (pairingFailReason != event[2])
            pairingFailReason = event[2];
        }
        break;
        case BTM_STATE_HF_link_established:
        {
          DBG_EVENTS((char *)"HF link established.");
        }
        break;
        case BTM_STATE_A2DP_link_established:
        {
          DBG_EVENTS((char *)"A2DP link established.");
        }
        break;
        case BTM_STATE_HF_link_disconnected:
        {
          DBG_EVENTS((char *)"HF link disconnected.");
        }
        break;
        case BTM_STATE_A2DP_link_disconnected:
        {
          DBG_EVENTS((char *)"AD2P link disconnected.");
        }
        break;
        case BTM_STATE_SCO_link_connected:
        {
          DBG_EVENTS((char *)"SCO link connected.");
        }
        break;
        case BTM_STATE_SCO_link_disconnected:
        {
          DBG_EVENTS((char *)"SCO link disconnected.");
        }
        break;
        case BTM_STATE_AVRCP_link_established:
        {
          DBG_EVENTS((char *)"AVRCP link established.");
        }
        break;
        case BTM_STATE_AVRCP_link_disconnected:
        {
          DBG_EVENTS((char *)"AVRCP link disconnected.");
        }
        break;
        case BTM_STATE_SPP_connected:
        {
          DBG_EVENTS((char *)"SSP link connected.");
        }
        break;
        case BTM_STATE_SPP_iAP_disconnected:
        {
          DBG_EVENTS((char *)"SCO iAP link connected.");
        }
        break;
        case BTM_STATE_standby:
        {
          DBG_EVENTS((char *)"BTM standby.");
        }
        break;
        case BTM_STATE_iAP_connected:
        {
          DBG_EVENTS((char *)"iAP connected.");
        }
        break;
        case BTM_STATE_ACL_disconnected:
        {
          DBG_EVENTS((char *)"ACL disconnected.");
        }

        case BTM_STATE_BTM_STATE_MAP_connected:
        {
          DBG_EVENTS((char *)"BTM_STATE_BTM_STATE_MAP_connected:");
        }
        break;
        case BTM_STATE_MAP_operation_forbidden:
        {
          DBG_EVENTS((char *)"BTM_STATE_MAP_operation_forbidden:");
        }
        break;
        case BTM_STATE_MAP_disconnected:
        {
          DBG_EVENTS((char *)"BTM_STATE_MAP_disconnected:");
        }
        break;
        case BTM_STATE_ACL_connected:
        {
          DBG_EVENTS((char *)"BTM_STATE_ACL_connected:");
        }
        break;
        case BTM_STATE_SPP_iAP_disconnected_no_other_Profile:
        {
          DBG_EVENTS((char *)"BTM_STATE_SPP_iAP_disconnected_no_other_Profile:");
        }
        break;
        case BTM_STATE_Link_back_ACL_BT_paging:
        {
          DBG_EVENTS((char *)"BTM_STATE_Link_back_ACL_BT_paging:");
        }
        break;
        case BTM_STATE_Inquiry_State:
        {
          DBG_EVENTS((char *)"BTM_STATE_Inquiry_State:");
        }
        break;
        case BTM_STATE_Current_audio_source_is_not_Aux_in_or_A2DP:
        {
          DBG_EVENTS((char *)"BTM_STATE_Current_audio_source_is_not_Aux_in_or_A2DP:");
        }
        break;
        case BTM_STATE_Current_audio_source_is_Aux_in:
        {
          DBG_EVENTS((char *)"BTM_STATE_Current_audio_source_is_Aux_in:");
        }
        break;
        case BTM_STATE_Current_audio_source_is_A2DP:
        {
          DBG_EVENTS((char *)"BTM_STATE_Current_audio_source_is_A2DP:");
        }
        break;

          break;
        }
        // IS2020::readLinkStatus(); // read info about paired devices
        IS2020::btmStatusChanged = 1;
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        Call_Status 0x02  data_base_index, call_status

        Description:  This event is used to indicate about the HF call status of BTM.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        call_status SIZE: 1 BYTE
        Value Parameter Description
        0x00  Idle
        0x01  voice dial
        0x02  incoming call
        0x03  outgoing call
        0x04  call active
        0x05  a call active with a call waiting
        0x06  a call active with a call hold

      */
      case EVT_Call_Status:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::callStatusChanged = 1;
        IS2020::callStatus[1] = event[2];

        switch (event[2])
        {
        case BTM_STATE_power_OFF:
        {
          DBG_EVENTS((char *)"0x00  Idle");
        }
        break;
        case BTM_STATE_discoverable_mode:
        {
          DBG_EVENTS((char *)"0x01  voice dial");
        }
        break;
        case BTM_STATE_power_ON_state:
        {
          DBG_EVENTS((char *)"0x02  incoming call");
        }
        break;
        case BTM_STATE_pairing_successful:
        {
          DBG_EVENTS((char *)"0x03  outgoing call");
        }
        break;
        case BTM_STATE_pairing_fail:
        {
          DBG_EVENTS((char *)"0x04  call active");
          // active voice transfer
          // IS2020::mmiAction(0x00, 0x0e);
          // // active Noise reduction
          // IS2020::dspNrCtrl(0x18);
          // IS2020::dspNrCtrl(0x1b);
          // // active gain
          // IS2020::setOverallGain();
        }
        break;
        case BTM_STATE_HF_link_established:
        {
          DBG_EVENTS((char *)"0x05  a call active with a call waiting");
        }
        break;
        case BTM_STATE_A2DP_link_established:
        {
          DBG_EVENTS((char *)"0x06  a call active with a call hold");
        }
        break;
        }
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        Caller_ID 0x03  data_base_index, num

        Description:  This event is used to indicate about the caller ID of the incoming call.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        num SIZE: N OCTETS
        Value Parameter Description
        0xXX..  caller Id or phone number

      */
      case EVT_Caller_ID:
      {
        IS2020::btmStatusChanged = 1;
        for (uint8_t i = 0; i < packetSize; i++)
        {
          IS2020::callerId[event[1]][packetSize] = event[i + 2]; // no need to any sort of flag bit, cose we have callstatus change flag and in case of incoming call we just read this array
        }
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        SMS_Received_Indication 0x04  data_base_index, indication

        Description:  This event is used to indicate about the sms status that BTM received from mobile phone.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        indication  SIZE: 1 BYTE
        Value Parameter Description
        0x00  no new sms received
        0x01  new sms received

      */
      case EVT_SMS_Received_Indication:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::smsStatus[event[1]] = event[2];
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        Missed_Call_Indication  0x05  data_base_index, information

        Description:  This event is used to indicate about the missed call indication that BTM received from mobile phone

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        information SIZE: 1 BYTE
        Value Parameter Description
        0xXX  reserved

      */
      case EVT_Missed_Call_Indication:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::callStatusChanged = 1;
        IS2020::missedCallStatus[event[1]] = event[2];
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        Phone_Max_Battery_Level 0x06  data_base_index, battery_level

        Description:  This event is used to indicate about the mobile phone max battery level that BTM received from mobile phone.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        battery_level SIZE: 1 BYTE
        Value Parameter Description
        0xXX  max battery level

      */
      case EVT_Phone_Max_Battery_Level:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::maxBatteryLevel[event[1]] = event[2];
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        Phone_Current_Battery_Level 0x07  data_base_index, battery_level

        Description:  This event is used to indicate about the mobile phone current battery level that BTM received from mobile phone.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        battery_level SIZE: 1 BYTE
        Value Parameter Description
        0xXX  current battery level

      */
      case EVT_Phone_Current_Battery_Level:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::currentBatteryLevel[event[1]] = event[2];
      }
      break;
      /*
        Event Format:  Event Event Code  Event Parameters
        Roaming_Status  0x08  data_base_index, status

        Description:  This event is used to indicate about the roaming status that BTM received from mobile phone.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        status  SIZE: 1 BYTE
        Value Parameter Description
        0x00  non-roaming
        0x01  roaming

      */
      case EVT_Roaming_Status:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::roamingStatus[event[1]] = event[2];
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        Phone_Max_Signal_Strength_Level 0x09  data_base_index, signal_level

        Description:  This event is used to indicate about the max signal strength level that BTM received from mobile phone.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        signal_level  SIZE: 1 BYTE
        Value Parameter Description
        0xXX  phone's max signal strength level

      */
      case EVT_Phone_Max_Signal_Strength_Level:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::maxSignalLevel[event[1]] = event[2];
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        Phone_Current_Signal_Strength_Level 0x0A  data_base_index, signal_strength

        Description:  This event is used to indicate about the signal strength level that BTM received from mobile phone.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        This event is used to indicate the MCU about the current signal strength level that BTM received from mobile phone. SIZE: 1 BYTE
        Value Parameter Description
        0xXX  phone's current signal strength level

      */
      case EVT_Phone_Current_Signal_Strength_Level:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::currentSignalLevel[event[1]] = event[2];
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        Phone_Service_Status  0x0B  data_base_index, service

        Description:  This event is used to indicate about the service status that BTM received from mobile phone.

        Event Parameters: data_base_index SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        service SIZE: 1 BYTE
        Value Parameter Description
        0x00  No service available
        0x01  service available

      */
      case EVT_Phone_Service_Status:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::serviceStatus[event[1]] = event[2];
      }
      break;
      /*
        Event Format: Event               Event Code  Event Parameters
                      BTM_Battery_Status  0x0C        battery_status, voltage_level

        Description:  This event is used to indicate about the BTM's battery status.

        Event Parameters: battery_status  SIZE: 1 BYTE
        Value Parameter Description
        0x00  dangerous level, and will auto shutdown
        0x01  low level
        0x02  normal level
        0x03  high level
        0x04  full level
        0x05  in charging
        0x06  charging complete

        voltage_level SIZE: 1 BYTE
        Value Parameter Description
        0xXX  "0x00: 3.0V
        0x01: 3.1V
        0x02: 3.2V
        0x03: 3.3V
        0x04: 3.4V
        0x05: 3.5V
        0x06: 3.6V
        0x07: 3.7V
        0x08: 3.8V
        0x09: 3.9V
        0x0A: 4.0V
        0x0B: 4.1V
        0x0C: 4.2V"

      */
      case EVT_BTM_Battery_Status:
      {
        IS2020::btmStatusChanged = 1;
        IS2020::btmBatteryStatus[0] = event[1];
        IS2020::btmBatteryStatus[1] = event[2];
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        BTM_Charging_Status 0x0D  charger_status, dummy

        Description:

        Event Parameters: charger_status  SIZE: 1 BYTE
        Value Parameter Description
        0x00  No charger plug in
        0x01  in charging
        0x02  charging complete
        0x03  charging fail

        dummy SIZE: 1 BYTE
        Value Parameter Description
        0xXX  reserved

      */
      case EVT_BTM_Charging_Status:
      {
        IS2020::btmChargingStatus = event[1];
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        Reset_To_Default  0x0E  dummy1,dummy2

        Description:  This event is used to indicate the BTM finish the Master Reset command for the MMI command (0x56) trigger.

        Event Parameters: dummy1  SIZE: 1 BYTE
        Value Parameter Description
        0x00

        dummy2  SIZE: 1 BYTE
        Value Parameter Description
        0x00

      */
      case EVT_Reset_To_Default:
      {
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        Report_HF_Gain_Level  0x0F  database_index, level

        Description:  This is used to report the HF gain level set by remote Audio Gateway (Phone)

        Event Parameters:
        database_index  SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate HF/HS device
        0x01  database 1 for a dedicate HF/HS device

        level     SIZE: 1 BYTE
        Value Parameter Description
        0x00-0x0F gain level that sync. with HF device

      */
      case EVT_Report_HF_Gain_Level:
      {
        IS2020::hfGainLevel[event[1]] = event[2];
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        EQ_Mode_Indication  0x10  mode, dummy

        Description:  This event is used to notice the EQ_mode setting by MMI or EQ_Mode_Setting command

        Event Parameters: mode  SIZE: 1 BYTE
        Value Parameter Description
        0x00  Off Mode
        0x01  Soft Mode
        0x02  Bass Mode
        0x03  Treble Mode
        0x04  Classical Mode
        0x05  Rock Mode
        0x06  Jazz Mode
        0x07  Pop Mode
        0x08  Dance Mode
        0x09  R&B Mode
        0x0A  User Mode 1

        dummy SIZE: 1 BYTE
        Value Parameter Description
        0xXX  reserved

      */
      case EVT_EQ_Mode_Indication:
      {
        IS2020::eqMode = event[1];
      }
      break;
      case EVT_PBAP_Missed_Call_History:
      {
      }
      break;
      case EVT_PBAP_Received_Call_History:
      {
      }
      break;
      case EVT_PBAP_Dialed_Call_History:
      {
      }
      break;
      case EVT_PBAP_Combine_Call_History:
      {
      }
      break;
      /*
        Event Format:	Event	Event Code	Event Parameters
        Get_PB_By_AT_Cmd_Reply	0x15	contact

        Description:

        Event Parameters:	contact	SIZE: N BYTE
        Value	Parameter Description
         0x…	AT Command format phone book contacts
      */
      case EVT_Get_PB_By_AT_Cmd_Reply:
      {
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        PBAP_Access_Finish  0x16  type, reserved

        Description:

        Event Parameters: type  SIZE: 1 BYTE
        Value Parameter Description
        0x0B  Retrieve miss call history records finish
        0x0C  Retrieve answered call history records finish
        0x0D  Retrieve dialed call history records finish
        0x0E  Retrieve combined call(missed, dialed,answered) history records finish
        0x0F  Retrieve phone book contacts finish


        reserved  SIZE: 1 BYTE
        Value Parameter Description
        0xXX  reserved

      */
      case EVT_PBAP_Access_Finish:
      {
        IS2020::pbapAccessFinish = event[1];
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        Read_Linked_Device_Information_Reply  0x17  database_index, type, info

        Description:  This event is used to reply Read_Linked_Device_Information command.

        Event Parameters: database_index  SIZE: 1 BYTE
        Value Parameter Description
        0x00  database 0 for a dedicate link
        0x01  database 1 for a dedicate link

        type  SIZE: 1 BYTE
        Value Parameter Description
        0x00  reply device name
        0x01  reply in-band ringtone status
        0x02  reply if remote device is a iAP device
        0x03  reply if remote device support AVRCP v1.3
        0x04  reply HF&A2DP gain
        0x05  reply Line_In gain
        others  reserved

        info (for device name)  SIZE: N OCTETS
        Value Parameter Description
        0xXX… "N bytes bluetooth name with NULL terminated. (N <= 249 with NULL terminated)

        Note:If remote device response empty name string, then BTM will report name with data NULL terminate(0x00) only."

        info (for in-band ringtone status)  SIZE: 1 BYTE
        Value Parameter Description
        0x00  disable
        0x01  enable

        info (for spp type : iAP device or not) SIZE: 1 BYTE
        Value Parameter Description
        0x00  Standard SPP device
        0x01  iAP device

        info (for AVRCP type : AVRCP v1.3 device or not)  SIZE: 1 BYTE
        Value Parameter Description
        0x00  not support AVRCP v1.3
        0x01  support AVRCP v1.3

        info (for type 0x04)  SIZE: 1 BYTE
        Value Parameter Description
        0xXX  "bit[3:0]: A2DP gain
        bit[7:4]: HF gain"

        info (for type 0x05)  SIZE: 1 BYTE
        Value Parameter Description
        0xXX  Line_In gain

      */
      case EVT_Read_Linked_Device_Information_Reply:
      {
        // IS2020::readLocalBtAddress();
        // DBG_EVENTS((char*)"Read_Linked_Device_Information_Reply");
        IS2020::btmStatusChanged = 1;
        switch (event[2])
        {          // event[1] is device id
        case 0x00: // reply device name
        {
          IS2020::deviceName[event[1]] = 0; // clear stored named
          // DBG_EVENTS((char*)"Reply device name");
          // N bytes bluetooth name with NULL terminated. (N <= 249 with NULL terminated)

          for (uint8_t i = 3; i < /*DEVICENAME_LENGHT_SUPPORT + 2*/ packetSize - 1; i++)
          { // event[2] is information type definition, last is checksum
            // check if data are not NULL
            if (event[i] == 0x00)
              break;
            // deviceName[event[1]][i - 3] = event[i];
            // Serial3.write(event[i]);
            IS2020::deviceName[event[1]] = IS2020::deviceName[event[1]] + (char)event[i]; // string append
          }
        }
        break;
        case 0x01: // reply in-band ringtone status
          /*
            info (for in-band ringtone status) SIZE: 1 BYTE
            Value Parameter Description
            0x00  disable
            0x01  enable
          */
          IS2020::deviceInBandRingtone[event[1]] = event[3];
          break;
        case 0x02: // reply if remote device is a iAP device
          /*
            info (for spp type : iAP device or not) SIZE: 1 BYTE
            Value Parameter Description
            0x00  Standard SPP device
            0x01  iAP device
          */
          IS2020::deviceIsIap[event[1]] = event[3];
          break;
        case 0x03: // reply if remote device support AVRCP v1.3
          IS2020::deviceSupportAvrcpV13[event[1]] = event[3];
          break;
        case 0x04: // reply HF&A2DP gain
          /*
            info (for type 0x04) SIZE: 1 BYTE
            Value Parameter Description
            0xXX  "bit[3:0]: A2DP gain
            bit[7:4]: HF gain"
          */
          IS2020::deviceHfAndA2dpGain[event[1]] = event[3];
          break;
        case 0x05: // reply Line_In gain
          /*
            info (for type 0x05)  SIZE: 1 BYTE
            Value Parameter Description
            0xXX  Line_In gain
          */
          IS2020::deviceLineInGain[event[1]] = event[3];
          break;
        default:
          break;

          // Serial3.print("Reserved response: ");
          // Serial3.println(event[1], HEX);
        }
      }
      break;
      /*
        Event Format: Event Event Code  Event Parameters
        Read_BTM_Version_Reply  0x18  type, version

        Description:

        Event Parameters: type  SIZE: 1 BYTE
        Value Parameter Description
        0x00  uart version
        0x01  BTM FW version

        version SIZE: 2 Octets
        Value Parameter Description
        0xXXYY
        1st byte bit[7:5]: flash version
        1st byte bit[4:0]: rom version
        2nd byte bit[7:4] : flash sub version
        2nd byte bit[3:0] : flash control version
        for example 00 07 means version 0.07

      */
      case EVT_Read_BTM_Version_Reply:
      {
        if (event[1] == 0x00)
          IS2020::btmUartVersion = (event[2] << 8) | (event[3] & 0xff);
        if (event[1] == 0x01)
          IS2020::btmFwVersion = (event[2] << 8) | (event[3] & 0xff);
      }
      break;
      case EVT_Call_List_Report:
      {
      }
      break;
      case EVT_AVRCP_Specific_Rsp:
      {
        decodeAvrcpPdu(event[8]);
        uint8_t deviceID = event[1];
        // event[2] && 0x0F Ctype
        // event[3] && B11111000 Subunit_type
        // event[3] && B00000111 Subunit_ID
        // event[4] Opcode
        // event[5][6][7] Company ID
        // event[8] PduID
        // event[9] && B11111000 Reserved
        // event[9] && B00000111 Packet Type
        // evemt[10][11] packet Length
        // event[12] event ID

        // uint16_t parameter_length = (event[10] << 8) | (event[11] & 0xff);

        if (event[2] == AVRCP_EVENT_RESPONSE_REJECTED)
        {
          DBG_AVRCP((char *)"Error: ");
          IS2020::decodeRejectReason(event[12]);
          IS2020::decodeAvrcpPdu(event[8]);

          for (uint16_t i = 0; i < packetSize; i++)
          {
            LOG_INF( "%x,", event[i]);
          }

          return false;
        }
        char *tmp;        // play/pause etc....
        switch (event[8]) // event[9] is always 0x00 10+11=size of sending responce
        {
        case AVRCP_REGISTER_NOTIFICATION:
        {
          DBG_AVRCP((char *)"Response: ");
          switch (event[2])
          { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
          case AVRCP_EVENT_RESPONSE_STABLE:
            DBG_AVRCP((char *)"STABLE: ");
            break;
          case AVRCP_EVENT_RESPONSE_REJECTED:
            DBG_AVRCP((char *)"REJECTED: ");
            break;
          case AVRCP_EVENT_RESPONSE_NOTIFY:
            DBG_AVRCP((char *)"NOTIFY: ");
            break;
          case AVRCP_EVENT_RESPONSE_INTERIM:
            DBG_AVRCP((char *)"INTERIM: ");
            break;
          case AVRCP_EVENT_RESPONSE_CHANGED:
            DBG_AVRCP((char *)"CHANGED: ");
            break;
          }
          IS2020::decodeAvrcpEvent(event[12]);
          switch (event[12]) // eventID
          {
          case AVRCP_EVENT_PLAYBACK_STATUS_CHANGED:
          {
            /*
              Description:
              Indicates the current status of
              playback

              Values:
              0x00: STOPPED
              0x01: PLAYING
              0x02: PAUSED
              0x03: FWD_SEEK
              0x04: REV_SEEK
              0xFF: ERROR
            */
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            {
              IS2020::musicState[deviceID] = event[13];
              IS2020::btmStatusChanged = 1;
              {
                switch (event[13])
                {
                case 0x00:
                  DBG_AVRCP((char *)"STOPPED");
                  break;
                case 0x01:
                  DBG_AVRCP((char *)"PLAYING");
                  break;
                case 0x02:
                  DBG_AVRCP((char *)"PAUSED");
                  break;
                case 0x03:
                  DBG_AVRCP((char *)"FWD_SEEK");
                  break;
                case 0x04:
                  DBG_AVRCP((char *)"REV_SEEK");
                  break;
                case 0xFF:
                  DBG_AVRCP((char *)"ERROR");
                  break;
                }
              }
              IS2020::avrcpRegNotifyPlaybackStatusChanged(deviceID);
            }
            break;
            }
          }
          break;
          case AVRCP_EVENT_TRACK_CHANGED:
          {
            /*Description:
              Unique Identifier to identify an
              element on TG, as is used for
              GetElementAttribute command in
              case Browsing is not supported
              and GetItemAttribute command in
              case Browsing is supported.

              Values:
              If a track is selected, then return 0x0
              in the response. If no track is
              currently selected, then return
              0xFFFFFFFFFFFFFFFF in the
              INTERIM response.

              If Browsing is not supported and a
              track is selected, then return 0x0 in
              the response. If Browsing is
              supported, then 0x0 is not allowed.

              If Browsing is supported and a track
              is selected, the identifier shall
              correspond to the currently played
              media element as listed in the
              NowPlaying folder.
            */
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            { // re-active event
              IS2020::avrcpRegNotifyTrackChanged(deviceID);
              IS2020::avrcpRegNotifyTrackPositionChanged(deviceID);
              // IS2020::avrcpGetElementAttributesAll(event[1]);
              IS2020::avrcpGetElementAttributes(event[1]);
            }
            break;
            }
          }
          break;
          case AVRCP_EVENT_TRACK_REACHED_END:
          {
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            { // re-active event
              IS2020::avrcpRegNotifyTrackReachedEnd(deviceID);
              // IS2020::avrcpGetElementAttributesAll(event[1]);
            }
            break;
            }
          }
          break;
          case AVRCP_EVENT_TRACK_REACHED_START:
          {
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            { // re-active event
              IS2020::avrcpRegNotifyTrackReachedStart(deviceID);
              // IS2020::avrcpRegNotifyTrackPositionChanged(deviceID);
              // IS2020::avrcpGetElementAttributesAll(event[1]);
            }
            break;
            }
          }
          break;
          case AVRCP_EVENT_PLAYBACK_POS_CHANGED:
          {
            // DBG_AVRCP((char*)" ["); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP((char*)String(event[i], HEX) + ",");  DBG_AVRCP((char*)"]");

            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            {
              // re-active event
              IS2020::avrcpRegNotifyTrackPositionChanged(deviceID);
              uint32_t time = ((uint32_t)event[13] << 24) | ((uint32_t)event[14] << 16) | ((uint32_t)event[15] << 8) | event[16];
              if (time != 0xFFFFFFFF)
              {
                /*
                   this ^^^^^^^^^^^^^ should be in INTERIM responce only but guess what:
                   Response: CHANGED: Decoded AVRCP event: AVRCP_EVENT_PLAYBACK_POS_CHANGED
                   [1a,0,d,48,0,0,19,58,31,0,0,5,5,ff,ff,ff,ff,]
                   4294967.00s

                   from specs:

                   If no track currently selected, then
                   return 0xFFFFFFFF in the
                   INTERIM response.

                */
                /*float f_*/ uint16_t time_S = time / 1000;
               LOG_INF( "Time: %d min %d sec", (time_S / 60), (time_S % 60));
              }
            }
            }
          }
          break;
          case AVRCP_EVENT_BATT_STATUS_CHANGED:
          {
            /*
              NOTE: Battery status notification defined in this specification is expected to be
              deprecated in favor of Attribute profile specification in the future.
            */
            DBG_AVRCP((char *)"Battery status: ");
            switch (event[13])
            {
              /*
                0x0 – NORMAL – Battery operation is in normal state
                0x1 – WARNING - unable to operate soon. Is provided when the battery level is going down.
                0x2 – CRITICAL – Cannot operate any more. Is provided when the battery level is going down.
                0x3 – EXTERNAL – Plugged to external power supply
                0x4 - FULL_CHARGE – when the device is completely charged from the external power supply
              */
            case 0x00:
              DBG_AVRCP((char *)"NORMAL");
              break;
            case 0x01:
              DBG_AVRCP((char *)"WARNING");
              break;
            case 0x02:
              DBG_AVRCP((char *)"CRITICAL");
              break;
            case 0x03:
              DBG_AVRCP((char *)"EXTERNAL");
              break;
            case 0x04:
              DBG_AVRCP((char *)"FULL_CHARGE");
              break;
            }
          }
          break;
          case AVRCP_EVENT_SYSTEM_STATUS_CHANGED:
          {
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:

              break;
            }
          }
          break;
          case AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:
          {
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
              uint8_t i = 14;
              while (i < 14 + 2 * event[13])
              {
                uint8_t event1 = event[i++];
                uint8_t event2 = event[i++];
                IS2020::decodeAvrcpPlayerAtributes(event1, event2);
              }
              IS2020::avrcpRegNotifyPlayerAppSettingsChanged(deviceID);
              break;
            }
          }
          break;
          case AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED:
            /*6.9.5
              Notify Now Playing Content Changed
              The Now Playing Content Changed notification allows a CT to be informed when the content of the
              NowPlaying folder for the Addressed Player is changed. The notification should not be completed if only
              the track has changed or the order of the tracks on the now playing list has changed.
              This is an event which may be used for the Register Notification command described in Section 6.7.2,
              which is a vendor dependent AV/C Notify. The interim and final responses to the notify shall contain no
              parameters.
              An example PDU for this command is given in Section 25.14.
              Note that to retrieve the content of the NowPlaying folder, the NowPlaying folder can be browsed (see
              Section 6.10.4.2). If the NowPlaying folder is browsed as reaction to the
              EVENT_NOW_PLAYING_CONTENT_CHANGED, the CT should register the
              EVENT_NOW_PLAYING_CONTENT_CHANGED again before browsing the NowPlaying folder in order
              to get informed about intermediate changes in that folder.
            */
            {
              switch (event[2])
              { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
              case AVRCP_EVENT_RESPONSE_NOTIFY:
                break;
              case AVRCP_EVENT_RESPONSE_INTERIM:
                break;
              case AVRCP_EVENT_RESPONSE_CHANGED:

                for (uint16_t i = 0; i < packetSize; i++)
                {
                  LOG_INF( "%x,", event[i]);
                }

                IS2020::avrcpRegNotifyNowPlayingContentChanged(deviceID);
                break;
              }
            }
            break;
          case AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED:
          {
            uint16_t playerID = event[13] << 8 | event[14];
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM: // currently playing playerid:
              IS2020::avrcpListPlayerAttributes(deviceID);
              IS2020::avrcpStorePlayerID(playerID);
              // IS2020::avrcpSetBrowsedPlayer(deviceID, playerID);
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            { // re-active event
              // DBG_AVRCP((char*)" ["); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP((char*)String(event[i], HEX) + ",");  DBG_AVRCP((char*)"]");
              IS2020::avrcpRegNotifyAddressedPlayerChanged(deviceID);
              IS2020::avrcpListPlayerAttributes(deviceID);
              IS2020::avrcpStorePlayerID(playerID);
              // IS2020::avrcpSetBrowsedPlayer(deviceID, playerID);
              IS2020::avrcpListPlayerAttributes(deviceID);
            }
            break;
            }
            LOG_INF( "Current PlayerId: %x", avrcpPlayerID[avrcpCurrentPlayerID]);
            // DBG_AVRCP((char*)" UID");
            // DBG_AVRCP((char*)String(event[15], HEX) + String(event[16], HEX));
          }
          break;
          case AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED:
          {
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            { // re-active event

              for (uint16_t i = 0; i < packetSize; i++)
              {
                LOG_INF( "%x,", event[i]);
              }

              IS2020::avrcpRegNotifyAvailablePlayersChanged(deviceID);
            }
            break;
            }
          }
          break;
          case AVRCP_EVENT_UIDS_CHANGED:
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            { // re-active event
              LOG_INF( "AVRCP_EVENT_RESPONSE_CHANGED:");
              for (uint16_t i = 0; i < packetSize; i++)
              {
                LOG_INF( "%x,", event[i]);
              }
              IS2020::avrcpRegNotifyUIDsChanged(deviceID);
            }
            break;
            }
            break;
          case AVRCP_EVENT_VOLUME_CHANGED:
            LOG_INF( "AVRCP_EVENT_VOLUME_CHANGED:");
            for (uint16_t i = 0; i < packetSize; i++)
            {
              LOG_INF( "%x,", event[i]);
            }
            switch (event[2])
            { // thx to vincent https://github.com/VincentGijsen/MelbusRtos
            case AVRCP_EVENT_RESPONSE_NOTIFY:
              break;
            case AVRCP_EVENT_RESPONSE_INTERIM:
              break;
            case AVRCP_EVENT_RESPONSE_CHANGED:
            { // re-active event
              // IS2020::avrcpRegNotifyVolumeChanged(deviceID);
            }
            break;
            }
            break;
          }
        }
        break;
        case AVRCP_GET_CAPABILITIES: // event[12] determinate what capabilities we are receiving, 0x02 = company ID, 0x03=player caps.
        {
          if (event[12] == CAP_COMPANY_ID)
          { // company ID:
            /*                       DBG_AVRCP((char*)"Company IDs: ");
                                  DBG_AVRCP((char*)(String)event[13]);
                                   DBG_AVRCP((char*)" ");
                                  for (uint8_t numCompID = 0; numCompID < event[13]; numCompID++) { //event[13] = CapabilityCount
                                    for (uint8_t i = 0; i < 3; i++) {
                                      DBG_AVRCP((char*)String(event[14 + (numCompID * 3) + i], HEX));
                                    }
                                  }*/
          }
          else if (event[12] == CAP_EVENTS_SUPPORTED)
          {
            for (uint16_t parameter_byte = 14; parameter_byte < (14 + event[13]); parameter_byte++)
            { // event[13] = CapabilityCount
              if (event[parameter_byte] == AVRCP_EVENT_PLAYBACK_POS_CHANGED)
              {
                IS2020::avrcpRegistrationForNotificationOfEvent(event[1], event[parameter_byte], 0x00, 0x00, 0x13, 0x88);
              }
              else
              {
                IS2020::avrcpRegistrationForNotificationOfEvent(event[1], event[parameter_byte], 0x00, 0x00, 0x00, 0x00);
              }
            }
          }
          else
          {
            tmp = (char *)"Unknown capabilities ID! ";
            LOG_INF( "Unknown capabilities ID! %x", event[12]);
          }
        }
        break;
        case AVRCP_LIST_PLAYER_ATTRIBUTES:
        {
          for (uint8_t i = 0; i < event[12]; i++)
          {
            /*switch (event[12] + i) {
              case AVRCP_ATTRIBUTE_EQUALIZER:
               //  DBG_AVRCP((char*)"Equalizer");
                break;
              case AVRCP_ATTRIBUTE_REPEAT_MODE:
               //  DBG_AVRCP((char*)"Repeat");
                break;
              case AVRCP_ATTRIBUTE_SHUFFLE:
               //  DBG_AVRCP((char*)"Shuffle");
                break;
              case AVRCP_ATTRIBUTE_SCAN:
               //  DBG_AVRCP((char*)"Scan");
                break;
              default: //unknown feature id?
                IS2020::avrcpGetPlayerAttributeText(deviceID, event[12] + i);
                break;
              }*/
            // if ((event[12] + i) > 0x04) // non standard attribute
            //   // IS2020::avrcpGetPlayerAttributeText(deviceID, event[12] + i);
            // else
            // IS2020::avrcpGetCurrentPlayerValue(deviceID, event[12] + i);
          }
        }
        break;
        case AVRCP_LIST_PLAYER_VALUES:
        {
          DBG_AVRCP((char *)"Supported settings of this player:");
          for (uint16_t parameter_byte = 13; parameter_byte < packetSize; parameter_byte++)
          {
            IS2020::decodeAvrcpPlayerAtributes(event[12], event[parameter_byte]);
            // for unknown attributes:
            if (event[12] > 0x04)
              IS2020::avrcpGetPlayerValueText(deviceID, event[12], event[parameter_byte]);
          }
        }
        break;
        case AVRCP_GET_CURRENT_PLAYER_VALUE:
          for (uint16_t parameter_byte = 13; parameter_byte < packetSize; parameter_byte++)
          {
            IS2020::decodeAvrcpPlayerAtributes(event[12], event[parameter_byte]);
            // for unknown attributes:
            if (event[12] > 0x04)
              IS2020::avrcpGetPlayerValueText(deviceID, event[12], event[parameter_byte]);
          }
          break;
        case AVRCP_SET_PLAYER_VALUE:
        {
          DBG_AVRCP((char *)"arvrcp_set_player_value");
          for (uint16_t i = 0; i < packetSize; i++)
          {
            LOG_INF( "%x,", event[i]);
          }
        }
        break;
        case AVRCP_GET_PLAYER_ATTRIBUTE_TEXT:
        {
          // DBG_AVRCP((char*)"Text of player attribute:");
          // DBG_AVRCP((char*)" ["); for (uint16_t i = 0; i<packetSize; i++)   DBG_AVRCP((char*)String(event[i],HEX)+",");  DBG_AVRCP((char*)"]");
          // event[12] - pocet atributov
          // event[13] - attrID
          // event[14][15] = character code
          // event[16] - string length
          // event[17]- [x] string
          uint8_t attribOffset = 13;
          for (int x = 0; x < event[12] /*numberOfAttribs*/; x++)
          {
            // uint8_t attributeID = event[attribOffset];
            // uint16_t codingOfString = event[attribOffset + 1] << 8 | event[attribOffset + 2]; //0x6a == UTF8
            uint8_t attribValLength = event[attribOffset + 3];
            for (uint16_t i = 0; i < attribValLength; i++)
            {
              // Serial.write(event[attribOffset + 4 + i]);
              // here we should store this text, cose this is non standard player feature
            }
            // Serial.println();
            attribOffset += 4 + attribValLength;
          }
        }
        break;
        case AVRCP_GET_PLAYER_VALUE_TEXT:
        {
          // DBG_AVRCP((char*)"Text of player attribute value:");
          // DBG_AVRCP((char*)" ["); for (uint16_t i = 0; i < packetSize; i++)   DBG_AVRCP((char*)String(event[i], HEX) + ",");  DBG_AVRCP((char*)"]");
          // event[12] - pocet atributov
          // event[13] - attrID
          // event[14][15] = character code
          // event[16] - string length
          // event[17]- [x] string
          uint8_t attribOffset = 13;
          for (int x = 0; x < event[12] /*numberOfAttribs*/; x++)
          {
            // uint8_t valueID = event[attribOffset];
            // uint16_t codingOfString = event[attribOffset + 1] << 8 | event[attribOffset + 2]; //0x6a == UTF8
            uint8_t atribValLength = event[attribOffset + 3];
            for (uint16_t i = 0; i < atribValLength; i++)
            {
              // Serial.write(event[attribOffset + 4 + i]);
              // here we should store this text, cose this is non standard player feature
            }
            // Serial.println();
            attribOffset += 4 + atribValLength;
          }
        }
        break;
        case AVRCP_GET_ELEMENT_ATTRIBUTES:
        {
          // uint8_t numberOfAttribs = event[12];
          LOG_INF( "AVRCP_GET_ELEMENT_ATTRIBUTES");
          uint8_t attribOffset = 16;
          for (int x = 0; x < event[12] /*numberOfAttribs*/; x++)
          {

            uint8_t mediaAttributeID = event[attribOffset + 0]; // events 13-16, currently in avrcp1.5 only 7 ID's are defined
            // uint16_t codingOfString = event[attribOffset + 1] << 8 | event[attribOffset + 2]; //will default to only 18... 0x6a == UTF8
            uint16_t attribValLength = event[attribOffset + 3] << 8 | event[attribOffset + 4];
            switch (mediaAttributeID)
            {
            case AVRCP_MEDIA_ATTRIBUTE_ILLEGAL:
              LOG_INF( "Ilegal");
              break;
            case AVRCP_MEDIA_ATTRIBUTE_TITLE:
              LOG_INF( "Title: ");
              break;
            case AVRCP_MEDIA_ATTRIBUTE_ARTIST:
              LOG_INF( "Artist: ");
              break;
            case AVRCP_MEDIA_ATTRIBUTE_ALBUM:
              LOG_INF( "Albun: ");
              break;
            case AVRCP_MEDIA_ATTRIBUTE_TRACK:
              LOG_INF( "Track: ");
              break;
            case AVRCP_MEDIA_ATTRIBUTE_N_TRACKS:
              LOG_INF( "Number of tracks: ");
              break;
            case AVRCP_MEDIA_ATTRIBUTE_GENRE:
              LOG_INF( "Genre: ");
              break;
            case AVRCP_MEDIA_ATTRIBUTE_DURATION:
              LOG_INF( "Duration: ");
              break;
            case AVRCP_MEDIA_DEFAULT_COVER_ART: // BIP Image Handle
              LOG_INF( "Cover: ");
              break;
            default:
              LOG_INF( "Not-impl:");
            }
            if (mediaAttributeID == AVRCP_MEDIA_ATTRIBUTE_DURATION)
            {
              uint16_t time_MS = 0; // uint32_t for miliseconds....

              for (uint16_t i = 0; i < attribValLength - 3; i++)
              { // duration in ms, ascii ,if we need only seconds, then we need only 1st 3 numbers
                time_MS *= 10;
                /*if (event[attribOffset + 5 + i] > 0 ) */ time_MS += (event[attribOffset + 5 + i] - 0x30);
              }

              // uint16_t time_S = time_MS / 1000;
              // uint8_t time_M = time_S/60; //minutes
              // time_S = time_S%60;//seconds
              LOG_INF( "Time: %d min %d sec", (time_MS / 60), (time_MS % 60));
            }
            else
              for (uint16_t i = 0; i < attribValLength; i++)
              {
                // Serial.write(event[attribOffset + 5 + i]);
              }
            // update offset to point to next attribute (if any)
            attribOffset += 8 + attribValLength;
          }
        }
        break;
        case AVRCP_DISPLAYABLE_CHARSET:
          DBG_AVRCP((char *)"AVRCP_DISPLAYABLE_CHARSET");
          for (uint16_t i = 0; i < packetSize; i++)
          {
            LOG_INF( "%x,", event[i]);
          }
          break;
        case AVRCP_CT_BATTERY_STATUS:
          DBG_AVRCP((char *)"Battery level: ");
          switch (event[12])
          {
          case AVRCP_BATTERY_STATUS_NORMAL:
            DBG_AVRCP((char *)"Normal.");
            break;
          case AVRCP_BATTERY_STATUS_WARNING:
            DBG_AVRCP((char *)"Warning, almost off!");
            break;
          case AVRCP_BATTERY_STATUS_CRITICAL:
            DBG_AVRCP((char *)"Critical, going down!");
            break;
          case AVRCP_BATTERY_STATUS_EXTERNAL:
            DBG_AVRCP((char *)"Charging.");
            break;
          case AVRCP_BATTERY_STATUS_FULL_CHARGE:
            DBG_AVRCP((char *)"Fully charged battery.");
            break;
          }
          break;
        case AVRCP_GET_PLAY_STATUS:
        {
          DBG_AVRCP((char *)"PLAY STATUS");
          for (uint16_t i = 0; i < packetSize; i++)
          {
            LOG_INF( "%x,", event[i]);
          }

          DBG_AVRCP((char *)"Song length: ");
          uint32_t time_MS = 0;

          time_MS = ((uint32_t)event[12] << 24 | (uint32_t)event[13] << 16 | (uint32_t)event[14] << 8 | event[15]) / 1000;

          LOG_INF( "Time: %u min %u sec", (time_MS / 60), (time_MS % 60));
          DBG_AVRCP((char *)"Song position: ");
          time_MS = 0;

          time_MS = ((uint32_t)event[16] << 24 | (uint32_t)event[17] << 16 | (uint32_t)event[18] << 8 | event[19]) / 1000;
          LOG_INF( "Time: %u min %u sec", (time_MS / 60), (time_MS % 60));
          DBG_AVRCP((char *)"Play status: ");
          switch (event[20])
          {
          case 0x00:
            DBG_AVRCP((char *)"STOPPED");
            break;
          case 0x01:
            DBG_AVRCP((char *)"PLAYING");
            break;
          case 0x02:
            DBG_AVRCP((char *)"PAUSED");
            break;
          case 0x03:
            DBG_AVRCP((char *)"FWD_SEEK");
            break;
          case 0x04:
            DBG_AVRCP((char *)"REV_SEEK");
            break;
          case 0xFF:
            DBG_AVRCP((char *)"ERROR");
            break;
          }
        }
        break;
        case AVRCP_SET_BROWSED_PLAYER:
          LOG_INF( "events: %d , %d , %d , %d", event[16], event[17], event[18], event[19]);
          break;
        case AVRCP_REQUEST_CONTINUING:
          DBG_AVRCP((char *)"AVRCP_REQUEST_CONTINUING");
          break;
        }

        // Moved from start to end, so we ask for next data after processing already read data
        // Serial3.print("packet type: ");
        switch (event[9] & 0x07)
        {
        //                case AVRCP_PACKET_TYPE_SINGLE:
        //                  Serial3.println("single packet");
        //                  break;
        //                case AVRCP_PACKET_TYPE_START:
        //                  Serial3.println("start packet");
        //                  break;
        case AVRCP_PACKET_TYPE_CONTINUING:
          DBG_AVRCP((char *)"continue packet");
          IS2020::avrcpRequestContinuing(event[0], event[8]); // send after all data are processed
          break;
          //                case AVRCP_PACKET_TYPE_END:
          //                  Serial3.println("end packet");
          //                  break;
          //                default:
          //                  Serial3.println(event[3], HEX);
          //                  break;
        }
      }
      break;
      /*
        Event Format:	Command	Event Code	Event Parameters
        BTM_Utility_Req	0x1B	action_type, parameter

        Description:	This event is used to ask specific utility request for MCU.

        Event Parameters:	action_type	SIZE: 1 BYTE
        Value	Parameter Description
        0x00	BTM ask MCU to control the external amplifier
        0x01	BTM report the Aux line-in status to Host MCU.
        others	reserved

        parameter	SIZE: 1 BYTE
        action_type=0x00
        Value	Parameter Description
        0x00	Mute or switch off amplifier
        0x01	Unmute or switch on amplifier
        others	reserved

        action_type=0x01
        Value	Parameter Description
        0x00	Aux line in is unplugged.
        0x01	Aux line in is plugged.
        0x02	Aux line in is plugged and with audio signal.
        0x03	Aux line in is plugged and silence.
        others	reserved
      */
      case EVT_BTM_Utility_Req:
      {
      }
      break;
      /*
        Event Format:	Event	Event Code	Event Parameters
        Vendor_AT_Cmd_Rsp	0x01C	data_base_index, status

        Description:	This event is used to reply the Vendor_AT_Cmd command(0x0A).

        Event Parameters:	data_base_index	SIZE: 1 BYTE
        Value	Parameter Description
        0x00	database 0 for a dedicate link
        0x01	database 1 for a dedicate link


        status	SIZE: 1 BYTE
        Value	Parameter Description
        0	AG response OK
        1	AG response ERROR
        2	No response from AG
        others	RFD

      */
      case EVT_Vendor_AT_Cmd_Reply:
      {
        switch (event[2])
        {
        case 0x00:
          DBG((char *)"AG response OK"); /*allowedSendATcommands=1;*/
          break;
        case 0x01:
          DBG((char *)"AG response ERROR");
          allowedSendATcommands = 1;
          break;
        case 0x02:
          DBG((char *)"NO response from AG");
          allowedSendATcommands = 1;
          break;
        }
      }
      break;
      /*
        Value	Parameter Description
        0x00	database 0 for a dedicate link
        0x01	database 1 for a dedicate link


        result_payload	SIZE: N BYTES
        Value	Parameter Description
        0xXX…	"result code.
        For example : AG send result code ""+test:1"" , the result code will be ""+test:1"" "

      */
      case EVT_Report_Vendor_AT_Event:
      {
        // uint8_t deviceId = event[1];
        //
        if (event[2] == '+')
        {

          for (uint16_t i = 2; i < packetSize; i++)
          {
            LOG_INF( "event; %d", event[i]);
          }

          switch (event[3])
          {
          case 'C': // command?
          {
            if (event[4] == 'P' && event[5] == 'B')
            { // phonebook commands
              switch (event[6])
              {
              case 'S':
              {
                //[+CPBS: ("ME","SM","DC","RC","MC")] available phonebooks:
                for (uint8_t i = 10; i < packetSize; i++)
                {
                  if (event[i] == '"' && event[i + 3] == '"')
                  {
                    /*
                      EN = 1,
                      FD = 2,
                      DC = 4,
                      LD = 8,
                      MC = 16,
                      ME = 32,
                      MT = 64,
                      ON = 128,
                      RC = 256,
                      SM = 512,
                      SN = 1024
                    */
                    if (event[i + 1] == 'E' && event[i + 2] == 'N')
                      supportedPBs = EN;
                    if (event[i + 1] == 'F' && event[i + 2] == 'D')
                      supportedPBs = FD;
                    if (event[i + 1] == 'D' && event[i + 2] == 'C')
                      supportedPBs = DC;
                    if (event[i + 1] == 'L' && event[i + 2] == 'D')
                      supportedPBs = LD;
                    if (event[i + 1] == 'M' && event[i + 2] == 'C')
                      supportedPBs = MC;
                    if (event[i + 1] == 'M' && event[i + 2] == 'E')
                      supportedPBs = ME;
                    if (event[i + 1] == 'M' && event[i + 2] == 'T')
                      supportedPBs = MT;
                    if (event[i + 1] == 'O' && event[i + 2] == 'N')
                      supportedPBs = ON;
                    if (event[i + 1] == 'R' && event[i + 2] == 'C')
                      supportedPBs = RC;
                    if (event[i + 1] == 'S' && event[i + 2] == 'M')
                      supportedPBs = SM;
                    if (event[i + 1] == 'S' && event[i + 2] == 'N')
                      supportedPBs = SN;
                    // Serial.println(supportedPBs,BIN);
                    i += 4;
                  }
                }
              }
              }
              break;
            }
          }
          break;
          }
        }
        allowedSendATcommands = 1;
      }
      break;
      /*
        Event Format:  Event    Event Code     Event Parameters
        Read_Link_Status_Reply  0x1E           device_state,
        database0_connect_status,
        database1_connect_status,
        database0_play_status,
        database1_play_status,
        database0_stream_status,
        database1_stream_status,

        Description:  This event is used to reply the Read_Link_Status command.

        Event Parameters:
        device_state  SIZE: 1 BYTE
        Value Parameter Description
        0x00  Power OFF state
        0x01  pairing state (discoverable mode)
        0x02  standby state
        0x03  Connected state with only HF profile connected
        0x04  Connected state with only A2DP profile connected
        0x05  Connected state with only SPP profile connected
        0x06  Connected state with multi-profile connected

        database0_connect_status  SIZE: 1 BYTE
        Value Parameter Description
        0xXX  "1 indicate connected
        Bit0 : A2DP profile signaling channel connected
        Bit1 : A2DP profile stream channel connected
        Bit2 : AVRCP profile connected
        Bit3 : HF profile connected
        Bit4 : SPP connected"

        database1_connect_status  SIZE: 1 BYTE
        Value Parameter Description
        0xXX  "1 indicate connected
        Bit0 : A2DP profile signaling channel connected
        Bit1 : A2DP profile stream channel connected
        Bit2 : AVRCP profile connected
        Bit3 : HF profile connected
        Bit4 : SPP connected"

        "database0_play_status,
        database1_play_status, " SIZE: 1 BYTE
        Value Parameter Description
        0xXX  "0x00: STOP
        0x01: PLAYING
        0x02: PAUSED
        0x03: FWD_SEEK
        0x04: REV_SEEK
        0x05: FAST_FWD
        0x06: REWIND
        0x07: WAIT_TO_PLAY
        0x08: WAIT_TO_PAUSE"

        "database0_stream_status,
        database1_stream_status, " SIZE: 1 BYTE
        Value Parameter Description
        0xXX  "1 indicate connected
        0x00: no stream
        0x01: stream on - going"

      */
      case EVT_Read_Link_Status_Reply:
      {
        switch (event[1])
        {
        case 0x00:
          LOG_INF( "Power OFF state");
          break;
        case 0x01:
          LOG_INF( "pairing state (discoverable mode)");
          break;
        case 0x02:
          LOG_INF( "standby state");
          break;
        case 0x03:
          LOG_INF( "Connected state with only HF profile connected");
          break;
        case 0x04:
          LOG_INF( "Connected state with only A2DP profile connected");
          break;
        case 0x05:
          LOG_INF( "Connected state with only SPP profile connected");
          break;
        case 0x06:
          LOG_INF( "Connected state with multi-profile connected");
          break;
        }

        for (uint8_t i = 0; i < 7; i++)
        {
          if (linkStatus[i] != event[i + 1])
          {
            linkStatus[i] = event[i + 1]; // link status is array of 7 bytes, like response is 7bytes.
            btmStatusChanged = 1;
          }
        }
        if (linkStatus[1] > 0)
        {
          IS2020::queryDeviceName(0x00);
          // IS2020::queryInBandRingtone_status(0x00);
          // IS2020::queryIfRemoteDeviceIsIAPDevice(0x00);
          // IS2020::queryIfRemoteDeviceSupportAavrcpV13(0x00);
          // IS2020::queryHfA2DPGain(0x00);
          // IS2020::queryLineInGain(0x00);
          // IS2020::avrcpDisplayableCharset(0x00);
          // IS2020::avrcpGetCapabilities(0x00, 0x02); //Get Capability command for Company ID
          IS2020::avrcpGetCapabilities(0x00, 0x03); // Get Capability command for Events
          IS2020::avrcpListPlayerAttributes(0x00);
          // IS2020::registerAllEvents(0x00);
        }
        else
        {
          removeInfoAboutDevice(0);
        }
        if (linkStatus[2] > 0)
        {
          IS2020::queryDeviceName(1);
          // IS2020::queryInBandRingtoneStatus(0x01);
          // IS2020::queryIfRemoteDeviceIsIAPDevice(0x01);
          // IS2020::queryIfRemoteDeviceSupportAvrcpV13(0x01);
          // IS2020::queryHfA2DPGain(0x01);
          // IS2020::queryLineInGain(0x01);
          // IS2020::avrcpDisplayableCharset(0x01);
          // IS2020::avrcpGetCapabilities(0x01, 0x02);
          IS2020::avrcpGetCapabilities(0x01, 0x03);
          IS2020::avrcpListPlayerAttributes(0x01);
          // IS2020::registerAllEvents(0x01);
        }
        else
        {
          IS2020::removeInfoAboutDevice(1);
        }
      }
      break;

      case EVT_Read_Paired_Device_Record_Reply:
      {
        for (uint8_t i = 0; i < 8; i++)
        {
          for (uint8_t y = 0; y < 6; y++)
            btAddress[i][y] = 0;
        }
        for (uint8_t dev = event[1]; dev > 0; dev--)
        {
          uint8_t pos = dev - 1;
          uint8_t offset = pos * 7;
          btAddress[pos][0] = event[offset + 8];
          btAddress[pos][1] = event[offset + 7];
          btAddress[pos][2] = event[offset + 6];
          btAddress[pos][3] = event[offset + 5];
          btAddress[pos][4] = event[offset + 4];
          btAddress[pos][5] = event[offset + 3];
          // LOG_INF( "MAC: %s", btAddress[pos][0]);
        }
      }
      break;
      case EVT_Read_Local_BD_Address_Reply:
      {
        DBG_EVENTS((char *)"Local BT adr: ");
        for (uint8_t _byte = 0; _byte < 6; _byte++)
        {
          IS2020::moduleBtAddress[_byte] = event[6 - _byte];
        }
        LOG_INF( "%x:%x:%x:%x:%x:%x", IS2020::moduleBtAddress[0], IS2020::moduleBtAddress[1], IS2020::moduleBtAddress[2],
                 IS2020::moduleBtAddress[3], IS2020::moduleBtAddress[4], IS2020::moduleBtAddress[5]);
      }
      break;
      case EVT_Read_Local_Device_Name_Reply:
      {
        IS2020::localDeviceName = (char *)"";
        for (uint8_t _byte = 0; _byte < event[1]; _byte++)
        {
          IS2020::localDeviceName += (char)event[_byte + 2];
        }
        DBG_EVENTS((char *)"BT name: ");
        LOG_INF( "%s", IS2020::localDeviceName);
      }
      break;
      case EVT_Report_SPP_iAP_Data:
      {
        const uint8_t message_size = event[6];
        uint8_t message[message_size];
        uint8_t message_response[message_size + 5];
        uint8_t count = 7;
        uint8_t count_msg_response = 5;
        for (uint8_t i = 0; i < message_size; i++)
        {
          message[i] = event[count];
          message_response[count_msg_response] = event[count];
          count_msg_response++;
          count++;
        }

        message_response[0] = 0x00;
        message_response[1] = 0x00;
        message_response[2] = 0x05;
        message_response[3] = 0x00;
        message_response[4] = message_size;

        IS2020::sendPacketArrayInt(message_size + 7, CMD_Send_SPP_iAP_Data, 0, message_response);
        // LOG_ERR( "message_size %d message:", message_size);
        // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) message, message_size, ESP_LOG_INFO);
      }
      break;
      case EVT_Report_Link_Back_Status:
      {
      }
      break;
      case EVT_Ringtone_Finish_Indicate:
      {
      }
      break;
      case EVT_User_Confrim_SSP_Req:
      {
      }
      break;
      case EVT_Report_AVRCP_Vol_Ctrl:
      {
      }
      break;
      case EVT_Report_Input_Signal_Level:
      {
      }
      break;
      case EVT_Report_iAP_Info:
      {
      }
      break;
      case EVT_REPORT_AVRCP_ABS_VOL_CTRL:
      {
      }
      break;
      case EVT_Report_Voice_Prompt_Status:
      {
      }
      break;
      case EVT_Report_MAP_Data:
      {
        LOG_ERR( "EVT_Report_MAP_Data");
      }
      break;
      case EVT_Security_Bonding_Res:
      {
      }
      break;
      case EVT_Report_Type_Codec:
      {
      }
      break;
      case Report_BTM_Initial_Status:
      {
        LOG_INF( "Report_BTM_Initial_Status");
        // void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size) event, packetSize, ESP_LOG_INFO);
        if (event[1] == 0)
        {
          LOG_INF( "initialization completed");
        }
      }
      break;
      default:
      {
        /*Serial3.println();
          Serial3.print("Unknown  BYTE: ");
          for (uint8_t i = 0; i < packetSize; i++) {
          Serial3.print(event[i], HEX);
          }
          Serial3.println();*/
      }
      }
      return event[0];
    }
    return 1;
  }
  // }
  return 0;
}

void IS2020::decodeEvent(uint8_t Event)
{
  switch (Event)
  {
  case 0x00:
    DBG_EVENTS((char *)"Command_ACK");
    break;
  case 0x01:
    DBG_EVENTS((char *)"BTM_Status");
    break;
  case 0x02:
    DBG_EVENTS((char *)"Call_Status");
    break;
  case 0x03:
    DBG_EVENTS((char *)"Caller_ID");
    break;
  case 0x04:
    DBG_EVENTS((char *)"SMS_Received_Indication");
    break;
  case 0x05:
    DBG_EVENTS((char *)"Missed_Call_Indication");
    break;
  case 0x06:
    DBG_EVENTS((char *)"Phone_Max_Battery_Level");
    break;
  case 0x07:
    DBG_EVENTS((char *)"Phone_Current_Battery_Level");
    break;
  case 0x08:
    DBG_EVENTS((char *)"Roaming_Status");
    break;
  case 0x09:
    DBG_EVENTS((char *)"Phone_Max_Signal_Strength_Level");
    break;
  case 0x0A:
    DBG_EVENTS((char *)"Phone_Current_Signal_Strength_Level");
    break;
  case 0x0B:
    DBG_EVENTS((char *)"Phone_Service_Status");
    break;
  case 0x0C:
    DBG_EVENTS((char *)"BTM_Battery_Status");
    break;
  case 0x0D:
    DBG_EVENTS((char *)"BTM_Charging_Status");
    break;
  case 0x0E:
    DBG_EVENTS((char *)"Reset_To_Default");
    break;
  case 0x0F:
    DBG_EVENTS((char *)"Report_HF_Gain_Level");
    break;
  case 0x10:
    DBG_EVENTS((char *)"EQ_Mode_Indication");
    break;
  case 0x11:
    DBG_EVENTS((char *)"PBAP_Missed_Call_History");
    break;
  case 0x12:
    DBG_EVENTS((char *)"PBAP_Received_Call_History");
    break;
  case 0x13:
    DBG_EVENTS((char *)"PBAP_Dialed_Call_History");
    break;
  case 0x14:
    DBG_EVENTS((char *)"PBAP_Combine_Call_History");
    break;
  case 0x15:
    DBG_EVENTS((char *)"Get_PB_By_AT_Cmd_Reply");
    break;
  case 0x16:
    DBG_EVENTS((char *)"PBAP_Access_Finish");
    break;
  case 0x17:
    DBG_EVENTS((char *)"Read_Linked_Device_Information_Reply");
    break;
  case 0x18:
    DBG_EVENTS((char *)"Read_BTM_Version_Reply");
    break;
  case 0x19:
    DBG_EVENTS((char *)"Call_List_Report");
    break;
  case 0x1A:
    DBG_EVENTS((char *)"AVRCP_Specific_Rsp");
    break;
  case 0x1B:
    DBG_EVENTS((char *)"BTM_Utility_Req");
    break;
  case 0x1C:
    DBG_EVENTS((char *)"Vendor_AT_Cmd_Reply");
    break;
  case 0x1D:
    DBG_EVENTS((char *)"Report_Vendor_AT_Event");
    break;
  case 0x1E:
    DBG_EVENTS((char *)"Read_Link_Status_Reply");
    break;
  case 0x1F:
    DBG_EVENTS((char *)"Read_Paired_Device_Record_Reply");
    break;
  case 0x20:
    DBG_EVENTS((char *)"Read_Local_BD_Address_Reply");
    break;
  case 0x21:
    DBG_EVENTS((char *)"Read_Local_Device_Name_Reply");
    break;
  case 0x22:
    DBG_EVENTS((char *)"Report_SPP_iAP_Data");
    break;
  case 0x23:
    DBG_EVENTS((char *)"Report_Link_Back_Status");
    break;
  case 0x24:
    DBG_EVENTS((char *)"Ringtone_Finish_Indicate");
    break;
  case 0x25:
    DBG_EVENTS((char *)"User_Confrim_SSP_Req");
    break;
  case 0x26:
    DBG_EVENTS((char *)"Report_AVRCP_Vol_Ctrl");
    break;
  case 0x27:
    DBG_EVENTS((char *)"Report_Input_Signal_Level");
    break;
  case 0x28:
    DBG_EVENTS((char *)"Report_iAP_Info");
    break;
  case 0x29:
    DBG_EVENTS((char *)"REPORT_AVRCP_ABS_VOL_CTRL");
    break;
  case 0x2A:
    DBG_EVENTS((char *)"Report_Voice_Prompt_Status");
    break;
  case 0x2B:
    DBG_EVENTS((char *)"Report_MAP_Data");
    break;
  case 0x2C:
    DBG_EVENTS((char *)"Security_Bonding_Res");
    break;
  case 0x2D:
    DBG_EVENTS((char *)"Report_Type_Codec");
    break;
  case 0x30:
    DBG_EVENTS((char *)"Report_BTM_Initial_Status");
    break;
  }
}

void IS2020::decodeCommandInEvents(uint8_t cmd)
{
  switch (cmd)
  {
  case CMD_Make_Call:
    DBG_EVENTS((char *)"Make Call");
    break;
  case CMD_Make_Extension_Call:
    DBG_EVENTS((char *)"Make Extension Call");
    break;
  case CMD_MMI_Action:
    DBG_EVENTS((char *)"MMI Action");
    break;
  case CMD_Event_Mask_Setting:
    DBG_EVENTS((char *)"Event Mask Setting");
    break;
  case CMD_Music_Control:
    DBG_EVENTS((char *)"Music Control");
    break;
  case CMD_Change_Device_Name:
    DBG_EVENTS((char *)"Change Device Name");
    break;
  case CMD_Change_PIN_Code:
    DBG_EVENTS((char *)"Change PIN Code");
    break;
  case CMD_BTM_Parameter_Setting:
    DBG_EVENTS((char *)"BTM_Parameter_Setting");
    break;
  case CMD_Read_BTM_Version:
    DBG_EVENTS((char *)"Read_BTM_Version");
    break;
  case CMD_Get_PB_By_AT_Cmd:
    DBG_EVENTS((char *)"Get PB By AT Cmd");
    break;
  case CMD_Vendor_AT_Command:
    DBG_EVENTS((char *)"Vendor AT Command");
    break;
  case CMD_AVRCP_Specific_Cmd:
    DBG_EVENTS((char *)"AVRCP Specific Cmd");
    break;
  case CMD_AVRCP_Group_Navigation:
    DBG_EVENTS((char *)"AVRCP Group Navigation");
    break;
  case CMD_Read_Link_Status:
    DBG_EVENTS((char *)"Read Link Status");
    break;
  case CMD_Read_Paired_Device_Record:
    DBG_EVENTS((char *)"Read Paired Device Record");
    break;
  case CMD_Read_Local_BT_Address:
    DBG_EVENTS((char *)"Read Local BT Address");
    break;
  case CMD_Read_Local_Device_Name:
    DBG_EVENTS((char *)"Read Local Device Name");
    break;
  case CMD_Set_Access_PB_Method:
    DBG_EVENTS((char *)"Set Access PB Method");
    break;
  case CMD_Send_SPP_iAP_Data:
    DBG_EVENTS((char *)"Send SPP iAP Data");
    break;
  case CMD_BTM_Utility_Function:
    DBG_EVENTS((char *)"BTM Utility Function");
    break;
  case CMD_Event_ACK:
    DBG_EVENTS((char *)"Event ACK");
    break;
  case CMD_Additional_Profiles_Link_Setup:
    DBG_EVENTS((char *)"Additional Profiles Link Setup");
    break;
  case CMD_Read_Linked_Device_Information:
    DBG_EVENTS((char *)"Read Linked Device Information");
    break;
  case CMD_Profile_Link_Back:
    DBG_EVENTS((char *)"Profile Link Back");
    break;
  case CMD_Disconnect:
    DBG_EVENTS((char *)"Disconnect");
    break;
  case CMD_MCU_Status_Indication:
    DBG_EVENTS((char *)"MCU Status Indication");
    break;
  case CMD_User_Confirm_SPP_Req_Reply:
    DBG_EVENTS((char *)"User Confirm SPP Req Reply");
    break;
  case CMD_Set_HF_Gain_Level:
    DBG_EVENTS((char *)"Set HF Gain Level");
    break;
  case CMD_EQ_Mode_Setting:
    DBG_EVENTS((char *)"EQ Mode Setting");
    break;
  case CMD_DSP_NR_CTRL:
    DBG_EVENTS((char *)"DSP NR CTRL");
    break;
  case CMD_GPIO_Control:
    DBG_EVENTS((char *)"GPIO Control");
    break;
  case CMD_MCU_UART_Rx_Buffer_Size:
    DBG_EVENTS((char *)"MCU UART Rx Buffer Size");
    break;
  case CMD_Voice_Prompt_Cmd:
    DBG_EVENTS((char *)"Voice Prompt Cmd");
    break;
  case CMD_MAP_REQUEST:
    DBG_EVENTS((char *)"MAP REQUEST");
    break;
  case CMD_Security_Bonding_Req:
    DBG_EVENTS((char *)"Security Bonding Req");
    break;
  case CMD_Set_Overall_Gain:
    DBG_EVENTS((char *)"Set Overall Gain");
    break;
  default:
    DBG_EVENTS((char *)"Unknown CMD command");
    break;
  }
}

void IS2020::decodeRejectReason(uint8_t event)
{
  switch (event)
  {
  case AVRCP_STATUS_INVALID_COMMAND:
  {
    DBG_AVRCP((char *)"INVALID COMMAND: ");
  }
  break;
  case AVRCP_STATUS_INVALID_PARAM:
  {
    DBG_AVRCP((char *)"INVALID PARAM: ");
  }
  break;
  case AVRCP_STATUS_PARAM_NOT_FOUND:
  {
    DBG_AVRCP((char *)"PARAM NOT FOUND: ");
  }
  break;
  case AVRCP_STATUS_INTERNAL_ERROR:
  {
    DBG_AVRCP((char *)"INTERNAL ERROR: ");
  }
  break;
  case AVRCP_STATUS_SUCCESS:
  {
    DBG_AVRCP((char *)"SUCCESS");
  }
  break;
  case AVRCP_STATUS_UID_CHANGED:
  {
    DBG_AVRCP((char *)"UID CHANGED");
  }
  break;
  case AVRCP_STATUS_INVALID_DIRECTION:
  {
    DBG_AVRCP((char *)"INVALID DIRECTION");
  }
  break;
  case AVRCP_STATUS_NOT_DIRECTORY:
  {
    DBG_AVRCP((char *)"NOT DIRECTORY");
  }
  break;
  case AVRCP_STATUS_NOT_EXIST:
  {
    DBG_AVRCP((char *)"NOT EXIST");
  }
  break;
  case AVRCP_STATUS_INVALID_SCOPE:
  {
    DBG_AVRCP((char *)"INVALID SCOPE");
  }
  break;
  case AVRCP_STATUS_RANGE_OUT_OF_BOUNDS:
  {
    DBG_AVRCP((char *)"RANGE OUT OF BOUNDS");
  }
  break;
  case AVRCP_STATUS_FOLDER_NOT_PLAYABLE:
  {
    DBG_AVRCP((char *)"FOLDER NOT PLAYABLE");
  }
  break;
  case AVRCP_STATUS_MEDIA_IN_USE:
  {
    DBG_AVRCP((char *)"MEDIA IN USE");
  }
  break;
  case AVRCP_STATUS_NOW_PLAYING_LIST_FULL:
  {
    DBG_AVRCP((char *)"NOW PLAYING LIST FULL");
  }
  break;
  case AVRCP_STATUS_SEARCH_NOT_SUPPORTED:
  {
    DBG_AVRCP((char *)"SEARCH NOT SUPPORTED");
  }
  break;
  case AVRCP_STATUS_SEARCH_IN_PROGRESS:
  {
    DBG_AVRCP((char *)"SEARCH IN PROGRESS");
  }
  break;
  case AVRCP_STATUS_INVALID_PLAYER_ID:
  {
    DBG_AVRCP((char *)"INVALID PLAYER ID");
  }
  break;
  case AVRCP_STATUS_PLAYER_NOT_BROWSABLE:
  {
    DBG_AVRCP((char *)"PLAYER NOT BROWSABLE");
  }
  break;
  case AVRCP_STATUS_PLAYER_NOT_ADDRESSED:
  {
    DBG_AVRCP((char *)"PLAYER NOT ADDRESSED");
  }
  break;
  case AVRCP_STATUS_NO_VALID_SEARCH_RESULT:
  {
    DBG_AVRCP((char *)"NO VALID SEARCH RESULT");
  }
  break;
  case AVRCP_STATUS_NO_AVAILABLE_PLAYERS:
  {
    DBG_AVRCP((char *)"NO AVAILABLE PLAYERS");
  }
  break;
  case AVRCP_STATUS_ADDRESSED_PLAYER_CHANGED:
  {
    DBG_AVRCP((char *)"ADDRESSED PLAYER CHANGED");
  }
  break;
  }
}
