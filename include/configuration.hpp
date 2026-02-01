#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

// --- Cart Specific Pins (YD-RP2040) ---
#define ENCODER_PIN_A 2 // Quadrature A
#define ENCODER_PIN_B 3 // Quadrature B
#define MOTOR_IN1 5     // PWM A
#define MOTOR_IN2 6     // PWM B

#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5

// --- Cart Physics ---
#define WHEEL_DIAMETER_MM 65.0f
#define WHEEL_CIRCUMFERENCE_MM 204.20f // PI * D
#define COUNTS_PER_REV 2080.0f         // 520 * 4
#define MM_PER_COUNT 0.09817f          // CIRC / COUNTS

// --- AS5600 Logic ---
// The AS5600 has a 12-bit resolution (0-4095)
#define AS5600_ADDR 0x36

// --- PID Defaults (Pendulum) ---
#define PENDULUM_SETPOINT -6.0f
#define CART_KP 15.0f
#define CART_KI 0.1f
#define CART_KD 0.5f

#define CMD_UPDATE_KP 0x01
#define CMD_UPDATE_KI 0x02
#define CMD_UPDATE_KD 0x03

#endif
