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

// Updated set_motors with Deadband Mapping
void set_motors(float power) {
  // 1. Handle complete stop
  if (power == 0.0f) {
    pwm_set_gpio_level(MOTOR_IN1,
                       255); // Or 0, depending on your brake preference
    pwm_set_gpio_level(MOTOR_IN2, 255);
    return;
  }

  // 2. Map requested power to the usable motor range
  float abs_power = fabs(power);
  if (abs_power > 255.0f)
    abs_power = 255.0f;

  float mapped_power =
      MOTOR_DEADBAND + (abs_power / 255.0f) * (255.0f - MOTOR_DEADBAND);
  int pwm_val = (int)mapped_power;

  // 3. Drive motors based on original sign
  if (power > 0) {
    pwm_set_gpio_level(MOTOR_IN1, pwm_val);
    pwm_set_gpio_level(MOTOR_IN2, 0);
  } else if (power < 0) {
    pwm_set_gpio_level(MOTOR_IN1, 0);
    pwm_set_gpio_level(MOTOR_IN2, pwm_val);
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
  float output = 0.0f; // This is now FORCE
  float set_point = PENDULUM_SETPOINT;

  // PID Output limits changed to match MAX_FORCE
  PID cartPID(&angle, &output, &set_point, CART_KP, CART_KI, CART_KD, REVERSE);
  cartPID.SetMode(AUTOMATIC);
  cartPID.SetOutputLimits(-MAX_FORCE, MAX_FORCE);

  multicore_launch_core1(core1_entry);

  float current_speed = 0.0f;
  uint32_t last_time = time_us_32();

  while (true) {
    // 1. Calculate time delta (dt)
    uint32_t current_time = time_us_32();
    float dt = (current_time - last_time) / 1000000.0f;
    last_time = current_time;

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

    // 2. Compute PID to get requested Force
    cartPID.Compute();
    float force = output;

    if (fabs(angle - set_point) < 2.0f) {
      force = 0.0f; // Deadband for perfect balance
    }

    // 3. Physics Pipeline: Force -> Accel -> Speed -> PWM
    float acceleration = force / CART_MASS;
    current_speed += (acceleration * dt);

    // Slight damping to prevent infinite drift
    current_speed *= 0.98f;

    if (current_speed > MAX_SPEED)
      current_speed = MAX_SPEED;
    if (current_speed < -MAX_SPEED)
      current_speed = -MAX_SPEED;

    float target_pwm = current_speed * SPEED_TO_PWM_RATIO;
    set_motors(target_pwm);

    printf("A: %.2f | F: %.1f | S: %.2f | PWM: %.0f | E: %ld\n", angle, force,
           current_speed, target_pwm, encoder_count);

    // 4. AS5600 Magnetic Status Decoding
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

    // Run much faster for pendulum stability
    sleep_ms(10);
  }

  return 0;
}
