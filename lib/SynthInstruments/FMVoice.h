#ifndef FMVOICE_H
#define FMVOICE_H

#include "Voice.h"
#include "../../src/Envelope.h"

class FMVoice : public Voice {
private:
    float modulatorPhase;
    float carrierPhase;
    float lfoPhase;

    float ratio;
    float baseModIndex;
    float amplitude;

    float carrierFreq;
    float modulatorFreq;

    float carrierIncrement;
    float modulatorIncrement;
    float lfoIncrement;

    uint32_t sampleRate;
    bool active;

    Envelope envelope;

    void updateIncrements();

public:
    FMVoice(float ratio = 2.0f, float modIndex = 5.0f);

    void setSampleRate(uint32_t sr) override;
    void noteOn(uint8_t note, uint8_t velocity) override;
    void noteOff() override;
    float readSample() override;
    bool isActive() override;
    void resetPhase() override;

    void setRatio(float newRatio);
    void setModIndex(float newModIndex);
    void setAmplitude(float newAmplitude);
};

#endif // FMVOICE_H
