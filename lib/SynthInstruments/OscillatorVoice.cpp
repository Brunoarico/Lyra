#include "OscillatorVoice.h"
#include <stdlib.h>
#include <math.h>
#include "MidiUtils.h"
#include "Constants.h"

OscillatorVoice::OscillatorVoice(WaveformType type)
    : waveform(type) {}

void OscillatorVoice::noteOn(uint8_t note, uint8_t velocity) {
    active = true;
    releasing = false;
    releaseProgress = 0.0f;
    this->velocity = amplitudeNormalized(velocity);
    phase = static_cast<float>(rand()) / RAND_MAX * M_TWOPI;
    cycles = 0.0f;

    float freq = noteToFreq(note);
    frequency = fmaxf(freq, 20.0f);

    attackProgress = 0.0f;
}

void OscillatorVoice::noteOff() {
    releasing = true;
    releaseProgress = 0.0f;
}

bool OscillatorVoice::isActive() {
    return active;
}

void OscillatorVoice::setSampleRate(uint32_t sr) {
    sampleRate = sr;
    deltaTime = 1.0f / sr;
}

void OscillatorVoice::setWaveform(WaveformType type) {
    waveform = type;
}

void OscillatorVoice::resetPhase() {
    phase = static_cast<float>(rand()) / RAND_MAX * M_TWOPI;
    cycles = 0.0f;
}

float OscillatorVoice::readSample() {
    if (!active) return 0.0f;

    float t = cycles;
    float sample = 0.0f;

    switch (waveform) {
        case SINE:
            sample = sinf(M_TWOPI * t + phase);
            break;
        case SQUARE:
            sample = (fmodf(t, 1.0f) < 0.5f) ? 1.0f : -1.0f;
            break;
        case TRIANGLE:
            sample = 4.0f * fabsf(fmodf(t, 1.0f) - 0.5f) - 1.0f;
            break;
        case SAWTOOTH:
            sample = 2.0f * fmodf(t, 1.0f) - 1.0f;
            break;
        case NOISE:
            sample = 2.0f * ((float)rand() / RAND_MAX) - 1.0f;
            break;
    }

    cycles += frequency * deltaTime;
    if (cycles > 1.0f) cycles -= 1.0f;

    if (!releasing && attackProgress < 1.0f) {
        attackProgress += deltaTime / attackTime;
        if (attackProgress > 1.0f) attackProgress = 1.0f;
    }

    if (releasing) {
        releaseProgress += deltaTime / releaseTime;
        if (releaseProgress >= 1.0f) {
            active = false;
            releasing = false;
            return 0.0f;
        }
    }

    float amp = velocity * attackProgress * (1.0f - releaseProgress);
    return sample * amp * RESOLUTION;
}
