#include "I2C.h"
#include "config.h"


#include "pico/aon_timer.h"
#include <Arduino.h>


// Notice: All time functions and time in the RTC are based on UTC time
// since UNIX timestamp is based on it.
// However, SDFS will use the system clock as localtime, so we need to convert it
// before write RTC time to system clock.
class RTC {
    public:
        void init();
        void setTime(time_t timeStamp);
        time_t getTime();
        void updateSysTime(time_t timeStamp, int timezoneOffset);
    private:
        uint8_t BCD2hex(uint8_t bcd);
        uint8_t hex2BCD(uint8_t hex);
        time_t YMDHMS2UnixTimeStamp(
            uint16_t year,
            uint8_t month,
            uint8_t day,
            uint8_t hour,
            uint8_t minute,
            uint8_t second
        );
        void unixTimeStamp2YMDHMS(
            time_t timeStamp,
            uint16_t &year,
            uint8_t &month,
            uint8_t &day,
            uint8_t &wday,
            uint8_t &hour,
            uint8_t &minute,
            uint8_t &second
        );
};