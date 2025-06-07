#include "FMVoice.h"
#include "MidiUtils.h"
#include "Constants.h"
#include <Arduino.h>
#include <cmath>

FMVoice::FMVoice(float ratio, float modIndex)
    : modulatorPhase(0), carrierPhase(0), lfoPhase(0),
      ratio(ratio), baseModIndex(modIndex),
      active(false), amplitude(0.0f),
      carrierFreq(440.0f), modulatorFreq(880.0f),
      carrierIncrement(0.0f), modulatorIncrement(0.0f),
      lfoIncrement(0.0f), sampleRate(44100) {}

void FMVoice::setSampleRate(uint32_t sr) {
    sampleRate = sr;
    envelope.setSampleRate(sr);
    updateIncrements();
}

void FMVoice::noteOn(uint8_t note, uint8_t velocity) {
    active = true;
    carrierFreq = noteToFreq(note);
    modulatorFreq = carrierFreq * ratio;
    amplitude = amplitudeNormalized(velocity) * RESOLUTION * 0.8f; // mais headroom
    envelope.noteOn();
    updateIncrements();
    resetPhase();
}

void FMVoice::noteOff() {
    envelope.noteOff();
}

float FMVoice::readSample() {
    if (!active && envelope.finished()) return 0;

    float env = envelope.next();
    if (env <= 0.0001f) return 0;

    // LFO para modulação do índice
    float lfo = 0.5f * sinf(lfoPhase);
    lfoPhase = fmodf(lfoPhase + lfoIncrement, M_TWOPI);

    float mod = sinf(modulatorPhase) * (baseModIndex + lfo * 2.0f); // modulação dinâmica
    float sample = sinf(carrierPhase + mod);

    modulatorPhase = fmodf(modulatorPhase + modulatorIncrement, M_TWOPI);
    carrierPhase = fmodf(carrierPhase + carrierIncrement, M_TWOPI);

    float finalSample = tanhf(sample * amplitude * env * RESOLUTION); // clipping suave
    return finalSample;
}

bool FMVoice::isActive() {
    return active || !envelope.finished();
}

void FMVoice::resetPhase() {
    carrierPhase = random(0, 1000) / 1000.0f * M_TWOPI;
    modulatorPhase = random(0, 1000) / 1000.0f * M_TWOPI;
    lfoPhase = 0;
}

void FMVoice::updateIncrements() {
    carrierIncrement = M_TWOPI * carrierFreq / sampleRate;
    modulatorIncrement = M_TWOPI * modulatorFreq / sampleRate;
    lfoIncrement = M_TWOPI * 2.0f / sampleRate; // LFO a 2 Hz
}

void FMVoice::setRatio(float newRatio) {
    ratio = newRatio;
    modulatorFreq = carrierFreq * ratio;
    updateIncrements();
}

void FMVoice::setModIndex(float newModIndex) {
    baseModIndex = newModIndex;
}

void FMVoice::setAmplitude(float newAmplitude) {
    amplitude = newAmplitude;
}
