#include "KarplusStrongVoice.h"
#include <stdlib.h>
#include <cmath>
#include "Constants.h"
#include "MidiUtils.h"

KarplusStrongVoice::KarplusStrongVoice() {}

void KarplusStrongVoice::setSampleRate(uint32_t sr) {
    sampleRate = sr;
}

void KarplusStrongVoice::noteOn(uint8_t note, uint8_t velocity) {
    float freq = noteToFreq(note);
    size_t size = sampleRate / freq;

    buffer.resize(size);
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = (rand() % int(RESOLUTION * 2.0f)) - RESOLUTION;
    }
    index = 0;
    active = true;
}

void KarplusStrongVoice::noteOff() {
    active = false;
}

void KarplusStrongVoice::resetPhase() {
    index = 0;
}

float KarplusStrongVoice::readSample() {
    if (!active || buffer.empty()) return 0.0f;

    int16_t current = buffer[index];
    int16_t next = buffer[(index + 1) % buffer.size()];

    int16_t newSample = (int16_t)(((current + next) / 2.0f) * damping);

    buffer[index] = newSample;
    index = (index + 1) % buffer.size();

    // Retorna valor float normalizado e aplicado volume (mix)
    return (float)newSample;
}

bool KarplusStrongVoice::isActive() {
    return active;
}

void KarplusStrongVoice::setDamping(float d) {
    if (d < 0.0f) d = 0.0f;
    else if (d > 1.0f) d = 1.0f;
    damping = d;
}

