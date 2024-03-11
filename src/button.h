#ifndef __BUTTON_H__
#define __BUTTON_H__

#define BOTTON_GPIO_STATE_PRESSED    1
#define BOTTON_GPIO_STATE_RELEASED   0

#define BUTTON_PRESS_SHORT_TIME_MS      100
#define BUTTON_PRESS_LONG_TIME_MS       1000
#define BUTTON_PRESS_VERY_LONG_TIME_MS  3000

int button_init();

#endif