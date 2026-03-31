#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define MOTOR_IN1 6
#define MOTOR_IN2 7

#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5
#define DIR_PIN 1

#define WHEEL_DIAMETER_MM 65.0f
#define WHEEL_CIRCUMFERENCE_MM 204.20f
#define COUNTS_PER_REV 2080.0f
#define MM_PER_COUNT 0.09817f

#define AS5600_ADDR 0x36

// defaults
#define PENDULUM_SETPOINT 88.0f
#define CART_KP 15.0f
#define CART_KI 0.1f
#define CART_KD 0.5f

#define MAX_DISTANCE_MM 304.8f
// --- Physics & Motor Mapping Constants ---

// Keep this at 1.0 for now. It effectively removes mass from the
// equation so Force directly equals Acceleration.
#define CART_MASS 1.0f

// Treat this as "100% maximum effort" the PID is allowed to ask for.
#define MAX_FORCE 100.0f

// Treat this as "100% maximum speed".
#define MAX_SPEED 100.0f

// The math: 100 (Max Speed) * 2.55 = 255 (Max 8-bit PWM).
// This cleanly maps your 0-100 speed scale to the hardware limit.
#define SPEED_TO_PWM_RATIO 2.55f

// PLACEHOLDER: You MUST replace this number with the actual value
// you get after running the deadband calibration script!
#define MOTOR_DEADBAND 70.0f

#define CMD_UPDATE_KP 0x01
#define CMD_UPDATE_KI 0x02
#define CMD_UPDATE_KD 0x03

#endif
