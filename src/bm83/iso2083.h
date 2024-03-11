#pragma once

/**
 * @file network_manager.h
 * @brief Component for easy network initialisation and switching
 */

#ifdef __cplusplus
extern "C" {
#endif
void bm83_init();
void uart_init();
void iso2083_make_call(char phoneNumber[19]);
void bm83_enable_pair_mode();
void bm83_reset();
#ifdef __cplusplus
}
#endif
