#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <stdint.h>

enum ButtonID { BTN_NONE, BTN_TOGGLE_BALANCE, BTN_RESET };

void initScreen();

void updateScreen(float angle, float speed, long encoder_counts,
                  uint32_t freeRam, uint32_t loopTime, float current_setpoint,
                  bool is_balancing);

ButtonID checkButtons(uint16_t touchX, uint16_t touchY);

#endif
