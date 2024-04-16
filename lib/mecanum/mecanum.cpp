#include <mecanum.h>

#include <utility>
#include <cmath>

#define MECANUM_KICK 5

namespace
{
    Mecanum::Dir operator - (Mecanum::Dir d)
    {
        return (d == Mecanum::Dir::N) ? Mecanum::Dir::P
             : (d == Mecanum::Dir::P) ? Mecanum::Dir::N
             :                          Mecanum::Dir::Z;
    }
}

Mecanum::Mecanum(Motor && fl, Motor && fr, Motor && rl, Motor && rr) noexcept :
    wheels
    {
        {std::move(fl), Dir::P, Dir::P},
        {std::move(fr), Dir::N, Dir::P},
        {std::move(rl), Dir::N, Dir::P},
        {std::move(rr), Dir::P, Dir::P}
    }
{
}

Mecanum::~Mecanum() noexcept {}

void Mecanum::update() noexcept
{
#ifdef MECANUM_KICK
    bool k = false;
    for (auto & w : wheels)
    {
        float r = (w.update > w.speed) ? 1.0f : (w.update < w.speed) ? -1.0f : 0.0f;
        if (r != 0.0f)
        {
            w.motor.run(r);
            k = true;
        }
    }
    if (k)
    {
        delay(MECANUM_KICK);
    }
#endif
    for (auto & w : wheels)
    {
        w.speed = w.update;
        w.motor.run(w.speed);
    }
}

void Mecanum::move(Dir x, Dir y, float speed) noexcept
{
    for (auto & w : wheels)
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
        w.update = s;
    }
    update();
}

void Mecanum::stop() noexcept
{
    for (auto & w : wheels)
    {
        w.update = 0.0f;
    }
    update();
}

void Mecanum::brake(float force) noexcept
{
    for (auto & w : wheels)
    {
        w.speed = w.update = 0.0f;
        w.motor.brake(force);
    }
}

void Mecanum::rotate(float speed) noexcept
{
    int r = 1;
    for (auto & w : wheels)
    {
        w.update = speed * r;
        r = -r;
    }
    update();
}

#ifdef MECANUM_AUTO_TEST
#include <Arduino.h>

void Mecanum::test() noexcept
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