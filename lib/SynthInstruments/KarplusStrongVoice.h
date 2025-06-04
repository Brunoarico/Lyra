#ifndef KARPLUS_STRONG_VOICE_H
#define KARPLUS_STRONG_VOICE_H

#include "Voice.h"
#include <vector>
#include <stdint.h>

class KarplusStrongVoice : public Voice {
private:
    std::vector<int16_t> buffer;
    size_t index = 0;
    bool active = false;
    uint32_t sampleRate = 44100;
    float damping = 1.0f;  // controle do decaimento (0..1)

public:
    KarplusStrongVoice();

    void setSampleRate(uint32_t sr) override;

    void noteOn(uint8_t note, uint8_t velocity) override;

    void noteOff() override;

    void resetPhase() override;

    float readSample() override;

    bool isActive() override;

    void setDamping(float d);

};

#endif // KARPLUS_STRONG_VOICE_H
