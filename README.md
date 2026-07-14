# Maicorder

An audio recorder designed for arcade games, with 2.1Vrms maximum FS input and dedicated headphone amplifier.


## Hardware Design

This device is based on [RP2350](https://www.raspberrypi.com/products/rp2350/), which has 520KB of SRAM, and the whole stack is around 435KB, giving us ample memory for audio buffer.

Though RP2350 does not have various hardware-based peripheral controllers like I2S or SDIO, we can make it possible by using PIOs without complex configurations, thanks to [Arduino-Pico](https://arduino-pico.readthedocs.io/en/latest/) project. 


### PCM186x

The PCM186x family of audio ADCs can accept 2.1 Vrms Full-Scale single-ended input under 3.3V AVcc without dividers, which is great for built-in headphone amplifiers of arcade games. 

In this family, PCM1860 and PCM1861 can only be controlled by changing levels of several pins with fixed PGA gains, which is definitely insufficient for us, so we have to use other chips with advanced control method (PCM1862~PCM1865).

**The best choice for this application is [PCM1862](https://www.ti.com/product/PCM1862) or [PCM1863](https://www.ti.com/product/PCM1863).** Both of them can be controlled externally using I2C or SPI, but PCM1863 has a slightly higher SNR performance under differential input (103dB for PCM1862 and 110dB for PCM1863, A weighted), which is not really important for us. You can indeed use PCM1864 or PCM1865, which can record 2 channels simultaneously, but we only need 1 channel for single-ended stereo audio.

PCM186x uses I2S to transfer audio data, but it can only accept at least 128x fs MCLK (6.144MHz under 48kHz sample rate) from master device. However, RP2350 can only provide MCLK at 64x fs, so we have to use **ADC slave PLL mode**. Under this mode, PCM186x will generate an internal MCLK for whole system using PLL. By setting BCK(64x fs in this case) as PLL reference clock and configuring some coefficients, we can generate an 98.304MHz (2048x fs) internal clock and divide it to 12.288MHz&6.144MHz for DSPs and ADCs.


### [MAX4410](https://www.analog.com/en/products/max4410.html)

The major reason why I choose this amplifier is the voltage of power supply. Most of arcade games have a dedicated headphone amplifier between system board and headphone jack, and this amp is usually powered by 5V. To provide an alternating current for audio, the amplifier need a common mode(CM), which is usually set to slightly lower than half of the power supply. Under 5V, the amplifier can easilly provide audio signal with 2.1V peak(1.5 Vrms) under high output impedance, or even higher. 

However, our device is powered by Li-ion battery, and power rail is designed to 3.3V, which means a regular amplifier can only provide 1.35V peak(0.95Vrms) ideally. This restriction won't be a problem if you are using a headphone with 32Ω or 16Ω impedance, but it will significant reduce loudness when high impedance headphones are plugged like 200Ω or even 600Ω, due to the voltage limitation.

Thankfully, amplifiers like MAX4410 can provide a negative voltage by using inverting charge pump, expanding output voltage directaly to 3.3V(2.3Vrms), which is definitely sufficient for this device. Other amps like [MAX97220](https://www.analog.com/en/products/max97220.html) can provide even higher output voltage and better SNR performance, but I still choose MAX4410, since it is much cheeper and easier to solder.