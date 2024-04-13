#ifndef HM10_H
#define HM10_H

#include <string>
#include <functional>

#include <cstdint>
#include <cstddef>

namespace HM10
{
    using WriteCb = std::function<void (uint8_t const *, size_t)>;

#ifdef HM10_SERIAL
    bool start(std::string const & name, uint8_t rx, uint8_t tx, WriteCb cb = nullptr);

    void update();
#else
    bool start(std::string const & name, WriteCb cb = nullptr);
#endif

    void stop();

    bool send(uint8_t const * data, size_t size);

    bool send(std::string const & data);

    template <typename T>
    bool send(T data)
    {
        return send(reinterpret_cast<uint8_t const *>(&data), sizeof data);
    }

    void onWrite(WriteCb cb);
}

#if 0
class HM10Stream : public Stream
{
public:
    HM10Stream()
    {
    }

    HM10Stream(HM10Stream const &) = delete;
    HM10Stream operator = (HM10Stream const &) = delete;

    virtual size_t write(uint8_t) override
    {
        return 0;
    }

    virtual int available() override
    {
        return 0;
    }

    virtual int read() override
    {
        return 0;
    }

    virtual int peek() override
    {
        return 0;
    }
};
#endif

#endif