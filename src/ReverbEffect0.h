#ifndef REVERB_EFFECT_H
#define REVERB_EFFECT_H

#include "Effect.h"
#include <math.h>
#include <stdlib.h>
#include <cstring>

class ReverbEffect : public Effect {
public:
    ReverbEffect(float mix = 0.3f, uint32_t sampleRate = 44100)
        : mix(mix), sampleRate(sampleRate) {
        init();
    }

    ~ReverbEffect() {
        freeBuffers();
    }

    void reset() override {
        for (int i = 0; i < NUM_COMBS; i++) {
            std::memset(combBuffers[i], 0, sizeof(float) * combSizes[i]);
            combIndices[i] = 0;
        }
        for (int i = 0; i < NUM_ALLPASS; i++) {
            std::memset(allpassBuffers[i], 0, sizeof(float) * allpassSizes[i]);
            allpassIndices[i] = 0;
        }
    }

    float process(float input) override {
        float wet = 0.0f;

        // Comb Filters
        for (int i = 0; i < NUM_COMBS; i++) {
            int idx = combIndices[i];
            float delayed = combBuffers[i][idx];
            combBuffers[i][idx] = input + delayed * 0.7f;
            combIndices[i] = (idx + 1) % combSizes[i];
            wet += delayed;
        }
        wet /= NUM_COMBS;

        // Allpass Filters (diffusion)
        for (int i = 0; i < NUM_ALLPASS; i++) {
            int idx = allpassIndices[i];
            float bufout = allpassBuffers[i][idx];
            float bufIn = wet + bufout * 0.5f;
            allpassBuffers[i][idx] = bufIn;
            wet = -wet + bufout;
            allpassIndices[i] = (idx + 1) % allpassSizes[i];
        }

        return (1.0f - mix) * input + mix * wet;
    }

private:
    float mix;
    uint32_t sampleRate;

    // === Comb filters ===
    static const int NUM_COMBS = 4;
    const int combSizes[NUM_COMBS] = {
        1116, 1188, 1277, 1356 // em amostras (~25-30 ms)
    };
    float* combBuffers[NUM_COMBS] = { nullptr };
    int combIndices[NUM_COMBS] = { 0 };

    // === Allpass filters ===
    static const int NUM_ALLPASS = 2;
    const int allpassSizes[NUM_ALLPASS] = {
        225, 341
    };
    float* allpassBuffers[NUM_ALLPASS] = { nullptr };
    int allpassIndices[NUM_ALLPASS] = { 0 };

    void init() {
        for (int i = 0; i < NUM_COMBS; i++) {
            combBuffers[i] = (float*)ps_malloc(sizeof(float) * combSizes[i]);
            if (!combBuffers[i]) combBuffers[i] = new float[combSizes[i]];
            std::memset(combBuffers[i], 0, sizeof(float) * combSizes[i]);
        }
        for (int i = 0; i < NUM_ALLPASS; i++) {
            allpassBuffers[i] = (float*)ps_malloc(sizeof(float) * allpassSizes[i]);
            if (!allpassBuffers[i]) allpassBuffers[i] = new float[allpassSizes[i]];
            std::memset(allpassBuffers[i], 0, sizeof(float) * allpassSizes[i]);
        }
    }

    void freeBuffers() {
        for (int i = 0; i < NUM_COMBS; i++) {
            if (combBuffers[i]) free(combBuffers[i]);
            combBuffers[i] = nullptr;
        }
        for (int i = 0; i < NUM_ALLPASS; i++) {
            if (allpassBuffers[i]) free(allpassBuffers[i]);
            allpassBuffers[i] = nullptr;
        }
    }
};

#endif
