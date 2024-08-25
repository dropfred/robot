#include <hm10.h>

#if HM10_BLE_CFG == HM10_BLE_NATIVE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <esp_log.h>

// setValue takes non const data (wtf?)
#ifndef HM10_COPY_DATA
#define HM10_COPY_DATA 1
#endif
#if HM10_COPY_DATA
#define HM10_BUFFER_SIZE 20
#include <array>
#include <algorithm>
#include <cstring>
#endif

namespace
{
    char const * TAG = "HM10";

    BLEUUID const HM10_SERVICE        {"0000FFE0-0000-1000-8000-00805F9B34FB"};
    BLEUUID const HM10_CHARACTERISTIC {"0000FFE1-0000-1000-8000-00805F9B34FB"};

    HM10::WriteCb write_cb {};

    struct : public BLEServerCallbacks
    {
        virtual void onConnect(BLEServer * server) override
        {
            ESP_LOGD(TAG, "onConnect");
        }

        virtual void onConnect(BLEServer * server, esp_ble_gatts_cb_param_t * param) override
        {
            ESP_LOGD(TAG, "onConnect (param)");
        }

        virtual void onDisconnect(BLEServer * server) override
        {
            ESP_LOGD(TAG, "onDisconnect");
        }

        virtual void onDisconnect(BLEServer * server, esp_ble_gatts_cb_param_t * param) override
        {
            ESP_LOGD(TAG, "onDisconnect (param)");
        }

        virtual void onMtuChanged(BLEServer * server, esp_ble_gatts_cb_param_t * param) override
        {
            ESP_LOGD(TAG, "onMtuChanged");
        }

    } server_callback;

    struct : public BLECharacteristicCallbacks
    {
        virtual void onRead(BLECharacteristic * c) override
        {
            ESP_LOGD(TAG, "onRead");
        };
        
        virtual void onWrite(BLECharacteristic * c) override
        {
            uint8_t const * data = c->getData();
            size_t size = c->getLength();
            ESP_LOGD(TAG, "onWrite : %zu", size);
            if (write_cb)
            {
                write_cb(data, size);
            }
        };

        virtual void onNotify(BLECharacteristic * c) override
        {
            ESP_LOGD(TAG, "onNotify");
        }

        virtual void onStatus(BLECharacteristic * c, Status s, uint32_t code) override
        {
            ESP_LOGD(TAG, "onStatus");
        }
    } characteristic_callback {};

    BLE2902 descriptor {};
    
    BLEServer * server = nullptr;
    BLEService * service = nullptr;
    BLECharacteristic * characteristic = nullptr;
    BLEAdvertising * advertising = nullptr;
}

bool HM10::start(std::string const & name, WriteCb cb)
{
    bool ok = (server == nullptr);
    if (ok)
    {
        ESP_LOGD(TAG, "BLE start");

        write_cb = cb;

        BLEDevice::init(name);

        server = BLEDevice::createServer();
        server->setCallbacks(&server_callback);

        service = server->createService(HM10_SERVICE);

        descriptor.setNotifications(true);

        characteristic = service->createCharacteristic
        (
            HM10_CHARACTERISTIC,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE_NR |
            BLECharacteristic::PROPERTY_NOTIFY
        );
        characteristic->setCallbacks(&characteristic_callback);
        characteristic->addDescriptor(&descriptor);

        service->start();

        advertising = server->getAdvertising();
        advertising->addServiceUUID(service->getUUID());
        advertising->start();

        ESP_LOGD(TAG, "BLE started");
    }
    return ok;
}

void HM10::stop()
{
    // TODO
}

bool HM10::send(uint8_t const * data, size_t size)
{
    bool ok = server != nullptr;
    if (ok)
    {
        #if HM10_COPY_DATA
        std::array<uint8_t, HM10_BUFFER_SIZE> buffer {};
        while (size > 0)
        {
            size_t s = std::min(size, buffer.size());
            std::memcpy(buffer.data(), data, s);
            characteristic->setValue(buffer.data(), s);
            data += s;
            size -= s;
        }
        #else
        // YOLO, LET'S GO UB
        characteristic->setValue(const_cast<uint8_t *>(data), size);
        #endif
        characteristic->notify(true);
    }
    return ok;
}

bool HM10::send(std::string const & data)
{
    return send(reinterpret_cast<uint8_t const * >(data.c_str()), data.size());
}

void HM10::onWrite(WriteCb cb)
{
    write_cb = cb;
}
#elif HM10_BLE_CFG == HM10_BLE_SERIAL
#include <Arduino.h>

#include <array>

#ifndef HM10_SERIAL_STREAM
#define HM10_SERIAL_STREAM Serial1
#endif

#ifndef HM10_SERIAL_TIMEOUT
#define HM10_SERIAL_TIMEOUT 10
#endif

#define HM10_BUFFER_SIZE 20

namespace
{
    struct
    {
        std::string name;
        uint8_t rx, tx;
    } hm10 {};
    
    void connect()
    {
        pinMode(hm10.rx, INPUT);
        pinMode(hm10.tx, OUTPUT);
        HM10_SERIAL_STREAM.begin(9600, SERIAL_8N1, hm10.rx, hm10.tx);
        delay(500);
        HM10_SERIAL_STREAM.println(("AT+NAME" + hm10.name).c_str());
        delay(500);
    }
}

HM10::WriteCb write_cb {};

bool HM10::start(std::string const & name, uint8_t rx, uint8_t tx, WriteCb cb)
{
    hm10.name = name; hm10.rx = rx; hm10.tx = tx; write_cb = cb;
    connect();
    return HM10_SERIAL_STREAM;
}

void HM10::stop()
{
    HM10_SERIAL_STREAM.end();
}

void HM10::update()
{
    if (!HM10_SERIAL_STREAM)
    {
        connect();
    }

    if (HM10_SERIAL_STREAM)
    {
        std::array<uint8_t, HM10_BUFFER_SIZE> buffer {};
        size_t size = 0;
        unsigned long int t0 = millis(), t1;

        while ((size < buffer.size()) && (t1 = millis(), (t1 - t0) < HM10_SERIAL_TIMEOUT))
        {
            if (HM10_SERIAL_STREAM.available())
            {
                buffer[size++] = uint8_t(HM10_SERIAL_STREAM.read());
                t0 = t1;
            }
            else
            {
                delay(1);
            }
        }

        if (write_cb && (size > 0))
        {
            write_cb(buffer.data(), size);
        }
    }
}

void HM10::onWrite(WriteCb cb)
{
    write_cb = cb;
}
#endif
