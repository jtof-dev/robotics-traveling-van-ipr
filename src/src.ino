#include <Arduino.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <math.h>

extern "C" {
#include "dwm_pico_AS5600.h"
}

#include "configuration.hpp"
#include "pid.h"
#include "motor.hpp"
#include "screen.hpp"
#include "touch.hpp"

// define our system states
enum SystemState {
  STATE_BALANCING,
  STATE_NEEDS_RESET,
  STATE_WAITING
};

// shared volatile variables
volatile float current_angle = 0.0f;
volatile float current_target_speed = 0.0f; // Renamed from current_force
volatile float current_speed = 0.0f;
volatile long current_encoder = 0;
volatile float current_setpoint = PENDULUM_SETPOINT;
volatile bool system_running = true;
volatile SystemState shared_state = STATE_BALANCING;

// core 1 globals
volatile long encoder_count = 0;
MOTOR* cartMotor;
PID* cartPID;
as5600_t pendulum_encoder = {0};

float angle_val = 0.0f;
float output_speed = 0.0f; // Renamed from output_force
float set_point = PENDULUM_SETPOINT;
SystemState current_state = STATE_BALANCING;

// hardware interrupt
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

// core 0
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("--- Pendulum Booting ---");
  Serial.println("Core 0: UI & USB Telemetry Manager Active.");

  initScreen();
  initTouch(); 
}

void loop() {
  uint32_t loopStart = millis();

  // usb telemetry & logging
  static uint32_t lastPrint = 0;
  static SystemState last_tracked_state = STATE_BALANCING;

  if (shared_state != last_tracked_state) {
    if (shared_state == STATE_NEEDS_RESET) {
      Serial.println("\r\nERR: Max distance reached! Motors stopped.\r\n");
    } 
    last_tracked_state = shared_state; 
  }

  // continuous logging (10Hz)
  if (shared_state == STATE_BALANCING) {
    if (millis() - lastPrint > 100) {
      lastPrint = millis();
      // Updated telemetry to reflect target speed rather than force
      Serial.printf("Ang: %6.2f | T_Spd: %6.1f | Spd: %6.2f | Enc: %ld\r\n", 
                    current_angle, current_target_speed, current_speed, current_encoder);
    }
  }

  digitalWrite(LED_BUILTIN, millis() % 1000 < 500 ? HIGH : LOW);

  // touchscreen UI
  static uint32_t lastTouchTime = 0; 
  uint16_t rawX, rawY;

  if (millis() - lastTouchTime > 200) {
    if (readTouch(rawX, rawY)) {
      lastTouchTime = millis();

      uint16_t mappedX = rawY; 
      uint16_t mappedY = 320 - rawX;

      ButtonID btn = checkButtons(mappedX, mappedY);

      switch(btn) {
        case BTN_TOGGLE_BALANCE:
          system_running = !system_running;
          break;
          
        case BTN_RESET:
          encoder_count = 0;
          break;

        case BTN_NONE:
        default: 
          break;
      }
    }
  }

  updateScreen(current_angle, current_speed, current_encoder, rp2040.getFreeHeap(), 
               millis() - loopStart, current_setpoint, system_running);

  delay(50); 
}

// core 1
void setup1() {
  delay(2000);

  // I2C & AS5600 init
  i2c_init(I2C_PORT, 400 * 1000);
  gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(SDA_PIN);
  gpio_pull_up(SCL_PIN);
  as5600_init(SDA_PIN, SCL_PIN, &pendulum_encoder);

  // motor init
  cartMotor = new MOTOR(MOTOR_IN1, MOTOR_IN2);

  // encoder init
  gpio_init(ENCODER_PIN_A);
  gpio_init(ENCODER_PIN_B);
  gpio_pull_up(ENCODER_PIN_A);
  gpio_pull_up(ENCODER_PIN_B);
  gpio_set_irq_enabled_with_callback(ENCODER_PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &encoder_callback);
  gpio_set_irq_enabled(ENCODER_PIN_B, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

  // PID init - Now outputting directly to output_speed limits
  cartPID = new PID(&angle_val, &output_speed, &set_point, CART_KP, CART_KI, CART_KD, REVERSE);
  cartPID->SetMode(AUTOMATIC);
  cartPID->SetOutputLimits(-MAX_SPEED, MAX_SPEED);
  cartPID->SetSampleTime(10); // 10ms (100Hz) to match the loop delay
}

void loop1() {
  static uint32_t last_time = micros();
  static float local_speed = 0.0f;

  // calculate time delta (dt)
  uint32_t current_time = micros();
  float dt = (current_time - last_time) / 1000000.0f;
  last_time = current_time;

  float robot_distance_mm = encoder_count * MM_PER_COUNT;

  if (fabs(robot_distance_mm) > MAX_DISTANCE_MM) {
    current_state = STATE_NEEDS_RESET;
  } else {
    current_state = STATE_BALANCING;
  }

  if (current_state == STATE_NEEDS_RESET || !system_running) {
    cartMotor->set_power(0);
    local_speed = 0.0f;
  } 
  else {
    // read sensors
    uint16_t raw_angle = as5600_read_raw_angl(&pendulum_encoder);
    angle_val = ((float)raw_angle / 4095.0f * 360.0f) - 180.0f;

    set_point = current_setpoint;

    cartPID->Compute();
    
    // Direct speed mapping instead of force -> acceleration physics
    local_speed = output_speed;

    if (fabs(angle_val - set_point) < 2.0f) {
      local_speed = 0.0f; // deadband for perfect balance
    }

    // Safety clamps
    if (local_speed > MAX_SPEED) local_speed = MAX_SPEED;
    if (local_speed < -MAX_SPEED) local_speed = -MAX_SPEED;

    float target_pwm = local_speed * SPEED_TO_PWM_RATIO;
    cartMotor->set_power(target_pwm);
  }

  // push telemetry to Core 0
  current_angle = angle_val;
  current_target_speed = output_speed; // Sharing the target speed from PID
  current_speed = local_speed;
  current_encoder = encoder_count;
  shared_state = current_state;

  delay(10); // 100Hz physics loop
}
