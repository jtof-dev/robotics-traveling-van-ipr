#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

int MOTOR_IN1 = 6;
int MOTOR_IN2 = 7;

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
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while (true) {
    printf("spinning forward\n");
    gpio_put(LED_PIN, 1);
    set_motors(50.0f);
    sleep_ms(1000);
    /*set_motors(0);
    sleep_ms(500);
    printf("spinning backward\n");
    set_motors(-100.0f);
    sleep_ms(1000);
    set_motors(0);
    sleep_ms(2000);*/
  }

  return 0;
}
