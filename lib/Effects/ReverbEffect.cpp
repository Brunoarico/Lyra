#include "ReverbEffect.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// === Constantes base para delays ===
const int ReverbEffect::baseCombSizes[NUM_COMBS] = { 1116, 1188, 1277, 1356 };
const int ReverbEffect::baseAllpassSizes[NUM_ALLPASS] = { 225, 341 };

ReverbEffect::ReverbEffect(float mix, uint32_t sampleRate)
    : mix(mix), feedback(0.7f), sizeScale(1.0f), sampleRate(sampleRate)
{
    for (int i = 0; i < NUM_COMBS; ++i) {
        combBuffers[i] = nullptr;
        combIndices[i] = 0;
    }
    for (int i = 0; i < NUM_ALLPASS; ++i) {
        allpassBuffers[i] = nullptr;
        allpassIndices[i] = 0;
    }
    init();
}

ReverbEffect::~ReverbEffect() {
    freeBuffers();
}

void ReverbEffect::setSampleRate(uint32_t sr) {
    sampleRate = sr;
    reset();
}

void ReverbEffect::init() {
    // Comb filters
    for (int i = 0; i < NUM_COMBS; i++) {
        combSizes[i] = static_cast<int>(baseCombSizes[i] * sizeScale);
        if (combBuffers[i]) free(combBuffers[i]);
        combBuffers[i] = (float*)malloc(sizeof(float) * combSizes[i]);
        if (!combBuffers[i]) combBuffers[i] = new float[combSizes[i]];
        memset(combBuffers[i], 0, sizeof(float) * combSizes[i]);
        combIndices[i] = 0;
    }

    // Allpass filters
    for (int i = 0; i < NUM_ALLPASS; i++) {
        allpassSizes[i] = static_cast<int>(baseAllpassSizes[i] * sizeScale);
        if (allpassBuffers[i]) free(allpassBuffers[i]);
        allpassBuffers[i] = (float*)malloc(sizeof(float) * allpassSizes[i]);
        if (!allpassBuffers[i]) allpassBuffers[i] = new float[allpassSizes[i]];
        memset(allpassBuffers[i], 0, sizeof(float) * allpassSizes[i]);
        allpassIndices[i] = 0;
    }
}

void ReverbEffect::freeBuffers() {
    for (int i = 0; i < NUM_COMBS; ++i) {
        if (combBuffers[i]) free(combBuffers[i]);
        combBuffers[i] = nullptr;
    }
    for (int i = 0; i < NUM_ALLPASS; ++i) {
        if (allpassBuffers[i]) free(allpassBuffers[i]);
        allpassBuffers[i] = nullptr;
    }
}

void ReverbEffect::reset() {
    for (int i = 0; i < NUM_COMBS; i++) {
        memset(combBuffers[i], 0, sizeof(float) * combSizes[i]);
        combIndices[i] = 0;
    }
    for (int i = 0; i < NUM_ALLPASS; i++) {
        memset(allpassBuffers[i], 0, sizeof(float) * allpassSizes[i]);
        allpassIndices[i] = 0;
    }
}

float ReverbEffect::process(float input) {
    float wet = 0.0f;

    // Comb Filters
    for (int i = 0; i < NUM_COMBS; i++) {
        int idx = combIndices[i];
        float delayed = combBuffers[i][idx];
        combBuffers[i][idx] = input + delayed * feedback;
        combIndices[i] = (idx + 1) % combSizes[i];
        wet += delayed;
    }
    wet /= NUM_COMBS;

    // Allpass Filters
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

void ReverbEffect::setMix(float m) {
    if (m < 0.0f) m = 0.0f;
    if (m > 1.0f) m = 1.0f;
    mix = m;
}

void ReverbEffect::setFeedback(float fb) {
    if (fb < 0.0f) fb = 0.0f;
    if (fb > 0.9f) fb = 0.9f;
    feedback = fb;
}

void ReverbEffect::setSize(float scale) {
    if (scale < 0.5f) scale = 0.5f;
    if (scale > 2.0f) scale = 2.0f;
    sizeScale = scale;
    init(); // realoca os buffers com novos tamanhos
}
