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
    for (auto & w : wheels)
    {
        if (w.update != 0.0f)
        {
            w.motor.run((w.update > 0.0f) ? 1.0f : -1.0f);
        }
    }
    delay(MECANUM_KICK);
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
