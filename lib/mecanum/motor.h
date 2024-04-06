#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#include <functional>
#include <utility>

class Motor
{
    struct
    {
        std::function<void (float)> run;
        std::function<void ()> stop;
        std::function<void (float)> brake;
    } drive;

    Motor() noexcept : drive {}
    {
    }

    static std::function<void (uint8_t, uint8_t)> pinWrite(bool pwm)
    {
        std::function<void (uint8_t, uint8_t)> f {};

        if (pwm)
        {
            f = [] (uint8_t p, uint8_t v) {analogWrite(p, v);};
        }
        else
        {
            f = [] (uint8_t p, uint8_t v) {digitalWrite(p, (v == 0) ? LOW : HIGH);};
        }
        return f;
    }

public:

    Motor(uint8_t pin_1, uint8_t pin_2, bool reverse = false, float speed = 1.0f, bool pwm = true) noexcept : Motor()
    {
        auto write = pinWrite(pwm);

        if (speed > 1.0f) speed = 1.0f;

        drive.run = [write, pin_1, pin_2, s = 255 * (reverse ? -1 : 1) * speed] (float speed)
        {
            speed *= s;
            if (speed > 0)
            {
                write(pin_1, uint8_t(speed));
                write(pin_2, 0);
            }
            else
            {
                write(pin_1, 0);
                write(pin_2, uint8_t(-speed));
            }
        };

        drive.stop = [write, pin_1, pin_2] ()
        {
            write(pin_1, 0);
            write(pin_2, 0);
        };

        drive.brake = [write, pin_1, pin_2] (float force)
        {
            write(pin_1, uint8_t(255 * force));
            write(pin_2, uint8_t(255 * force));
        };

        pinMode(pin_1, OUTPUT);
        pinMode(pin_2, OUTPUT);

        stop();
    }

    Motor(uint8_t pin_1, uint8_t pin_2, uint8_t pin_enable, bool reverse = false, float speed = 1.0f, bool pwm = true) noexcept : Motor()
    {
        auto write = pinWrite(pwm);

        if (speed > 1.0f) speed = 1.0f;

        drive.run = [write, pin_1, pin_2, pin_enable, s = 255 * (reverse ? -1 : 1) * speed] (float speed)
        {
            speed *= s;
            if (speed > 0)
            {
                digitalWrite(pin_1, HIGH);
                digitalWrite(pin_2, LOW);
                write(pin_enable, uint8_t(speed));
            }
            else
            {
                digitalWrite(pin_1, LOW);
                digitalWrite(pin_2, HIGH);
                write(pin_enable, uint8_t(-speed));
            }
        };

        drive.stop = [write, pin_1, pin_2, pin_enable] ()
        {
            digitalWrite(pin_1, LOW);
            digitalWrite(pin_2, LOW);
            write(pin_enable, 0);
        };

        drive.brake = [write, pin_1, pin_2, pin_enable] (float force)
        {
            digitalWrite(pin_1, HIGH);
            digitalWrite(pin_2, HIGH);
            write(pin_enable, uint8_t(255 * force));
        };

        pinMode(pin_1, OUTPUT);
        pinMode(pin_2, OUTPUT);
        pinMode(pin_enable, OUTPUT);

        stop();
    }

    Motor(Motor const & m) = delete;

    Motor(Motor && m) noexcept : Motor()
    {
        *this = std::move(m);
    }

    ~Motor() noexcept
    {
    }

    Motor & operator = (Motor const & m) = delete;

    Motor & operator = (Motor && m) noexcept
    {
        std::swap(drive, m.drive);
        return *this;
    }

    void run(float speed = 1.0f) const noexcept
    {
        drive.run(speed);
    }

    void stop() const noexcept
    {
        drive.stop();
    }

    void brake(float force = 1.0f) const noexcept
    {
        drive.brake(force);
    }
};

#endif