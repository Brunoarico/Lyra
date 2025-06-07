#include "AdditiveVoice.h"
#include <cmath>
#include "MidiUtils.h"
#include "Constants.h"
#include "esp_system.h"  // esp_random()

AdditiveVoice::AdditiveVoice(int harmonics)
    : numHarmonics(harmonics), frequency(440.0f), velocityAmp(1.0f),
      sampleRate(44100.0f), globalGain(4.0f), active(false) {
    
    // Decaimento harmônico mais natural
    amplitudes.resize(harmonics);
    float sum = 0.0f;
    for (int i = 0; i < harmonics; ++i) {
        amplitudes[i] = 1.0f / (i + 1);  // Amplitude inversamente proporcional ao harmônico
        sum += amplitudes[i];
    }
    for (int i = 0; i < harmonics; ++i) {
        amplitudes[i] /= sum;  // normaliza
    }

    phases.resize(harmonics);
    resetPhase();  // inicializa com fases aleatórias
}

void AdditiveVoice::setSampleRate(uint32_t sr) {
    sampleRate = sr;
    env.setSampleRate(sr);
}

void AdditiveVoice::noteOn(uint8_t note, uint8_t velocity) {
    frequency = noteToFreq(note);
    velocityAmp = amplitudeNormalized(velocity);
    resetPhase();  // fase aleatória evita cancelamentos periódicos
    env.noteOn();
    active = true;
}

void AdditiveVoice::noteOff() {
    env.noteOff();
}

float AdditiveVoice::readSample() {
    if (!active) return 0;

    float sample = 0.0f;
    float baseFreq = frequency;

    for (int i = 0; i < numHarmonics; ++i) {
        float freq = baseFreq * (i + 1);
        float phaseInc = M_TWOPI * freq / sampleRate;

        phases[i] += phaseInc;
        if (phases[i] >= M_TWOPI) phases[i] -= M_TWOPI;

        sample += sinf(phases[i]) * amplitudes[i];
    }

    float envLevel = env.next();
    if (env.finished()) active = false;

    // Normaliza pela soma total de amplitudes (ou use uma constante)
    return sample * velocityAmp * globalGain * envLevel * (RESOLUTION / numHarmonics);
}

bool AdditiveVoice::isActive() {
    return active;
}

void AdditiveVoice::resetPhase() {
    for (auto& phase : phases) {
        phase = (esp_random() & 0x3FF) / 1024.0f * M_TWOPI;  // [0, 2π) aleatório
    }
}

void AdditiveVoice::setAmplitude(int harmonic, float amp) {
    if (harmonic >= 0 && harmonic < numHarmonics) {
        amplitudes[harmonic] = amp;
    }
}

void AdditiveVoice::setGlobalGain(float gain) {
    globalGain = gain;
}
