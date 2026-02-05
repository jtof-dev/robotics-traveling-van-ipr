#include "configuration.hpp"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>


void set_motors(float power) {
  int pwm_val = (int)fabs(power);
  if (pwm_val > 255)
    pwm_val = 255;

  if (power > 0) {
    pwm_set_gpio_level(MOTOR_IN1, pwm_val);
    pwm_set_gpio_level(MOTOR_IN2, 0);
  } else if (power < 0) {
    pwm_set_gpio_level(MOTOR_IN1, 0);
    pwm_set_gpio_level(MOTOR_IN2, pwm_val);
  } else {
    pwm_set_gpio_level(MOTOR_IN1, 255);
    pwm_set_gpio_level(MOTOR_IN2, 255);
  }
}

int main() {
  stdio_init_all();
  sleep_ms(2000);

  gpio_set_function(MOTOR_IN1, GPIO_FUNC_PWM);
  gpio_set_function(MOTOR_IN2, GPIO_FUNC_PWM);
  uint slice = pwm_gpio_to_slice_num(MOTOR_IN1);
  pwm_set_wrap(slice, 255);
  pwm_set_enabled(slice, true);

  while (true) {
    // Spin Forward (approx 50% speed)
    printf("Spinning Forward\n");
    set_motors(128.0f);
    sleep_ms(4000);

    // Stop briefly
    set_motors(0);
    sleep_ms(500);

    // Spin Backward
    printf("Spinning Backward\n");
    set_motors(-128.0f);
    sleep_ms(4000);

    // Stop briefly
    set_motors(0);
    sleep_ms(500);
  }

  return 0;
}
