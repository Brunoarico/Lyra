#include "SnareVoice.h"
#include "Constants.h"
#include <stdlib.h>
#include "MidiUtils.h"

SnareVoice::SnareVoice()
    : amplitude(0.0f), decay(0.0005f), brightness(0.0f), active(false), sampleRate(44100) {}

void SnareVoice::setSampleRate(uint32_t sr) {
    sampleRate = sr;
}

void SnareVoice::noteOn(uint8_t note, uint8_t velocity) {
    amplitude = amplitudeNormalized(velocity);
    active = true;
}

void SnareVoice::noteOff() {
    // Snare decays naturally
}

float SnareVoice::readSample() {
    if (!active) return 0.0f;

    float noise = 2.0f * ((float)rand() / RAND_MAX) - 1.0f;
    noise = noise + brightness; // pode introduzir um tom ou realce

    float sample = amplitude * noise;

    amplitude -= decay;
    if (amplitude <= 0.0f) {
        amplitude = 0.0f;
        active = false;
    }

    return sample * (RESOLUTION - 1);
}

void SnareVoice::resetPhase() {
    // No phase used
}

bool SnareVoice::isActive() {
    return active;
}

void SnareVoice::setDecay(float d) {
    // Clamp to reasonable range
    if (d < 0.0001f) d = 0.0001f;
    if (d > 0.01f) d = 0.01f;
    decay = d;
}

void SnareVoice::setNoiseColor(float b) {
    // Clamp brightness (bias)
    if (b < -1.0f) b = -1.0f;
    if (b > 1.0f) b = 1.0f;
    brightness = b;
}
