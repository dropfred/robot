#ifndef HM10_H
#define HM10_H

#include <string>
#include <functional>

#include <cstdint>
#include <cstddef>

#ifdef HM10_SERIAL
namespace HM10
{
    using WriteCb = std::function<void (uint8_t const *, size_t)>;
    
    bool start(std::string const & name, uint8_t rx, uint8_t tx, WriteCb cb = nullptr);

    void stop();

    void update();

    void onWrite(WriteCb cb);
}
#else
namespace HM10
{
    using WriteCb = std::function<void (uint8_t const *, size_t)>;
    
    bool start(std::string const & name, WriteCb cb = nullptr);

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
#endif

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