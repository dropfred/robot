#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#define SLIDER_THRESHOLD 0.5f

// wheels (motor driver) pins
#define MECANUM_FRONT_LEFT {26, 25, true}
#define MECANUM_FRONT_RIGHT {17, 16, false}
#define MECANUM_REAR_LEFT {33, 32, true}
#define MECANUM_REAR_RIGHT {19, 18, false}

// fred
// #define MECANUM_FRONT_LEFT {26, 25, true, 0.6f}
// #define MECANUM_FRONT_RIGHT {17, 16, false}
// #define MECANUM_REAR_LEFT {33, 32, true, 0.6f}
// #define MECANUM_REAR_RIGHT {19, 18, false}

#define MECANUM_SPEED_LOW  0.5f
#define MECANUM_SPEED_HIGH 1.0f

// tools (motor driver) pins
#define MAGNET_UPDOWN {4, 21, false, 1.0f, false}
#define BONUS_UPDOWN {27, 13, false, 1.0f, false}

// claw (servo) command pin
#define BONUS_CLAW 22

// bluetooth / serial module pins
#ifdef HM10_SERIAL
#define HM10_SERIAL_RX 35
#define HM10_SERIAL_TX 23
#endif

// boot led pin
#define DBG_BOOT 2

#endif
