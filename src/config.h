#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include <cstdint>

#define MECANUM_FL {26, 25, true}
#define MECANUM_FR {17, 16, false}
#define MECANUM_RL {33, 32, true}
#define MECANUM_RR {19, 18, false}

// fred
// #define MECANUM_FL {26, 25, true, 0.6f}
// #define MECANUM_FR {17, 16, false}
// #define MECANUM_RL {33, 32, true, 0.6f}
// #define MECANUM_RR {19, 18, false}

#define MECANUM_SPEED_LOW  0.5f
#define MECANUM_SPEED_HIGH 1.0f

#ifdef HM10_SERIAL
#define HM10_SERIAL_RX 35
#define HM10_SERIAL_TX 23
#endif

#endif