#ifndef XBLUE_H
#define XBLUE_H

#if XBLUE_INTERFACE_CFG == XBLUE_INTERFACE_MICRO
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

    using ButtonCb = std::function<void (bool)>;
    using SharedButtonCb = std::function<void (std::string const &, bool)>;

    using ToggleCb = std::function<void (bool)>;
    using SharedToggleCb = std::function<void (std::string const &, bool)>;

    using SliderCb = std::function<void (float)>;
    using SharedSliderCb = std::function<void (std::string const &, float)>;

    using PadCb = std::function<void (float, float)>;
    using SharedPadCb = std::function<void (std::string const &, float, float)>;

    using TextCb = std::function<void (std::string const &)>;
    using SharedTextCb = std::function<void (std::string const &, std::string const &)>;

    void on_button(std::string const & name, ButtonCb cb);
    void on_button(std::string const & name, SharedButtonCb cb);

    void on_toggle(std::string const & name, ToggleCb cb);
    void on_toggle(std::string const & name, SharedToggleCb cb);

    void on_slider(std::string const & name, SliderCb cb);
    void on_slider(std::string const & name, SharedSliderCb cb);

    void on_pad(std::string const & name, PadCb cb);
    void on_pad(std::string const & name, SharedPadCb cb);

    void on_text(std::string const & name, TextCb cb);
    void on_text(std::string const & name, SharedTextCb cb);
}
#elif XBLUE_INTERFACE_CFG == XBLUE_INTERFACE_ARDUINO
#include <string>
#include <functional>
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
#else
#error undefined xblue interface
#endif

#endif
