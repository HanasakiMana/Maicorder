// ----- buffers and writing -----
#define I2S_BUFFER_NUM 12
#define I2S_BUFFER_SIZE 4096 // samples, not bytes
// Start the transmission if data reached threshold in buffer
#define AUDIO_TRANSMIT_THRESHOLD 32768 // Bytes, not samples. 
#define SD_FLUSH_SEGMENT 2000 // ms


#define AUDIO_I2C_ADDR 0x18

// serial number in file name *.wav
#define MAX_FILE_SERIAL 65534

// ----- ADC control -----
// gain
#define AGC_TARGET 0b001 // -8dBFS
#define AGC_GAIN_HYSTERESIS 0b01 // -0.5dB～0.5dB
#define AGC_MAX_GAIN 0b0101000 // 30dB, 0.5dB/step
// noise
// #define AGC_NOISE_THRESHOLD 0b10111 // -(30+2*0b10111) = -76dB
// #define AGC_NOISE_HYSTERESIS 0b01 // 2dB
// ADSR
#define AGC_ATK 0b10000 // 31*32 ADC WCLK
#define AGC_ATK_SCALE 0b001 // x2, 41ms
#define AGC_DECAY 0b11000 // 47*512 ADC WCLK
#define AGC_DECAY_SCALE 0b001 // x2, 1000ms

// ----- output control -----
// routing
#define HP_OUTPUT 0b00000010 // Mixer -> Headphone
// #define HP_OUTPUT 0b00000100 // Analog Loopback -> Headphone


// gain
#define HP_GAIN 0b010000 // +16dB (111010-011101, aka -6dB~29dB)
#define HP_ANALOG_GAIN 0b00000000 // 0~-72.3dB
#define LO_GAIN 0b00000 // 0dB
#define MICPGA_GAIN 0b0001100 // 6dB, 0.5dB/step
// Register of 6dB with input impedance of 20K => Channel Gain of 0dB
