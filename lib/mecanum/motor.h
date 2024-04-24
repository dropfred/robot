#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#include <functional>
#include <variant>
#include <utility>

class Motor
{
    static std::function<void (uint8_t, uint8_t)> writer(bool pwm)
    {
        return pwm ? [] (uint8_t p, uint8_t v) {analogWrite(p, v);}
                   : [] (uint8_t p, uint8_t v) {digitalWrite(p, (v == 0) ? LOW : HIGH);};
    }

    std::function<void ()> release;

    std::function<void (uint8_t, uint8_t)> write;

    struct D2 {uint8_t pin_1,  pin_2;};
    struct D3 {uint8_t pin_1,  pin_2, pin_enable;};
    std::variant<D2, D3> driver;

    struct
    {
        std::function<void (float)> run;
        std::function<void ()> stop;
        std::function<void (float)> brake;
    } drive;

    Motor() : release {}
    {
    }

public:

    Motor(uint8_t pin_1, uint8_t pin_2, bool reverse = false, bool pwm = true) : Motor()
    {
        driver = D2 {pin_1, pin_2};

        write = Motor::writer(pwm);

        drive.run = [this, s = 255 * (reverse ? -1 : 1)] (float speed)
        {
            D2 & d = std::get<D2>(driver);
            speed *= s;
            if (speed > 0)
            {
                write(d.pin_1, uint8_t(speed));
                write(d.pin_2, 0);
            }
            else
            {
                write(d.pin_1, 0);
                write(d.pin_2, uint8_t(-speed));
            }
        };

        drive.brake = [this] (float force)
        {
            D2 & d = std::get<D2>(driver);
            write(d.pin_1, uint8_t(255 * force));
            write(d.pin_2, uint8_t(255 * force));
        };

        drive.stop = [this] ()
        {
            D2 & d = std::get<D2>(driver);
            write(d.pin_1, 0);
            write(d.pin_2, 0);
        };

        if (pwm)
        {
            release = [this] ()
            {
                D2 & d = std::get<D2>(driver);
                ledcDetachPin(d.pin_1);
                ledcDetachPin(d.pin_2);
            };
        }

        pinMode(pin_1, OUTPUT);
        pinMode(pin_2, OUTPUT);

        stop();
    }

    Motor(uint8_t pin_1, uint8_t pin_2, uint8_t pin_enable, bool reverse = false, bool pwm = true) : Motor()
    {
        driver = D3 {pin_1, pin_2, pin_enable};

        write = Motor::writer(pwm);

        drive.run = [this, s = 255 * (reverse ? -1 : 1)] (float speed)
        {
            D3 & d = std::get<D3>(driver);
            speed *= s;
            if (speed > 0)
            {
                digitalWrite(d.pin_1, HIGH);
                digitalWrite(d.pin_2, LOW);
                write(d.pin_enable, uint8_t(speed));
            }
            else
            {
                digitalWrite(d.pin_1, LOW);
                digitalWrite(d.pin_2, HIGH);
                write(d.pin_enable, uint8_t(-speed));
            }
        };

        drive.brake = [this] (float force)
        {
            D3 & d = std::get<D3>(driver);
            digitalWrite(d.pin_1, HIGH);
            digitalWrite(d.pin_2, HIGH);
            write(d.pin_enable, force);
        };

        drive.stop = [this] ()
        {
            D3 & d = std::get<D3>(driver);
            digitalWrite(d.pin_1, LOW);
            digitalWrite(d.pin_2, LOW);
            write(d.pin_enable, 0);
        };

        if (pwm)
        {
            release = [this] ()
            {
                D3 & d = std::get<D3>(driver);
                ledcDetachPin(d.pin_enable);
            };
        }

        pinMode(pin_1, OUTPUT);
        pinMode(pin_2, OUTPUT);
        pinMode(pin_enable, OUTPUT);

        stop();
    }

    Motor(Motor const & m) = delete;

    Motor(Motor && m) : Motor()
    {
        *this = std::move(m);
    }

    ~Motor()
    {
        if (release)
        {
            release();
        }
    }

    Motor & operator = (Motor const & m) = delete;

    Motor & operator = (Motor && m)
    {
        std::swap(drive, m.drive);
        return *this;
    }

    void run(float speed = 1.0f) const
    {
        drive.run(speed);
    }

    void stop() const
    {
        drive.stop();
    }

    void brake(float force = 1.0f) const
    {
        drive.brake(force);
    }
};

#endif