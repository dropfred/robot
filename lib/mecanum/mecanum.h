#ifndef MECANUM_H
#define MECANUM_H

#include <motor.h>

#include <array>

// #define MECANUM_AUTO_TEST

class Mecanum
{
public :

    enum class Dir {N, P, Z};

private :

    struct
    {
        Motor motor;
        Dir x, y;
    } wheels[4];

public :

    Mecanum(Motor && fl, Motor && fr, Motor && rl, Motor && rr);

    Mecanum(Mecanum const &) = delete;

    Mecanum(Mecanum &&) = default;
    
    ~Mecanum();

    Mecanum & operator = (Mecanum const &) = delete;

    Mecanum & operator = (Mecanum &&) = default;

    void move(Dir x, Dir y, float speed);

    void stop();

    void brake(float force = 1.0f);

    void rotate(float speed);

#ifdef MECANUM_AUTO_TEST
    void test();
#endif
};

#endif