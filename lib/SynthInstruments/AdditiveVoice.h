#ifndef ADDITIVE_VOICE_H
#define ADDITIVE_VOICE_H

#include "Voice.h"
#include <vector>
#include "../../src/Envelope.h"

class AdditiveVoice : public Voice {
public:
    AdditiveVoice(int harmonics = 3);

    void setSampleRate(uint32_t sr) override;
    void noteOn(uint8_t note, uint8_t velocity) override;
    void noteOff() override;
    float readSample() override;
    bool isActive() override;
    void resetPhase() override;

    void setAmplitude(int harmonic, float amp);
    void setGlobalGain(float gain);

private:
    int numHarmonics;
    std::vector<float> amplitudes;
    std::vector<float> phases;
    float frequency;
    float velocityAmp;
    float sampleRate;
    float globalGain;
    bool active;
    Envelope env;
};

#endif
