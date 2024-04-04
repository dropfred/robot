#include <Arduino.h>

#include <ESP32_Servo.h>

#include "config.h"

#include <mecanum.h>
#include <xblue.h>

#include <algorithm>

// #define MEC2105_DISABLE_BURNOUT_DETECTOR
#ifdef MEC2105_DISABLE_BURNOUT_DETECTOR
#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>
#endif

namespace
{
    std::string const MEC2105_NAME {"MEC2105-04"};

    struct
    {
        Mecanum mec;
        struct
        {
            struct
            {
                Motor up_down;
            } waste;

            struct
            {
                Motor up_down;
                Servo claw;
            } bonus;
            
        } tool;
    } robot
    {
        {
            MECANUM_FL,
            MECANUM_FR,
            MECANUM_RL,
            MECANUM_RR
        },
        {
            {TOOL_MAGNET_UPDOWN},
            {TOOL_BONUS_UPDOWN}
        }
    };
}

void setup()
{
    // TODO : add led during setup to signal reboot ?
    pinMode(DBG_BOOT, OUTPUT);
    digitalWrite(DBG_BOOT, HIGH);

    Serial.begin(115200);
    delay(1000);

#ifdef MEC2105_DISABLE_BURNOUT_DETECTOR
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    // Serial.println("Bournout detector disabled");
#endif

#ifndef HM10_SERIAL
    {
        uint8_t mac[6];

        esp_read_mac(mac, ESP_MAC_BT);
        Serial.print("Bluetooth MAC : ");
        for (size_t i = 0; i < 6; ++i)
        {
            if (i > 0) Serial.print(":");
            Serial.printf("%02X", mac[i]);
        }
        Serial.println("");
    }
#endif

#if defined (XBLUE_MICRO)
    XBlue::on_button("b0", [] (bool v)
    {
        Serial.printf("b0 : %s\n", v ? "on" : "off");
    });

    // auto button = [] (std::string const & name, bool v)
    // {
    //     Serial.printf("button %s : %s\n", name.c_str(), v ? "on" : "off");
    // };

    // XBlue::on_button("b1", button);
    // XBlue::on_button("b2", button);

    XBlue::on_slider("sl0", [] (float v)
    {
        Serial.printf("sl0 : %.2f\n", v);
        // rotate
        v = v * 2 - 1;
        if (std::abs(v) > MECANUM_SPEED_LOW)
        {
            robot.mec.rotate(v);
        }
        else
        {
            robot.mec.stop();
        }
    });

    XBlue::on_slider("claw", [] (float v)
    {
        Serial.printf("claw : %.2f\n", v);
        robot.tool.bonus.claw.write(v * 180);
    });

    XBlue::on_toggle("sw0", [] (float v)
    {
        Serial.printf("sw0 : %s\n", v ? "on" : "off");
    });

    XBlue::on_text("t0", [] (std::string const & txt)
    {
        Serial.printf("t0 : %s\n", txt.c_str());
    });

    auto drive = [] (float x, float y)
    {
        // auto dx = (x < -MECANUM_SPEED_LOW) ? Mecanum::Dir::N : (x > MECANUM_SPEED_LOW) ? Mecanum::Dir::P : Mecanum::Dir::Z;
        // auto dy = (y < -MECANUM_SPEED_LOW) ? Mecanum::Dir::N : (y > MECANUM_SPEED_LOW) ? Mecanum::Dir::P : Mecanum::Dir::Z;
        // if ((dx != Mecanum::Dir::Z) || (dy != Mecanum::Dir::Z))
        // {
        //     float s = std::sqrt(x * x + y * y);
        //     robot.mec.move(dx, dy, s);
        // }
        // else
        // {
        //     robot.mec.stop();
        // }

        float s = std::sqrt(x * x + y * y);
        if (s > MECANUM_SPEED_LOW)
        {
            auto dx = (x < -0.3f) ? Mecanum::Dir::N : (x > 0.3f) ? Mecanum::Dir::P : Mecanum::Dir::Z;
            auto dy = (y < -0.3f) ? Mecanum::Dir::N : (y > 0.3f) ? Mecanum::Dir::P : Mecanum::Dir::Z;
            if (s > MECANUM_SPEED_HIGH)
            {
                s = MECANUM_SPEED_HIGH;
            }
            // auto dir = [] (Mecanum::Dir d)
            // {
            //     return (d == Mecanum::Dir::P) ? "P"
            //          : (d == Mecanum::Dir::N) ? "N"
            //          :                          "Z";
            // };
            // Serial.printf("drive : %.2f / %.2f -> %s / %s (%.2f)\n", x, y, dir(dx), dir(dy), s);
            robot.mec.move(dx, dy, s);
        }
        else
        {
            robot.mec.stop();
        }
    };

    XBlue::on_pad("d0", drive);
    XBlue::on_pad("d1", drive);
#elif defined(XBLUE_ARDUINO)
    XBlue::on_button([] (uint8_t id)
    {
        Serial.printf("button %d\n", id);
    });

    XBlue::on_slider([] (uint8_t id, float value)
    {
        Serial.printf("slider %d : %.2f\n", id, value);
        value = value * 2 - 1;
        if (std::abs(value) > MECANUM_SPEED_LOW)
        {
            robot.mec.rotate(value);
        }
        else
        {
            robot.mec.stop();
        }
    });

    XBlue::on_drive([] (float throttle, float steering)
    {
        float t = std::abs(throttle);
        float s = std::abs(steering);
        float d = std::sqrt(t * t + s * s);
        auto x = Mecanum::Dir::Z;
        auto y = Mecanum::Dir::Z;
        if (s > 0.3f)
        {
            x = (steering > 0.0f) ? Mecanum::Dir::P : Mecanum::Dir::N;
        }
        if ((s < 0.5f) && (t > 0.3f))
        {
            y = (throttle > 0.0f) ? Mecanum::Dir::P : Mecanum::Dir::N;
        }
        auto dir = [] (Mecanum::Dir d)
        {
            return (d == Mecanum::Dir::P) ? "P"
                 : (d == Mecanum::Dir::N) ? "N"
                 :                          "Z";
        };
        Serial.printf("drive : %.2f / %.2f -> %s / %s (%.2f)\n", throttle, steering, dir(x), dir(y), d);
        if (d > MECANUM_SPEED_LOW)
        {
            if (d > MECANUM_SPEED_HIGH)
            {
                d = MECANUM_SPEED_HIGH;
            }
            robot.mec.move(x, y, d);
        }
        else
        {
            robot.mec.stop();
        }
    });
#endif

    robot.tool.bonus.claw.attach(TOOL_BONUS_CLAW, 500, 2400);                                       
    robot.tool.bonus.claw.write(0);

#ifdef HM10_SERIAL
    XBlue::start(MEC2105_NAME + " (S)", HM10_SERIAL_RX, HM10_SERIAL_TX);
#else
    XBlue::start(MEC2105_NAME);
#endif

    Serial.println((MEC2105_NAME + " started").c_str());
    digitalWrite(DBG_BOOT, LOW);
}

void loop()
{
#ifdef HM10_SERIAL
    XBlue::update();
    delay(10);
#else
    // do nothing
    delay(1000);
#endif
}
