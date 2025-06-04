#ifndef SNARE_VOICE_H
#define SNARE_VOICE_H

#include "Voice.h"
#include <stdint.h>

class SnareVoice : public Voice {
public:
    SnareVoice();

    void setSampleRate(uint32_t sr) override;
    void noteOn(uint8_t note, uint8_t velocity) override;
    void noteOff() override;
    float readSample() override;
    void resetPhase() override;
    bool isActive() override;

    void setDecay(float d);     // d ∈ [0.0001, 0.01]
    void setNoiseColor(float b); // b ∈ [0.0, 1.0], distorce o centro do ruído

private:
    float amplitude;
    float decay;
    float brightness;
    bool active;
    uint32_t sampleRate;
};

#endif // SNARE_VOICE_H
