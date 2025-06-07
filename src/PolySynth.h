#ifndef POLYSYNTH_H
#define POLYSYNTH_H

#include "Voice.h"
#include "OscillatorVoiceLUT.h"
#include "WaveformTable.h"
#include "KarplusStrongVoice.h"
#include "SnareVoice.h"
#include "AdditiveVoice.h"
#include "FMVoice.h"
#include "FilterEffect.h"
#include "DelayEffect.h"
#include "ReverbEffect.h"
#include "EffectStack.h"
#include "Envelope.h"
#include <Arduino.h>
#include "Constants.h"

class PolySynth {
    Voice* voices[MAX_VOICES];
    bool active[MAX_VOICES];
    uint8_t notes[MAX_VOICES];
    Envelope envelope;
    uint32_t sampleRate;
    InstrumentType instrumentType;
    EffectStack fx;

public:

    PolySynth(InstrumentType type) : instrumentType(type), sampleRate(44100) {
        for (int i = 0; i < MAX_VOICES; i++) {
            voices[i] = nullptr;
            active[i] = false;
            notes[i] = 0;
        }
    }

    int instrumentId() { return instrumentType; }

    void begin(uint32_t sr) {
        sampleRate = sr;
        envelope.setSampleRate(sr);
        setupVoices();
        setupEffects();
    }

    void changeInstrument(uint8_t newInstrumentId) {
        instrumentType = static_cast<InstrumentType>(newInstrumentId);
        setupVoices(); // recria as vozes com o novo instrumento
    }

    ~PolySynth() {
        for (int i = 0; i < MAX_VOICES; i++) {
            if (voices[i]) {
                delete voices[i];
                voices[i] = nullptr;
            }
        }
    }

    int findVoice(uint8_t note, bool reuse = false) {
        for (int i = 0; i < MAX_VOICES; i++) {
            if (reuse && active[i] && notes[i] == note) return i;
            if (!reuse && !active[i]) return i;
        }
        return -1;
    }

    void noteOn(uint8_t note, uint8_t velocity) {
        int idx = findVoice(note, true);
        if (idx == -1) idx = findVoice(note, false);
        if (idx == -1) return;
        voices[idx]->resetPhase();
        voices[idx]->noteOn(note, velocity);
        active[idx] = true;
        notes[idx] = note;
        envelope.noteOn();
    }

    void noteOff(uint8_t note) {
        bool stillActive = false;
        for (int i = 0; i < MAX_VOICES; i++) {
            if (active[i] && notes[i] == note) {
                voices[i]->noteOff();
                active[i] = false;
            }
            if (active[i]) {
                stillActive = true;
            }
        }

        if (!stillActive) {
            envelope.noteOff();
        }
    }

    size_t read(uint8_t* buffer, size_t bytes) {
        size_t result = 0;
        int16_t* ptr = (int16_t*)buffer;
        int numSamples = bytes / 4; // Stereo: 2 canais * 2 bytes
        float masterGain = .8f;

        for (int i = 0; i < numSamples; i++) {
            float mix = 0.0f;
            int count = 0;

            for (int v = 0; v < MAX_VOICES; v++) {
                if (active[v]) {
                    float sample = voices[v]->readSample();
                    mix += sample;
                    count++;
                }
            }

            if (count > 0) {
                mix *= (1.0f / sqrtf((float)count)); // normalização suave
                mix *= envelope.next();              // envelope único
                mix *= masterGain;

                mix = fx.process(mix); // efeitos opcionais
            }

            int16_t finalSample = (int16_t)mix;
            *ptr++ = finalSample;
            *ptr++ = finalSample;
            result += 4;
        }

        return result;
    }

private:

    void setupVoices() {
        // Apaga vozes anteriores
        for (int i = 0; i < MAX_VOICES; i++) {
            if (voices[i]) {
                delete voices[i];
                voices[i] = nullptr;
            }
            active[i] = false;
            notes[i] = 0;
        }

        // Cria novas vozes conforme o tipo de instrumento
        for (int i = 0; i < MAX_VOICES; i++) {
            switch (instrumentType) {
                case INSTR_FM:
                    voices[i] = new FMVoice(10.0f, 6.0f);
                    break;
                case INSTR_SNARE:
                    voices[i] = new SnareVoice();
                    break;
                case INSTR_SINE:
                    voices[i] = new OscillatorVoiceLUT(SINE);
                    break;
                case INSTR_SQUARE:
                    voices[i] = new OscillatorVoiceLUT(SQUARE);
                    break;
                case INSTR_SAWTOOTH:
                    voices[i] = new OscillatorVoiceLUT(SAWTOOTH);
                    break;
                case INSTR_TRIANGLE:
                    voices[i] = new OscillatorVoiceLUT(TRIANGLE);
                    break;
                case INSTR_KARPLUS:
                    voices[i] = new KarplusStrongVoice();
                    break;
                case INSTR_ADDITIVE:
                    voices[i] = new AdditiveVoice(5);
                    break;
            }
            voices[i]->setSampleRate(sampleRate);
        }
    }

    void setupEffects() {
        fx.clear();
        // Ative aqui os efeitos desejados
        // fx.addEffect(new FilterEffect(LOW_PASS, 1000.0f, 0.7f, sampleRate));
        // fx.addEffect(new DelayEffect(sampleRate, 100.0f, 0.4f));
        // fx.addEffect(new ReverbEffect(0.5f, sampleRate));
    }
};

#endif // POLYSYNTH_H
