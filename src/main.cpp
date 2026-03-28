#include "configuration.hpp"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

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

void raw_motor_drive(int pwm_val) {
  if (pwm_val > 0) {
    pwm_set_gpio_level(MOTOR_IN1, pwm_val);
    pwm_set_gpio_level(MOTOR_IN2, 0);
  } else if (pwm_val < 0) {
    pwm_set_gpio_level(MOTOR_IN1, 0);
    pwm_set_gpio_level(MOTOR_IN2, abs(pwm_val));
  } else {
    pwm_set_gpio_level(MOTOR_IN1, 0);
    pwm_set_gpio_level(MOTOR_IN2, 0);
  }
}

int main() {
  stdio_init_all();

  // Give you 5 seconds to open the serial monitor and get ready
  printf("Starting deadband calibration in 5 seconds...\n");
  sleep_ms(5000);

  // Init PWM
  gpio_set_function(MOTOR_IN1, GPIO_FUNC_PWM);
  gpio_set_function(MOTOR_IN2, GPIO_FUNC_PWM);
  uint slice = pwm_gpio_to_slice_num(MOTOR_IN1);
  pwm_set_wrap(slice, 255);
  pwm_set_enabled(slice, true);

  // Init Encoders
  gpio_init(ENCODER_PIN_A);
  gpio_init(ENCODER_PIN_B);
  gpio_pull_up(ENCODER_PIN_A);
  gpio_pull_up(ENCODER_PIN_B);
  gpio_set_irq_enabled_with_callback(ENCODER_PIN_A,
                                     GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                     true, &encoder_callback);
  gpio_set_irq_enabled(ENCODER_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                       true);

  int test_pwm = 0;
  encoder_count = 0; // Reset just in case

  printf("Ramping up PWM to find forward deadband...\n");

  while (true) {
    raw_motor_drive(test_pwm);

    // If the encoder has moved more than 3 ticks, we overcame static friction
    if (abs(encoder_count) > 3) {
      raw_motor_drive(0); // STOP!
      printf("========================================\n");
      printf("MOVEMENT DETECTED!\n");
      printf("Your Forward MOTOR_DEADBAND is approx: %d\n", test_pwm);
      printf("========================================\n");

      // Infinite loop to lock the script so it doesn't run away
      while (true) {
        sleep_ms(1000);
      }
    }

    test_pwm++;

    if (test_pwm > 255) {
      raw_motor_drive(0);
      printf("Hit max PWM (255) without movement. Check motor power!\n");
      while (true) {
        sleep_ms(1000);
      }
    }

    // Print current attempt
    printf("Testing PWM: %d | Encoder: %ld\n", test_pwm, encoder_count);

    // Wait 100ms before increasing power to let physics react
    sleep_ms(100);
  }

  return 0;
}
