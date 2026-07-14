// ----- buffers and writing -----
#define I2S_BUFFER_NUM 24
#define I2S_BUFFER_SIZE 4096 // samples, not bytes
// Start the transmission if data reached threshold in buffer
#define AUDIO_TRANSMIT_THRESHOLD 65536 // Bytes, not samples. 
#define SD_FLUSH_SEGMENT 2000 // ms

// serial number in file name *.wav
#define MAX_FILE_SERIAL 65534

// I2C
#define I2C_CLK 100000

// PCM186x
#define AUDIO_I2C_ADDR 0x4a
#define PGA_GAIN_DEFAULT 0b01000000 // 32dB, -12~40dB, 0.5dB/step
#define AUTO_CLIPPING_LIMIT_DEFAULT 0b00 // 00: -3dB, 01: -4dB, 10: -5dB, 11: -6dB
#define CLIPPING_EVENT_NUM_DEFAULT 0b11 // 10 clipping events

// AGC
#define RMS_0DBFS_Q15 23170 // RMS of sine wave, or 0.707, from AES17 standard
#define TARGET_AVG_LOUDNESS_DBFS -12.0f // -16dBFS
#define AGC_GAIN_MAX 40
#define AGC_GAIN_MIN -12

// RGB
#define PWM_FREQ 1000 // 1kHz
#define PWM_RESOLUTION 8 // 8-bit to meet hex color format
