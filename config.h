#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#define HM10_BLE_NATIVE 1
#define HM10_BLE_SERIAL 2

#ifndef HM10_BLE_CFG
#define HM10_BLE_CFG HM10_BLE_NATIVE
#endif

#define XBLUE_INTERFACE_MICRO 1
#define XBLUE_INTERFACE_ARDUINO 2

#ifndef XBLUE_INTERFACE_CFG
#define XBLUE_INTERFACE_CFG XBLUE_INTERFACE_MICRO
#endif

#define SLIDER_THRESHOLD 0.5f

// wheels (motor driver) pins
#define MECANUM_FRONT_LEFT {26, 25, true}
#define MECANUM_REAR_LEFT {33, 32, true}
#define MECANUM_FRONT_RIGHT {16, 4, false}
#define MECANUM_REAR_RIGHT {18, 17, false}

#define MECANUM_SPEED_LOW  0.2f
#define MECANUM_SPEED_HIGH 0.7f
// #define MECANUM_SPEED_PAD 0.8f

#define MECANUM_BRAKE_DURATION 10

// tools (motor driver) pins
#define MAGNET_UPDOWN {23, 22, false, 1.0f, false}
#define BONUS_UPDOWN {21, 19, false, 1.0f, false}

// claw (servo) command pin
#define BONUS_CLAW 27

// bluetooth / serial module pins
#if ! HM10_NATIVE_BLE
#define HM10_SERIAL_RX 35
#define HM10_SERIAL_TX 13
#endif

// boot led pin
#define MEC2105_DBG_BOOT 2

// #define MEC2105_DISABLE_BURNOUT_DETECTOR

#endif
