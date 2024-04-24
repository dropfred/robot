#include <mecanum.h>

#include <utility>
#include <cmath>

namespace
{
    Mecanum::Dir operator - (Mecanum::Dir d)
    {
        return (d == Mecanum::Dir::N) ? Mecanum::Dir::P
             : (d == Mecanum::Dir::P) ? Mecanum::Dir::N
             :                          Mecanum::Dir::Z;
    }
}

Mecanum::Mecanum(Motor && fl, Motor && fr, Motor && rl, Motor && rr) :
    wheels
    {
        {std::move(fl), Dir::P, Dir::P},
        {std::move(fr), Dir::N, Dir::P},
        {std::move(rl), Dir::N, Dir::P},
        {std::move(rr), Dir::P, Dir::P}
    }
{
}

Mecanum::~Mecanum() {}

void Mecanum::move(Dir x, Dir y, float speed)
{
    for (auto const & w : wheels)
    {
        float s = 0.0f;
        bool zx = (x == Dir::Z), zy = (y == Dir::Z);
        if (!zx || !zy)
        {
            if ((zx || (x == w.x)) && (zy || (y == w.y)))
            {
                s = speed;
            }
            else if ((zx || (x == -w.x)) && (zy || (y == -w.y)))
            {
                s = -speed;
            }
        }
        w.motor.run(s);
    }
}

void Mecanum::stop()
{
    for (auto const & w : wheels)
    {
        w.motor.stop();
    }
}

void Mecanum::brake(float force)
{
    for (auto & w : wheels)
    {
        w.motor.brake(force);
    }
}

void Mecanum::rotate(float speed)
{
    int r = 1;
    for (auto & w : wheels)
    {
        w.motor.run(speed * r);
        r = -r;
    }
}

#ifdef MECANUM_AUTO_TEST
#include <Arduino.h>

void Mecanum::test()
{
    static char const * names[] = {"FL", "FR", "RL", "RR"};

    delay(3000);

    auto test = [this] (size_t w, float s)
    {
        wheels[w].motor.run(s);
        delay(1000);
        wheels[w].motor.brake();
        delay(10);
        wheels[w].motor.stop();
    };

    for (size_t i = 0; i < 4; ++i)
    {
        Serial.printf("+ %s\n", names[i]);
        test(i, 1.0f);
        delay(1000);
        Serial.printf("- %s\n", names[i]);
        test(i, -1.0f);
        delay(1000);
    }
}
#endif