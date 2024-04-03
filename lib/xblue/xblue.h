#ifndef XBLUE_H
#define XBLUE_H

#if defined(XBLUE_ARDUINO)

#include <hm10.h>

#include <string>
#include <vector>
#include <cstdint>

namespace XBlue
{
    using ButtonCb = std::function<void (uint8_t)>;
    using SliderCb = std::function<void (uint8_t, float)>;
    using DriveCb = std::function<void (float, float)>;
    using TextCb = std::function<void (std::string const &)>;
    using PathCb = std::function<void (std::vector<float> const &)>;
    
    bool start(std::string const & name);

    void stop();

    void on_button(ButtonCb cb);
    void on_slider(SliderCb cb);
    void on_drive(DriveCb cb);
    void on_text(TextCb cb);
    void on_path(PathCb cb);
}

#elif defined (XBLUE_MICRO)

#include <string>
#include <functional>

namespace XBlue
{
#ifdef HM10_SERIAL
    bool start(std::string const & name, uint8_t rx, uint8_t tx);

    void update();
#else
    bool start(std::string const & name);
#endif

    void stop();

    void on_button(std::string const & name, std::function<void (bool)> cb);
    void on_button(std::string const & name, std::function<void (std::string const &, bool)> cb);

    void on_toggle(std::string const & name, std::function<void (bool)> cb);
    void on_toggle(std::string const & name, std::function<void (std::string const &, bool)> cb);

    void on_slider(std::string const & name, std::function<void (float)> cb);
    void on_slider(std::string const & name, std::function<void (std::string const &, float)> cb);

    void on_pad(std::string const & name, std::function<void (float, float)> cb);
    void on_pad(std::string const & name, std::function<void (std::string const &, float, float)> cb);

    void on_text(std::string const & name, std::function<void (std::string const &)> cb);
    void on_text(std::string const & name, std::function<void (std::string const &, std::string const &)> cb);
}

#else
#error Targeted application not defined
#endif

#endif
