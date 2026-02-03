#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3
#define MOTOR_IN1 6
#define MOTOR_IN2 7

#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

#define WHEEL_DIAMETER_MM 65.0f
#define WHEEL_CIRCUMFERENCE_MM 204.20f
#define COUNTS_PER_REV 2080.0f
#define MM_PER_COUNT 0.09817f

#define AS5600_ADDR 0x36

// defaults
#define PENDULUM_SETPOINT -6.0f
#define CART_KP 15.0f
#define CART_KI 0.1f
#define CART_KD 0.5f

#define CMD_UPDATE_KP 0x01
#define CMD_UPDATE_KI 0x02
#define CMD_UPDATE_KD 0x03

#endif
