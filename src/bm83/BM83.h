#ifndef BM83_H
#define BM83_H

#ifdef __cplusplus
extern "C" {
#endif
void BM83_LOG_HEXDUMP(uint8_t *data, uint16_t data_size);
void bm83_init();
void bm83_setup();
void bm83_reset();
void bm83_enable_device();
void bm83_disable_device();
void bm83_decode_event();
void bm83_virtual_power_off();
void bm83_virtual_power_on();
void bm83_set_enable();
void bm83_enable_pair_mode();
void iso2083_make_call(char phoneNumber[19]);
#ifdef __cplusplus
}
#endif

#endif