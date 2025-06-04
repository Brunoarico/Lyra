#ifndef POLYSYNTH_H
#define POLYSYNTH_H

#include "Voice.h"
#include "OscillatorVoice.h"
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
    Envelope envelopes[MAX_VOICES];    // Envelope para cada voz
    bool active[MAX_VOICES];           // Se está em uso
    uint8_t notes[MAX_VOICES];         // Nota atual
    uint32_t sampleRate;
    InstrumentType instrumentType;
    EffectStack fx;

public:
    // Construtor que define qual instrumento esse PolySynth vai controlar
    PolySynth(InstrumentType type) : instrumentType(type) {
        for (int i = 0; i < MAX_VOICES; i++) {
            voices[i] = nullptr;
            active[i] = false;
            notes[i] = 0;
        }
    }

    int instrumentId() { return instrumentType; }

    void begin(uint32_t sr) {
        sampleRate = sr;

        // Apaga vozes antigas (se houver)
        for (int i = 0; i < MAX_VOICES; i++) {
            if (voices[i]) {
                delete voices[i];
                voices[i] = nullptr;
            }
        }

        // Cria as vozes do tipo escolhido
        for (int i = 0; i < MAX_VOICES; i++) {
            switch (instrumentType) {
                case INSTR_FM:
                    voices[i] = new FMVoice();
                    break;
                case INSTR_SNARE:
                    voices[i] = new SnareVoice();
                    break;
                case INSTR_SINE:
                    voices[i] = new OscillatorVoice(SINE);
                    break;
                case INSTR_SQUARE:
                    voices[i] = new OscillatorVoice(SQUARE);
                    break;
                case INSTR_SAWTOOTH:
                    voices[i] = new OscillatorVoice(SAWTOOTH);
                    break;
                case INSTR_TRIANGLE:
                    voices[i] = new OscillatorVoice(TRIANGLE);
                    break;
                case INSTR_KARPLUS:
                    voices[i] = new KarplusStrongVoice();
                    break;
                case INSTR_ADDITIVE:
                    voices[i] = new AdditiveVoice(1); // 5 harmônicos
                    break;
            }
            voices[i]->setSampleRate(sr);
            envelopes[i].setSampleRate(sr);
        }
        
        FilterEffect* filt = new FilterEffect(LOW_PASS, 1200.0f, 0.7f, sr);
        DelayEffect*  del  = new DelayEffect(sr, 250.0f, 0.4f);
        ReverbEffect* rev  = new ReverbEffect(0.5f, sr);

        fx.addEffect(filt);
        //fx.addEffect(del);
        fx.addEffect(rev);
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
        envelopes[idx].noteOn();
        active[idx] = true;
        notes[idx] = note;
    }

    void noteOff(uint8_t note) {
        for (int i = 0; i < MAX_VOICES; i++) {
            if (active[i] && notes[i] == note) {
                envelopes[i].noteOff();
                voices[i]->noteOff();
            }
        }
    }

    size_t read(uint8_t* buffer, size_t bytes) {
        size_t result = 0;
        int16_t* ptr = (int16_t*)buffer;
    
        int numSamples = bytes / 4; // Stereo: 2 canais * 2 bytes
        float masterGain = 1.0f;
    
        for (int i = 0; i < numSamples; i++) {
            float mix = 0.0f;
            int count = 0;
    
            for (int v = 0; v < MAX_VOICES; v++) {
                if (active[v]) {
                    float env = envelopes[v].next();
    
                    if (!envelopes[v].isActive()) {
                        active[v] = false;
                        continue;
                    }
    
                    float sample = voices[v]->readSample() * env;
                    mix += sample;
                    count++;
                }
            }
    
            if (count > 0) {
                float gainComp = 1.0f / sqrtf(count); // normalização suave
                mix *= gainComp;
    
                mix *= masterGain;
    
                mix = fx.process(mix); 
    
            }
    
            // Clamp final para garantir range válido
            if (mix > 32767.0f) mix = 32767.0f, Serial.println("Clamped to 32767");
            else if (mix < -32768.0f) mix = -32768.0f, Serial.println("Clamped to -32768");
    
            int16_t finalSample = (int16_t)mix;
            *ptr++ = finalSample;
            *ptr++ = finalSample;
            result += 4;
        }
    
        return result;
    }
    
};

#endif // POLYSYNTH_H
