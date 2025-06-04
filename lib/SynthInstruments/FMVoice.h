#ifndef FMVOICE_H
#define FMVOICE_H

#include "Voice.h"
#include <math.h>

class FMVoice : public Voice {
public:
    FMVoice(float ratio = 2.0f, float modIndex = 1.0f);

    void setSampleRate(uint32_t sr) override;
    void noteOn(uint8_t note, uint8_t velocity) override;
    void noteOff() override;
    float readSample() override;
    bool isActive() override;
    void resetPhase() override;

    void setRatio(float newRatio);
    void setModIndex(float newModIndex);
    void setAmplitude(float newAmplitude);

private:
    void updateIncrements();

    bool active;
    float amplitude;

    float carrierFreq;
    float modulatorFreq;
    float ratio;
    float modIndex;

    float carrierPhase;
    float modulatorPhase;
    float carrierIncrement;
    float modulatorIncrement;
    uint32_t sampleRate;
};

#endif // FMVOICE_H
