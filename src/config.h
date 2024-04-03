#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include <cstdint>

// #define WHEEL_FL {26, 25, true}
// #define WHEEL_FR {17, 16, false}
// #define WHEEL_RL {33, 32, true}
// #define WHEEL_RR {19, 18, false}

// fred
#define WHEEL_FL {26, 25, true, 0.6f}
#define WHEEL_FR {17, 16, false}
#define WHEEL_RL {33, 32, true, 0.6f}
#define WHEEL_RR {19, 18, false}

namespace
{
    // uint8_t const WHEEL_FL_P = 26;
    // uint8_t const WHEEL_FL_N = 25;
    // uint8_t const WHEEL_FR_P = 17;
    // uint8_t const WHEEL_FR_N = 16;
    // uint8_t const WHEEL_RL_P = 33;
    // uint8_t const WHEEL_RL_N = 32;
    // uint8_t const WHEEL_RR_P = 19;
    // uint8_t const WHEEL_RR_N = 18;

    float const MECANUM_SPEED_LOW  = 0.5f;
    float const MECANUM_SPEED_HIGH = 1.0f;

#ifdef HM10_SERIAL
    uint8_t const HM10_SERIAL_RX = 35;
    uint8_t const HM10_SERIAL_TX = 23;
#endif
}

#endif