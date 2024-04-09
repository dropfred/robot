#ifndef SERVO_H
#define SERVO_H

#include <array>

// Values for TowerPro MG995 large servos (and many other hobbyist servos)
#define DEFAULT_uS_LOW 1000        // 1000us
#define DEFAULT_uS_HIGH 2000      // 2000us

// Values for TowerPro SG90 small servos
//#define DEFAULT_uS_LOW 400
//#define DEFAULT_uS_HIGH 2400

#define DEFAULT_TIMER_WIDTH 16
#define DEFAULT_TIMER_WIDTH_TICKS 65536

#define ESP32_Servo_VERSION           1     // software version of this library

#define MIN_PULSE_WIDTH       500     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2500     // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH  1500     // default pulse width when servo is attached
#define DEFAULT_PULSE_WIDTH_TICKS 4825
#define REFRESH_CPS            50
#define REFRESH_USEC         20000

#define MAX_SERVOS              16     // no. of PWM channels in ESP32

#ifndef SERVO_MAX
#define SERVO_MAX 16
#endif

class Servo
{
    static uint8_t const FREE  = 0xFF;
    static uint8_t const INVALID = 0xFF;
    static uint8_t const AUTO    = 0xFF;

    // struct helper
    // {
    //     template <class T, size_t N, T V, T... Vs>
    //     struct repeat
    //     {
    //         using type = typename repeat<T, N - 1, V, V, Vs...>::type;
    //     };

    //     template <class T, T V, T... Vs>
    //     struct repeat<T, 0, V, Vs...>
    //     {
    //         using type = std::integer_sequence<T, Vs...>;
    //     };

    //     template <class T>
    //     struct init_array;

    //     template <class T, T... Vs>
    //     struct init_array<std::integer_sequence<T, Vs...>>
    //     {
    //         static constexpr std::array<T, sizeof...(Vs)> value()
    //         {
    //             return std::array<T, sizeof...(Vs)>{{Vs...}};
    //         }
    //     };

    //     template <class T, size_t N, T V = 0UL>
    //     static  constexpr std::array<T, N> make_array()
    //     {
    //         return init_array<typename repeat<T, N, V>::type>::value();
    //     };
    // };

    // static std::array<uint8_t, SERVO_MAX> channels;

    uint8_t channel = INVALID;

public:
  Servo(uint8_t pin, uint8_t channel = AUTO) noexcept;
  // Arduino Servo Library calls
  int attach(int pin);                   // attach the given pin to the next free channel, returns channel number or 0 if failure
  int attach(int pin, int min, int max); // as above but also sets min and max values for writes. 
  void detach();
  void write(int value);                 // if value is < MIN_PULSE_WIDTH its treated as an angle, otherwise as pulse width in microseconds 
  void writeMicroseconds(int value);     // Write pulse width in microseconds 
  int read();                            // returns current pulse width as an angle between 0 and 180 degrees
  int readMicroseconds();                // returns current pulse width in microseconds for this servo
  bool attached();                       // return true if this servo is attached, otherwise false  
  
  // ESP32 only functions
  void setTimerWidth(int value);     // set the PWM timer width (ESP32 ONLY)
  int readTimerWidth();              // get the PWM timer width (ESP32 ONLY)  

  private: 
   int usToTicks(int usec);
   int ticksToUs(int ticks);
   static int ServoCount;                             // the total number of attached servos
   static int ChannelUsed[];                          // used to track whether a channel is in service
   int servoChannel = 0;                              // channel number for this servo
   int min = DEFAULT_uS_LOW;                          // minimum pulse width for this servo   
   int max = DEFAULT_uS_HIGH;                         // maximum pulse width for this servo 
   int pinNumber = 0;                                 // GPIO pin assigned to this channel
   int timer_width = DEFAULT_TIMER_WIDTH;             // ESP32 allows variable width PWM timers
   int ticks = DEFAULT_PULSE_WIDTH_TICKS;             // current pulse width on this channel
   int timer_width_ticks = DEFAULT_TIMER_WIDTH_TICKS; // no. of ticks at rollover; varies with width
};

#endif
