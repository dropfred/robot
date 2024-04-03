#include <mecanum.h>

#include <utility>

#define MOTOR_KICK_DURATION 10

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

void Mecanum::move(Dir x, Dir y, float speed) const noexcept
{
    for (auto const & w : wheels)
    {
        float s = 0.0f;
        bool zx = (x == Dir::Z), zy = (y == Dir::Z);
        if (!zx || !zy)
        {
            // if ((zx || (x && w.x)) && (zy || (y && w.y)))
            if ((zx || (x == w.x)) && (zy || (y == w.y)))
            {
                s = speed;
            }
            // else if ((zx || (x && -w.x)) && (zy || (y && -w.y)))
            else if ((zx || (x == -w.x)) && (zy || (y == -w.y)))
            {
                s = -speed;
            }
        }

#ifdef MOTOR_KICK_DURATION
        if (s != 0.0f)
        {
            w.motor.run((speed > 0.0f) ? 1.0f : -1.0f);
            delay(MOTOR_KICK_DURATION);
        }
#endif

        w.motor.run(s);
    }
}

void Mecanum::stop() const noexcept
{
    for (auto const & w : wheels)
    {
        w.motor.stop();
    }
}

void Mecanum::brake(float force) const noexcept
{
    for (auto const & w : wheels)
    {
        w.motor.brake(force);
    }
}

void Mecanum::rotate(float speed) const noexcept
{
    int r = 1;
    for (auto const & w : wheels)
    {
        w.motor.run(speed * r);
        r = -r;
    }
}
