#ifndef OSCILLATOR_VOICE_H
#define OSCILLATOR_VOICE_H

#include "Voice.h"
#include <stdint.h>

enum WaveformType {
    SINE,
    SQUARE,
    TRIANGLE,
    SAWTOOTH,
    NOISE
};

class OscillatorVoice : public Voice {
public:
    OscillatorVoice(WaveformType type = SINE);

    void noteOn(uint8_t note, uint8_t velocity) override;
    void noteOff() override;
    bool isActive() override;
    void setSampleRate(uint32_t sr) override;
    void resetPhase() override;
    float readSample() override;

    void setWaveform(WaveformType type);

private:
    WaveformType waveform;

    float frequency = 440.0f;
    float velocity = 1.0f;
    float phase = 0.0f;
    float cycles = 0.0f;
    float deltaTime = 1.0f / 44100.0f;
    uint32_t sampleRate = 44100;
    bool active = false;

    float attackTime = 0.05f;
    float attackProgress = 0.0f;
    float releaseTime = 0.05f;      // 50ms
    float releaseProgress = 0.0f;
    bool releasing = false;
};

#endif // OSCILLATOR_VOICE_H
