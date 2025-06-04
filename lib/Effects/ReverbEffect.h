#ifndef REVERB_EFFECT_H
#define REVERB_EFFECT_H

#include "Effect.h"
#include <stdint.h>

class ReverbEffect : public Effect {
public:
    ReverbEffect(float mix = 0.3f, uint32_t sampleRate = 44100);
    ~ReverbEffect();

    float process(float input) override;
    void reset() override;
    void setSampleRate(uint32_t sr);

    void setMix(float m);       // Mix entre som seco e molhado (0.0 a 1.0)
    void setFeedback(float fb); // Feedback dos combs (0.0 a ~0.9)
    void setSize(float scale);  // Escala dos tamanhos de delay (0.5 a 2.0)

private:
    void init();
    void freeBuffers();

    float mix;
    float feedback;
    float sizeScale;
    uint32_t sampleRate;

    static const int NUM_COMBS = 4;
    static const int NUM_ALLPASS = 2;

    int combSizes[NUM_COMBS];
    float* combBuffers[NUM_COMBS];
    int combIndices[NUM_COMBS];

    int allpassSizes[NUM_ALLPASS];
    float* allpassBuffers[NUM_ALLPASS];
    int allpassIndices[NUM_ALLPASS];

    static const int baseCombSizes[NUM_COMBS];
    static const int baseAllpassSizes[NUM_ALLPASS];
};

#endif // REVERB_EFFECT_H
