// #define DEBUG_NORMAL_MODE

// ----- buffers and writing -----
#define I2S_BUFFER_NUM 24
#define I2S_BUFFER_SIZE 4096 // samples, not bytes
// Start the transmission if data reached threshold in buffer
#define AUDIO_TRANSMIT_THRESHOLD 65536 // Bytes, not samples. 
#define SD_FLUSH_SEGMENT 2000 // ms

// serial number in file name *.wav
#define MAX_FILE_SERIAL 65534

// I2C
#define I2C_CLK 100000 // 100kHz

// RTC
#define RTC_ADDR 0x51
#define TIMEZONE_OFFSET 8 // UTC +8, or CST

// battery
#define LOW_BATTERY_VOLTAGE 3.4
#define FULL_BATTERY_VOLTAGE 4.2
#define LOW_BATTERY_WARN_THRESHOLD 3.7
#define BATTERY_VOLTAGE_TEST_SEGMENT 60000 // 60s or 60000ms

// PCM186x
#define ADC_ADDR 0x4a
#define AUTO_CLIPPING_LIMIT_DEFAULT 0b00 // 00: -3dB, 01: -4dB, 10: -5dB, 11: -6dB
#define CLIPPING_EVENT_NUM_DEFAULT 0b11 // 10 clipping events
#define PEAK_SUPPRESS_THRESHOLD -1.0f // -1dBFS
#define SUPPRESS_DEPTH -3.0f // -3dB
#define SUPPRESS_FREEZE_SEGMENT 3 // do not suppress again during 3 buffer r/w in the future
#define GAIN_LEVEL_1 0  //  0dB
#define GAIN_LEVEL_2 20 // 10dB
#define GAIN_LEVEL_3 40 // 20dB
#define GAIN_LEVEL_4 60 // 30dB

// AGC
#define RMS_0DBFS_Q15 23170 // RMS of sine wave, or 0.707, from AES17 standard

// RGB
#define PWM_FREQ 1000 // 1kHz
#define PWM_RESOLUTION 8 // 8-bit to meet hex color format

// misc
#define BOOTSEL_CHECK_FREQ 100 // 100Hz
#define BTN_NO_RESPONSE_TIME 500 // 500ms