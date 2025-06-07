#ifndef OSCILLATOR_VOICE_LUT_H
#define OSCILLATOR_VOICE_LUT_H

#include "Voice.h"
#include "WaveformTable.h"

class OscillatorVoiceLUT : public Voice {
public:
    OscillatorVoiceLUT(WaveformType type);

    void noteOn(uint8_t note, uint8_t velocity) override;
    void noteOff() override;
    bool isActive() override;
    void setSampleRate(uint32_t sr) override;
    float readSample() override;
    void resetPhase() override;

    void setWaveform(WaveformType type);

private:
    bool active = false;
    float velocity = 0.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    float attackProgress = 0.0f;
    float attackTime = 0.08f;
    float deltaTime = 0.0f;
    uint32_t sampleRate = 44100;
    WaveformType waveform;
};

#endif