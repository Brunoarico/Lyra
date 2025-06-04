#include "AdditiveVoice.h"
#include <cmath>

AdditiveVoice::AdditiveVoice(int harmonics) : numHarmonics(harmonics) {
    amplitudes.resize(harmonics, 1.0f / harmonics);
    phases.resize(harmonics, 0.0f);
}

void AdditiveVoice::setSampleRate(uint32_t sr) {
    sampleRate = sr;
}

void AdditiveVoice::noteOn(uint8_t note, uint8_t velocity) {
    frequency = noteToFreq(note);
    velocityAmp = amplitudeNormalized(velocity);
    for (auto& phase : phases) phase = 0.0f;
    active = true;
}

void AdditiveVoice::noteOff() {
    active = false;
}

float AdditiveVoice::readSample() {
    if (!active) return 0;

    float sample = 0.0f;

    for (int i = 0; i < numHarmonics; i++) {
        float harmonicFreq = frequency * (i + 1);
        float increment = M_TWOPI * harmonicFreq / sampleRate;
        phases[i] += increment;

        if (phases[i] >= M_TWOPI) phases[i] -= M_TWOPI;

        sample += sinf(phases[i]) * amplitudes[i];
    }

    sample *= velocityAmp;
    return (int16_t)(sample * (RESOLUTION-1.0f));
}

bool AdditiveVoice::isActive() {
    return active;
}

void AdditiveVoice::resetPhase() {
    for (auto& phase : phases) phase = 0.0f;
}

void AdditiveVoice::setAmplitude(int harmonic, float amp) {
    if (harmonic >= 0 && harmonic < numHarmonics) {
        amplitudes[harmonic] = amp;
    }
}
