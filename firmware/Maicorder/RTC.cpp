#include "RTC.h"

void RTC::init() {
    I2C::write(RTC_ADDR, 0x00, 0b00000000);
    I2C::write(RTC_ADDR, 0x01, 0b00000000);
}

void RTC::setTime(time_t timeStamp) {
    uint16_t year;
    uint8_t month, day, hour, minute, second, wday;
    this->unixTimeStamp2YMDHMS(timeStamp, year, month, day, wday, hour, minute, second);
    I2C::write(RTC_ADDR, 0x02, this->hex2BCD(second));
    I2C::write(RTC_ADDR, 0x03, this->hex2BCD(minute));
    I2C::write(RTC_ADDR, 0x04, this->hex2BCD(hour));
    I2C::write(RTC_ADDR, 0x05, this->hex2BCD(day));
    I2C::write(RTC_ADDR, 0x06, this->hex2BCD(wday));
    I2C::write(RTC_ADDR, 0x07, this->hex2BCD(month));
    I2C::write(RTC_ADDR, 0x08, this->hex2BCD(year - 2000));
}

time_t RTC::getTime() {
    uint8_t second = this->BCD2hex(I2C::read(RTC_ADDR, 0x02)&0x7F);
    uint8_t minute = this->BCD2hex(I2C::read(RTC_ADDR, 0x03)&0x7F);
    uint8_t hour = this->BCD2hex(I2C::read(RTC_ADDR, 0x04)&0x3F);
    uint8_t day = this->BCD2hex(I2C::read(RTC_ADDR, 0x05)&0x3F);
    uint8_t month = this->BCD2hex(I2C::read(RTC_ADDR, 0x07)&0x1F);
    uint16_t year = this->BCD2hex(I2C::read(RTC_ADDR, 0x08)) + 2000;
    Serial.printf("RTC Read: %d-%.2d-%.2d %.2d:%.2d:%.2d UTC\n", year, month, day, hour, minute, second);
    return this->YMDHMS2UnixTimeStamp(year, month, day, hour, minute, second);
}

void RTC::updateSysTime(time_t timeStamp, int timezoneOffset) {
    struct tm timeinfo;
    gmtime_r(&timeStamp, &timeinfo);
    // timeStamp is in UTC, need to be converted to local time
    timeinfo.tm_hour += timezoneOffset;
    aon_timer_set_time_calendar(&timeinfo);
}


uint8_t RTC::BCD2hex(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

uint8_t RTC::hex2BCD(uint8_t hex) {
    return ((hex / 10) << 4) | (hex % 10);
}

time_t RTC::YMDHMS2UnixTimeStamp(
    uint16_t year,
    uint8_t month,
    uint8_t day,
    uint8_t hour,
    uint8_t minute,
    uint8_t second
) {
    struct tm timeinfo;
    timeinfo.tm_year = year - 1900; // tm_year is years since 1900
    timeinfo.tm_mon = month - 1; // tm_mon is 0-11
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    return mktime(&timeinfo);
}

void RTC::unixTimeStamp2YMDHMS(
    time_t timeStamp,
    uint16_t &year,
    uint8_t &month,
    uint8_t &day,
    uint8_t &wday,
    uint8_t &hour,
    uint8_t &minute,
    uint8_t &second
) {
    struct tm timeinfo;
    gmtime_r(&timeStamp, &timeinfo);
    year = timeinfo.tm_year + 1900; // tm_year is years since 1900
    month = timeinfo.tm_mon + 1; // tm_mon is 0-11
    day = timeinfo.tm_mday;
    wday = timeinfo.tm_wday; // tm_wday is 0-6, Sunday = 0
    hour = timeinfo.tm_hour;
    minute = timeinfo.tm_min;
    second = timeinfo.tm_sec;
}