#include <Arduino.h>

#include <ESP32_Servo.h>

#include "config.h"

#include <mecanum.h>
#include <xblue.h>

#include <algorithm>

#ifdef MEC2105_DISABLE_BURNOUT_DETECTOR
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
#endif

namespace
{
    std::string const MEC2105_NAME {"MEC2105-44"};

    struct
    {
        struct
        {
            Mecanum mec;
#ifdef MECANUM_BRAKE_DURATION
            bool brake;
#endif
        } wheels;

        struct
        {
            Motor up_down;
        } magnet;

        struct
        {
            Motor up_down;
            Servo claw;
        } bonus;

    } robot
    {
        {
            {
                MECANUM_FRONT_LEFT, MECANUM_FRONT_RIGHT,
                MECANUM_REAR_LEFT , MECANUM_REAR_RIGHT
            }
        },
        {MAGNET_UPDOWN},
        {BONUS_UPDOWN, BONUS_CLAW}
    };

    #define SMOOTH_SERVO_STACK 1024
    #define SMOOTH_SERVO_DELAY 10

    struct
    {
        TaskHandle_t task;
        uint8_t current = 0;
        uint8_t target = 0;
    } smooth {};

    void smooth_servo(void * p)
    {
        // uxTaskGetStackHighWaterMark(NULL)
        while (true)
        {
            if (smooth.current != smooth.target)
            {
                smooth.current += (smooth.current < smooth.target) ? 1 : -1;
                // Serial.printf("current=%d\n", current);
                robot.bonus.claw.write(smooth.current);
            }
            delay(SMOOTH_SERVO_DELAY);
        }
    }
}

void setup()
{
#ifdef MEC2105_DBG_BOOT
    pinMode(MEC2105_DBG_BOOT, OUTPUT);
    digitalWrite(MEC2105_DBG_BOOT, HIGH);
#endif

    Serial.begin(115200);
    delay(1000);

#ifdef MEC2105_DISABLE_BURNOUT_DETECTOR
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    // Serial.println("Bournout detector disabled");
#endif

// #ifndef HM10_SERIAL
//     {
//         uint8_t mac[6];

//         esp_read_mac(mac, ESP_MAC_BT);
//         Serial.print("Bluetooth MAC : ");
//         for (size_t i = 0; i < 6; ++i)
//         {
//             if (i > 0) Serial.print(":");
//             Serial.printf("%02X", mac[i]);
//         }
//         Serial.println("");
//     }
// #endif

    auto stop = [] ()
    {
#ifdef MECANUM_BRAKE_DURATION
        if (robot.wheels.brake)
        {
            // Serial.println("brake");
            robot.wheels.mec.brake();
            delay(MECANUM_BRAKE_DURATION);
            robot.wheels.brake = false;
        }
#endif
        robot.wheels.mec.stop();
    };

    XBlue::on_slider("sl0", [stop] (float v)
    {
        // Serial.printf("sl0 : %.2f\n", v);
        // rotate
        v = v * 2 - 1;
        if (std::abs(v) > MECANUM_SPEED_LOW)
        {
            robot.wheels.mec.rotate(v);
#ifdef MECANUM_BRAKE_DURATION
            robot.wheels.brake = true;
#endif
        }
        else
        {
            stop();
        }
    });

    XBlue::on_slider("mud", [] (float v)
    {
        // Serial.printf("magnet : %.2f\n", v);
        v = v * 2 - 1;
        if (std::abs(v) > SLIDER_THRESHOLD)
        {
            robot.magnet.up_down.run(v);
        }
        else
        {
            robot.magnet.up_down.stop();
        }
    });

    XBlue::on_slider("bud", [] (float v)
    {
        // Serial.printf("bonus : %.2f\n", v);
        v = v * 2 - 1;
        if (std::abs(v) > SLIDER_THRESHOLD)
        {
            robot.bonus.up_down.run(v);
        }
        else
        {
            robot.bonus.up_down.stop();
        }
    });

    XBlue::on_slider("claw", [] (float v)
    {
        // Serial.printf("claw : %.2f\n", v);
        // robot.bonus.claw.write(v * 180);
        smooth.target = v * 180;
    });

    auto drive = [stop] (float x, float y)
    {
        auto dx = (x < -MECANUM_SPEED_LOW) ? Mecanum::Dir::N : (x > MECANUM_SPEED_LOW) ? Mecanum::Dir::P : Mecanum::Dir::Z;
        auto dy = (y < -MECANUM_SPEED_LOW) ? Mecanum::Dir::N : (y > MECANUM_SPEED_LOW) ? Mecanum::Dir::P : Mecanum::Dir::Z;
        if ((dx != Mecanum::Dir::Z) || (dy != Mecanum::Dir::Z))
        {
            robot.wheels.mec.move(dx, dy, std::sqrt(x * x + y * y));
#ifdef MECANUM_BRAKE_DURATION
            robot.wheels.brake = true;
#endif
        }
        else
        {
            stop();
        }
    };

    XBlue::on_pad("d0", drive);
    XBlue::on_pad("d1", drive);

    // robot.bonus.claw.attach(BONUS_CLAW, 500, 2400);
    xTaskCreatePinnedToCore
    (
        smooth_servo,
        "SmoothServo",
        SMOOTH_SERVO_STACK,
        NULL,
        1,
        &smooth.task,
        0
    );
    // smooth.current = smooth.target = robot.bonus.claw.read();
    robot.bonus.claw.write(0);

#ifdef HM10_SERIAL
    XBlue::start(MEC2105_NAME + " (S)", HM10_SERIAL_RX, HM10_SERIAL_TX);
#else
    XBlue::start(MEC2105_NAME);
#endif

    Serial.println((MEC2105_NAME + " started").c_str());

#ifdef MEC2105_DBG_BOOT
    digitalWrite(MEC2105_DBG_BOOT, LOW);
#endif
}

void loop()
{
#ifdef HM10_SERIAL
    XBlue::update();
#endif
}
