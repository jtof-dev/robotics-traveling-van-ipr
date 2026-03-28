#include "configuration.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pid.h"
#include <math.h>
#include <stdio.h>

extern "C" {
#include "dwm_pico_AS5600.h"
}

volatile long encoder_count = 0;

void encoder_callback(uint gpio, uint32_t events) {
  if (gpio == ENCODER_PIN_A) {
    if (gpio_get(ENCODER_PIN_A) == gpio_get(ENCODER_PIN_B))
      encoder_count--;
    else
      encoder_count++;
  } else {
    if (gpio_get(ENCODER_PIN_B) == gpio_get(ENCODER_PIN_A))
      encoder_count++;
    else
      encoder_count--;
  }
}

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

void core1_entry() {
  while (1) {
    tight_loop_contents();
  }
}

int main() {
  stdio_init_all();
  sleep_ms(2000);

  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(SDA_PIN);
  gpio_pull_up(SCL_PIN);

  gpio_init(DIR_PIN);
  gpio_set_dir(DIR_PIN, GPIO_OUT);
  gpio_put(DIR_PIN, 0);

  as5600_t pendulum_encoder = {0};
  as5600_init(SDA_PIN, SCL_PIN, &pendulum_encoder);

  gpio_set_function(MOTOR_IN1, GPIO_FUNC_PWM);
  gpio_set_function(MOTOR_IN2, GPIO_FUNC_PWM);
  uint slice = pwm_gpio_to_slice_num(MOTOR_IN1);
  pwm_set_wrap(slice, 255);
  pwm_set_enabled(slice, true);

  gpio_init(ENCODER_PIN_A);
  gpio_init(ENCODER_PIN_B);
  gpio_pull_up(ENCODER_PIN_A);
  gpio_pull_up(ENCODER_PIN_B);
  gpio_set_irq_enabled_with_callback(ENCODER_PIN_A,
                                     GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                     true, &encoder_callback);
  gpio_set_irq_enabled(ENCODER_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                       true);

  float angle = 0.0f;
  float output = 0.0f;
  float set_point = PENDULUM_SETPOINT;

  PID cartPID(&angle, &output, &set_point, CART_KP, CART_KI, CART_KD, REVERSE);
  cartPID.SetMode(AUTOMATIC);
  cartPID.SetOutputLimits(-255, 255);

  multicore_launch_core1(core1_entry);

  while (true) {
    if (multicore_fifo_rvalid()) {
      uint32_t cmd = multicore_fifo_pop_blocking();
      if (cmd == CMD_UPDATE_KP) {
        uint32_t raw_val = multicore_fifo_pop_blocking();
        float new_kp = (float)raw_val / 100.0f;
        cartPID.SetTunings(new_kp, cartPID.GetKi(), cartPID.GetKd());
      }
    }

    float robot_distance_mm = encoder_count * MM_PER_COUNT;
    if (fabs(robot_distance_mm) > MAX_DISTANCE_MM) {
      set_motors(0);
      printf("Max distance reached, stopping motors.\n");
      while (true) {
        tight_loop_contents();
      }
    }

    uint16_t raw_angle = as5600_read_raw_angl(&pendulum_encoder);
    angle = ((float)raw_angle / 4095.0f * 360.0f) - 180.0f;

    // if (fabs(angle - set_point) > 45.0f) {
    //   set_motors(0);
    //   sleep_ms(100);
    //   continue;
    // }

    cartPID.Compute();

    if (fabs(output) < (fabs(set_point) + 2)) {
      output = 0;
    }

    set_motors(output);

    printf("A: %.2f | O: %.1f | E: %ld\n", angle, output, encoder_count);

    // 0x0B is the STATUS register address
    uint8_t reg = 0x0B;
    uint8_t status = 0;

    // Request the status register
    i2c_write_blocking(I2C_PORT, 0x36, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, 0x36, &status, 1, false);

    // Extract the diagnostic bits
    bool md = (status >> 5) & 1; // MD: Magnet Detected
    bool ml = (status >> 4) & 1; // ML: Magnet too weak (too far away)
    bool mh = (status >> 3) & 1; // MH: Magnet too strong (too close)

    printf(
        "Raw Status: 0x%02X | Detected: %d | Too Weak: %d | Too Strong: %d\n",
        status, md, ml, mh);

    sleep_ms(200);
  }

  return 0;
}
