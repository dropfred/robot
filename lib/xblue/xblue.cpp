#include <xblue.h>
#include <hm10.h>

#include <esp_log.h>

#if XBLUE_INTERFACE_CFG == XBLUE_INTERFACE_ARDUINO
#include <ArduinoBlue.h>

#include <cstring>
#include <algorithm>

namespace
{
    char const * TAG = "AB";

    struct
    {
        XBlue::ButtonCb button;
        XBlue::SliderCb slider;
        XBlue::DriveCb drive;
        XBlue::TextCb text;
        XBlue::PathCb path;
    } callbacks {};

// Broken on iOS, data is received one byte at a time.
#if 0
    uint8_t type = TRANSMISSION_END;

    std::string text {};
    struct
    {
       size_t length;
       std::vector<float> points;
    } path {};
    
    void hm10_write_cb(uint8_t const * data, size_t size)
    {
        ESP_LOGD(TAG, "hm10_write_cb : %d (%zu)", int(data[0]), size);

        if (type == TRANSMISSION_END)
        {
            // assert size > 0
            type = data[0];
            if (type == DRIVE_TRANSMISSION)
            {
                // assert size == 4
                if (callbacks.drive)
                {
                    float d1 = data[1];
                    float d2 = data[2];
                    if (d1 > 98.0f) d1 = 98.0f;
                    if (d2 > 98.0f) d2 = 98.0f;
                    d1 = (d1 - 49.0f) / 49.0f;
                    d2 = (d2 - 49.0f) / 49.0f;
                    callbacks.drive(d1, d2);
                }
                // assert data[3] == TRANSMISSION_END
                type = TRANSMISSION_END;
            }
            else if (type == BUTTON_TRANSMISSION)
            {
                // assert size == 3
                if (callbacks.button)
                {
                    callbacks.button(data[1]);
                }
                // assert data[2] == TRANSMISSION_END
                type = TRANSMISSION_END;
            }
            else if (type == SLIDER_TRANSMISSION)
            {
                // assert size == 4
                if (callbacks.slider)
                {
                    callbacks.slider(data[1], data[2] / 200.0f);
                }
                // assert data[3] == TRANSMISSION_END
                type = TRANSMISSION_END;
            }
            else if (type == TEXT_TRANSMISSION)
            {
                // assert size == 1
            }
            else if (type == PATH_TRANSMISSION)
            {
                // assert size == 1
            }
            else if (type == CONNECTION_CHECK)
            {
                // assert size == 1
                HM10::send(char(CONNECTION_CHECK));
                type = TRANSMISSION_END;
            }
        }
        else if (type == TEXT_TRANSMISSION)
        {
            uint8_t d = data[0];
            if (d == TRANSMISSION_END)
            {
                type = TRANSMISSION_END;
                if (callbacks.text)
                {
                    callbacks.text(text);
                }
                text.clear();
            }
            else
            {
                text += char(d);
            }
        }
        else if (type == PATH_TRANSMISSION)
        {
            // assert size == 4
            float p; std::memcpy(&p, data, 4);

            if (path.length == 0)
            {
                path.length = static_cast<size_t>(p) * 2;
                path.points.reserve(path.length);
            }
            else
            {
                path.points.push_back(p);
                if (path.points.size() == path.length)
                {
                    type = TRANSMISSION_END;
                    HM10::send(uint8_t(PATH_TRANSMISSION_CONFIRMATION));
                    if (callbacks.path)
                    {
                        callbacks.path(path.points);
                    }
                    path.length = 0;
                    path.points.clear();
                }
            }
        }
    }
#endif
    #define TIMEOUT 250

    std::vector<uint8_t> buffer;
    unsigned long int time;

    void hm10_write_cb(uint8_t const * data, size_t size)
    {
        ESP_LOGD(TAG, "hm10_write_cb : %d (%zu)", int(data[0]), size);
        {
            unsigned long int t = millis();
            if ((t - time) > TIMEOUT)
            {
                buffer.clear();
            }
            time = t;
        }
        std::copy(data, data + size, std::back_inserter(buffer));

        if ((buffer.size() >= 4) && (buffer[0] == DRIVE_TRANSMISSION))
        {
            if (callbacks.drive)
            {
                float d1 = buffer[1];
                float d2 = buffer[2];
                if (d1 > 98.0f) d1 = 98.0f;
                if (d2 > 98.0f) d2 = 98.0f;
                d1 = (d1 - 49.0f) / 49.0f;
                d2 = (d2 - 49.0f) / 49.0f;
                callbacks.drive(d1, d2);
            }
            // assert buffer[3] == TRANSMISSION_END
            buffer.erase(buffer.begin(), buffer.begin() + 4);
        }
        else if ((buffer.size() >= 3) && (buffer[0] == BUTTON_TRANSMISSION))
        {
            if (callbacks.button)
            {
                callbacks.button(buffer[1]);
            }
            // assert buffer[2] == TRANSMISSION_END
            buffer.erase(buffer.begin(), buffer.begin() + 3);
        }
        else if ((buffer.size() >= 4) && (buffer[0] == SLIDER_TRANSMISSION))
        {
            if (callbacks.slider)
            {
                callbacks.slider(buffer[1], buffer[2] / 200.0f);
            }
            // assert buffer[3] == TRANSMISSION_END
            buffer.erase(buffer.begin(), buffer.begin() + 4);
        }
        else if ((buffer.size() >= 1) && (buffer[0] == TEXT_TRANSMISSION))
        {
            // TODO
        }
        else if ((buffer.size() >= 1) && (buffer[0] == PATH_TRANSMISSION))
        {
            // TODO
        }
    }
}

namespace XBlue
{
    bool start(std::string const & name)
    {
        return HM10::start(name, hm10_write_cb);
    }
    
    void stop()
    {
        HM10::stop();
    }

    void on_button(ButtonCb cb)
    {
        callbacks.button = cb;
    }

    void on_slider(SliderCb cb)
    {
        callbacks.slider = cb;
    }

    void on_drive(DriveCb cb)
    {
        callbacks.drive = cb;
    }

    void on_text(TextCb cb)
    {
        callbacks.text = cb;
    }

    void on_path(PathCb cb)
    {
        callbacks.path = cb;
    }
}
#elif XBLUE_INTERFACE_CFG == XBLUE_INTERFACE_MICRO
#include <vector>
#include <algorithm>

namespace
{
    template <class C>
    struct Control
    {
        using Cb = C;
        std::string name;
        Cb cb;
    };

    using Button = Control<std::function<void (std::string const &, bool)>>;
    using Toggle = Control<std::function<void (std::string const &, bool)>>;
    using Slider = Control<std::function<void (std::string const &, float)>>;
    using Pad    = Control<std::function<void (std::string const &, float, float)>>;
    using Text   = Control<std::function<void (std::string const &, std::string const &)>>;

    std::vector<Button> buttons;
    std::vector<Toggle> toggles;
    std::vector<Slider> sliders;
    std::vector<Pad>    pads;
    std::vector<Text>   textes;

    template <class C>
    typename C::Cb get_control(std::vector<C> const & cs, std::string const & name)
    {
        for (auto const & c : cs)
        {
            if (c.name == name) return c.cb;
        }
        return {};
    }

    template <class C>
    void on_control(std::vector<C> & cs, std::string const & name, typename C::Cb cb)
    {
        auto c = std::find_if(cs.begin(), cs.end(), [& name] (C const & c) {return c.name == name;});
        if (c != cs.end())
        {
            if (cb)
            {
                c->cb = cb;
            }
            else
            {
                cs.erase(c);
            }
        }
        else if (cb)
        {
            cs.push_back({name, cb});
        }
    }

    void hm10_callback(uint8_t const * data, size_t size)
    {
        char const * d  = (char const *)(data);
        char const * de = d + size;
        std::string name {};
        std::string value {};

        if ((d == de) || (*d++ != 1))
        {
            // log invalid event
            return;
        }
        while ((d != de) && (*d != 2))
        {
            name += *d++;
        }
        if (d == de)
        {
            // log invalid event
            return;
        }
        ++d;
        while ((d != de) && (*d != 3))
        {
            value += *d++;
        }
        if (d == de)
        {
            // log invalid event
            return;
        }

        if (auto cb = get_control(pads, name); cb)
        {
            auto c = value.find_first_of(',');
            if (c == std::string::npos)
            {
                // log invalid event
                return;
            }
            auto sx = value.substr(0, c);
            auto sy = value.substr(c + 1);
            if (sx.empty() || sy.empty())
            {
                // log invalid event
                return;
            }
            // -512 <= xy < 512
            int x = std::stoi(sx) - 512;
            int y = std::stoi(sy) - 512;
#ifdef XBLUE_NORMALIZE_POSITIVE_PAD
            cb(name, x / ((x > 0) ? 511.0f : 512.0f), y / ((y > 0) ? 511.0f : 512.0f));
#else
            cb(name, x / 512.0f, y / 512.0f);
#endif            
        }
        else if (auto cb = get_control(buttons, name); cb)
        {
            cb(name, value == "1");
        }
        else if (auto cb = get_control(sliders, name); cb)
        {
            cb(name, std::stoul(value) / 100.0f);
        }
        else if (auto cb = get_control(toggles, name); cb)
        {   
            cb(name, value == "1");
        }
        else if (auto cb = get_control(textes, name); cb)
        {   
            cb(name, value);
        }
    }
}

namespace XBlue
{
#if HM10_BLE_CFG == HM10_BLE_NATIVE
    bool start(std::string const & name)
    {
        return HM10::start(name, hm10_callback);
    }
#elif HM10_BLE_CFG == HM10_BLE_SERIAL
    bool start(std::string const & name, uint8_t rx, uint8_t tx)
    {
        return HM10::start(name, rx, tx, hm10_callback);
    }

    void update()
    {
        HM10::update();
    }
#endif
    void stop()
    {
        HM10::stop();
    }

    void on_pad(std::string const & name, std::function<void (std::string const &, float, float)> cb)
    {
        on_control(pads, name, cb);
    }

    void on_pad(std::string const & name, std::function<void (float, float)> cb)
    {
        on_pad(name, [cb] (std::string const & n, float x, float y) {cb(x, y);});
    }

    void on_button(std::string const & name, std::function<void (std::string const &, bool)> cb)
    {
        on_control(buttons, name, cb);
    }

    void on_button(std::string const & name, std::function<void (bool)> cb)
    {
        on_button(name, [cb] (std::string const & n, bool v) {cb(v);});
    }

    void on_slider(std::string const & name, std::function<void (std::string const &, float)> cb)
    {
        on_control(sliders, name, cb);
    }

    void on_slider(std::string const & name, std::function<void (float)> cb)
    {
        on_slider(name, [cb] (std::string const & n, float v) {cb(v);});
    }

    void on_toggle(std::string const & name, std::function<void (std::string const &, bool)> cb)
    {
        on_control(toggles, name, cb);
    }

    void on_toggle(std::string const & name, std::function<void (bool)> cb)
    {
        on_toggle(name, [cb] (std::string const & n, bool v) {cb(v);});
    }

    void on_text(std::string const & name, std::function<void (std::string const &, std::string const &)> cb)
    {
        on_control(textes, name, cb);
    }

    void on_text(std::string const & name, std::function<void (std::string const &)> cb)
    {
        on_text(name, [cb] (std::string const & n, std::string const & msg) {cb(msg);});
    }
}
#endif