#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#define SLIDER_THRESHOLD 0.5f

// wheels (motor driver) pins
#define MECANUM_FRONT_LEFT {26, 25, true}
#define MECANUM_REAR_LEFT {33, 32, true}
#define MECANUM_FRONT_RIGHT {16, 4, false}
#define MECANUM_REAR_RIGHT {18, 17, false}

#define MECANUM_SPEED_LOW  0.2f
#define MECANUM_SPEED_HIGH 1.0f
// #define MECANUM_SPEED_PAD 0.8f

// tools (motor driver) pins
#define MAGNET_UPDOWN {23, 22, false, 1.0f, false}
#define BONUS_UPDOWN {21, 19, false, 1.0f, false}

// claw (servo) command pin
#define BONUS_CLAW 27

// bluetooth / serial module pins
#ifdef HM10_SERIAL
#define HM10_SERIAL_RX 35
#define HM10_SERIAL_TX 13
#endif

// boot led pin
#define MEC2105_DBG_BOOT 2

// #define MEC2105_DISABLE_BURNOUT_DETECTOR

#endif
