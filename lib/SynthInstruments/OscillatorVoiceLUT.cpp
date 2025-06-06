#include "OscillatorVoiceLUT.h"
#include "MidiUtils.h"
#include "Constants.h"
#include <stdlib.h>
#include <Arduino.h>

OscillatorVoiceLUT::OscillatorVoiceLUT(WaveformType type)
    : waveform(type) {}

void OscillatorVoiceLUT::setWaveform(WaveformType type) {
    waveform = type;
}

void OscillatorVoiceLUT::setSampleRate(uint32_t sr) {
    sampleRate = sr;
    deltaTime = 1.0f / sampleRate;
}

void OscillatorVoiceLUT::noteOn(uint8_t note, uint8_t velocity) {
    float freq = noteToFreq(note);
    active = true;
    this->velocity = amplitudeNormalized(velocity);
    freq = fmaxf(freq, 100.0f);
    phaseIncrement = freq / sampleRate;
    attackProgress = 0.0f;
}

void OscillatorVoiceLUT::noteOff() {
    active = false;
}

bool OscillatorVoiceLUT::isActive() {
    return active;
}

void OscillatorVoiceLUT::resetPhase() {
    phase = static_cast<float>(rand()) / RAND_MAX;
}

float OscillatorVoiceLUT::readSample() {
    if (!active) return 0.0f;

    // Envelope attack
    if (attackProgress < 1.0f) {
        attackProgress += deltaTime / attackTime;
        if (attackProgress > 1.0f) attackProgress = 1.0f;
    }

    float sample = WaveformTable::getSample(waveform, phase);
    phase += phaseIncrement;
    if (phase >= 1.0f) phase -= 1.0f;

    return sample * velocity * attackProgress * RESOLUTION;
}
