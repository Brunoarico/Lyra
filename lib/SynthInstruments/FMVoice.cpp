#include "FMVoice.h"
#include "MidiUtils.h"
#include "Constants.h"
#include <Arduino.h>
#include <cmath>


FMVoice::FMVoice(float ratio, float modIndex)
    : modulatorPhase(0), carrierPhase(0), ratio(ratio), modIndex(modIndex),
      active(false), amplitude(0.0), carrierFreq(440.0), modulatorFreq(880.0),
      carrierIncrement(0.0), modulatorIncrement(0.0), sampleRate(44100) {}

void FMVoice::setSampleRate(uint32_t sr) {
    sampleRate = sr;
    updateIncrements();
}

void FMVoice::noteOn(uint8_t note, uint8_t velocity) {
    active = true;
    carrierFreq = noteToFreq(note);
    modulatorFreq = carrierFreq * ratio;
    amplitude = amplitudeNormalized(velocity) * RESOLUTION; // reduzir para evitar clipping
    updateIncrements();
    resetPhase();
}

void FMVoice::noteOff() {
    active = false;
}

float FMVoice::readSample() {
    if (!active) return 0;

    const int oversampleFactor = 2;
    float total = 0.0;

    for (int i = 0; i < oversampleFactor; ++i) {
        float mod = sinf(modulatorPhase) * modIndex;
        float sample = sinf(carrierPhase + mod);

        total += sample;

        modulatorPhase = fmodf(modulatorPhase + modulatorIncrement / oversampleFactor, M_TWOPI);
        carrierPhase = fmodf(carrierPhase + carrierIncrement / oversampleFactor, M_TWOPI);
    }

    float averagedSample = total / oversampleFactor;
    return averagedSample * amplitude;
}

bool FMVoice::isActive() {
    return active;
}

void FMVoice::resetPhase() {
    carrierPhase = random(0, 1000) / 1000.0f * M_TWOPI;
    modulatorPhase = random(0, 1000) / 1000.0f * M_TWOPI;
}

void FMVoice::updateIncrements() {
    carrierIncrement = M_TWOPI * carrierFreq / sampleRate;
    modulatorIncrement = M_TWOPI * modulatorFreq / sampleRate;
}

void FMVoice::setRatio(float newRatio) {
    ratio = newRatio;
    modulatorFreq = carrierFreq * ratio;
    updateIncrements();
}

void FMVoice::setModIndex(float newModIndex) {
    modIndex = newModIndex;
}

void FMVoice::setAmplitude(float newAmplitude) {
    amplitude = newAmplitude;
}
