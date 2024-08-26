#ifndef HM10_H
#define HM10_H

#include <string>
#include <functional>

#include <cstdint>
#include <cstddef>

#define HM10_BLE_NATIVE 1
#define HM10_BLE_SERIAL 2

#ifndef HM10_BLE_CFG
#define HM10_BLE_CFG HM10_BLE_NATIVE
#endif

#if HM10_BLE_CFG == HM10_BLE_NATIVE
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
#elif HM10_BLE_CFG == HM10_BLE_SERIAL
namespace HM10
{
    using WriteCb = std::function<void (uint8_t const *, size_t)>;
    
    bool start(std::string const & name, uint8_t rx, uint8_t tx, WriteCb cb = nullptr);

    void stop();

    void update();

    void onWrite(WriteCb cb);
}
#else
#error undefined ble device
#endif

#endif
