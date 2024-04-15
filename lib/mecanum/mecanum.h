#ifndef MECANUM_H
#define MECANUM_H

#include <motor.h>

class Mecanum
{
public :

    enum class Dir
    {
        N,
        P,
        Z
    };

private :

    struct
    {
        Motor motor;
        Dir x, y;
        // TODO: use analogGetChannel & ledcRead to retreive the current speed
        float speed;
        float update;
    } wheels[4];

    bool moving;

    void update() noexcept;

public :

    Mecanum(Motor && fl, Motor && fr, Motor && rl, Motor && rr) noexcept;

    Mecanum(Mecanum const &) = delete;

    Mecanum(Mecanum &&) noexcept = default;
    
    ~Mecanum() noexcept;

    Mecanum & operator = (Mecanum const &) = delete;

    Mecanum & operator = (Mecanum &&) noexcept = default;

    void move(Dir x, Dir y, float speed = 1.0f) noexcept;

    void stop() noexcept;

    void brake(float force = 1.0f) noexcept;

    void rotate(float speed) noexcept;

    bool is_moving() const noexcept;
};

#endif