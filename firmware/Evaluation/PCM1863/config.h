// ----- buffers and writing -----
#define I2S_BUFFER_NUM 12
#define I2S_BUFFER_SIZE 4096 // samples, not bytes
// Start the transmission if data reached threshold in buffer
#define AUDIO_TRANSMIT_THRESHOLD 32768 // Bytes, not samples. 
#define SD_FLUSH_SEGMENT 2000 // ms

// serial number in file name *.wav
#define MAX_FILE_SERIAL 65534

// PCM186x
#define AUDIO_I2C_ADDR 0x4a
#define AUDIO_I2C_CLK 10000 // 100kHz
#define PGA_GAIN_DEFAULT 0b01001000 // 36dB
#define AUTO_CLIPPING_LIMIT_DEFAULT 0b00 // -3dB
#define CLIPPING_EVENT_NUM_DEFAULT 0b11 // 10 clipping events

// RGB
#define PWM_FREQ 1000 // 1kHz
#define PWM_RESOLUTION 8 // 8-bit to meet hex color format
