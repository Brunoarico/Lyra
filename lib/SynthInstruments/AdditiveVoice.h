#pragma once

#include "Voice.h"
#include "MidiUtils.h"
#include "Constants.h"
#include <vector>
#include <stdint.h>

class AdditiveVoice : public Voice {
public:
    AdditiveVoice(int harmonics = 5);

    void setSampleRate(uint32_t sr) override;
    void noteOn(uint8_t note, uint8_t velocity) override;
    void noteOff() override;
    float readSample() override;
    bool isActive() override;
    void resetPhase() override;

    void setAmplitude(int harmonic, float amp); // controle externo

private:
    int numHarmonics;
    std::vector<float> amplitudes;
    std::vector<float> phases;
    float frequency = 440.0f;
    float velocityAmp = 1.0f;
    uint32_t sampleRate = 44100;
    bool active = false;
};
