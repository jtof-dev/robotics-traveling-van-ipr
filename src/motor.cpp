#include "motor.hpp"
#include "configuration.hpp"
#include <math.h>

MOTOR::MOTOR(int in1_pin, int in2_pin) {
  _in1 = in1_pin;
  _in2 = in2_pin;

  gpio_set_function(_in1, GPIO_FUNC_PWM);
  gpio_set_function(_in2, GPIO_FUNC_PWM);

  uint slice1 = pwm_gpio_to_slice_num(_in1);
  uint slice2 = pwm_gpio_to_slice_num(_in2);

  // set 8-bit wrap (0-255)
  pwm_set_wrap(slice1, 255);
  pwm_set_wrap(slice2, 255);

  pwm_set_enabled(slice1, true);
  pwm_set_enabled(slice2, true);

  set_power(0.0f);
}

void MOTOR::set_power(float power) {
  // handle complete stop
  if (power == 0.0f) {
    pwm_set_gpio_level(_in1, 255); // brake preference
    pwm_set_gpio_level(_in2, 255);
    return;
  }

  // map requested power to the usable motor range
  float abs_power = fabs(power);
  if (abs_power > 255.0f)
    abs_power = 255.0f;

  float mapped_power =
      MOTOR_DEADBAND + (abs_power / 255.0f) * (255.0f - MOTOR_DEADBAND);
  int pwm_val = (int)mapped_power;

  // drive motors based on original sign
  if (power > 0) {
    pwm_set_gpio_level(_in1, pwm_val);
    pwm_set_gpio_level(_in2, 0);
  } else if (power < 0) {
    pwm_set_gpio_level(_in1, 0);
    pwm_set_gpio_level(_in2, pwm_val);
  }
}
