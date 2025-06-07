#include <Arduino.h>
#include <AudioKitHAL.h>
#include "PolySynth.h"
#include "Constants.h"
#include "WaveformTable.h"

AudioKit kit;

// Número máximo de canais MIDI
#define MAX_CHANNELS 12

// Array de ponteiros para PolySynth (um por canal MIDI)
PolySynth* synths[MAX_CHANNELS];

// Buffers para cada synth e buffer final mixado
uint8_t* buffers[MAX_CHANNELS];
int16_t mixedBuffer[BUFFER_SIZE / 2]; // 16-bit samples, stereo = BUFFER_SIZE/2 samples

// Tabela que mapeia números de programa (0–127) para instrumentos
const uint8_t programToInstrument[128] = {
    INSTR_ADDITIVE,   // 0: Acoustic Grand Piano
    INSTR_ADDITIVE,   // 1: Bright Acoustic Piano
    INSTR_ADDITIVE,   // 2: Electric Grand Piano
    INSTR_ADDITIVE,   // 3: Honky-tonk Piano
    INSTR_ADDITIVE,   // 4: Electric Piano 1
    INSTR_ADDITIVE,   // 5: Electric Piano 2
    INSTR_FM,         // 6: Harpsichord
    INSTR_SQUARE,     // 7: Clavinet

    INSTR_ADDITIVE,   // 8: Celesta
    INSTR_ADDITIVE,   // 9: Glockenspiel
    INSTR_ADDITIVE,   // 10: Music Box
    INSTR_ADDITIVE,   // 11: Vibraphone
    INSTR_ADDITIVE,   // 12: Marimba
    INSTR_ADDITIVE,   // 13: Xylophone
    INSTR_ADDITIVE,   // 14: Tubular Bells
    INSTR_ADDITIVE,   // 15: Dulcimer

    INSTR_SINE,       // 16: Drawbar Organ
    INSTR_SINE,       // 17: Percussive Organ
    INSTR_SINE,       // 18: Rock Organ
    INSTR_SINE,       // 19: Church Organ
    INSTR_SINE,       // 20: Reed Organ
    INSTR_SINE,       // 21: Accordion
    INSTR_SINE,       // 22: Harmonica
    INSTR_SINE,       // 23: Tango Accordion

    INSTR_TRIANGLE,   // 24: Acoustic Guitar (nylon)
    INSTR_TRIANGLE,   // 25: Acoustic Guitar (steel)
    INSTR_KARPLUS,    // 26: Electric Guitar (jazz)
    INSTR_KARPLUS,    // 27: Electric Guitar (clean)
    INSTR_KARPLUS,    // 28: Electric Guitar (muted)
    INSTR_KARPLUS,    // 29: Overdriven Guitar
    INSTR_KARPLUS,    // 30: Distortion Guitar
    INSTR_KARPLUS,    // 31: Guitar harmonics

    INSTR_SQUARE,     // 32: Acoustic Bass
    INSTR_SQUARE,     // 33: Electric Bass (finger)
    INSTR_SQUARE,     // 34: Electric Bass (pick)
    INSTR_SQUARE,     // 35: Fretless Bass
    INSTR_SQUARE,     // 36: Slap Bass 1
    INSTR_SQUARE,     // 37: Slap Bass 2
    INSTR_SQUARE,     // 38: Synth Bass 1
    INSTR_FM,         // 39: Synth Bass 2

    INSTR_ADDITIVE,   // 40: Violin
    INSTR_ADDITIVE,   // 41: Viola
    INSTR_ADDITIVE,   // 42: Cello
    INSTR_ADDITIVE,   // 43: Contrabass
    INSTR_ADDITIVE,   // 44: Tremolo Strings
    INSTR_ADDITIVE,   // 45: Pizzicato Strings
    INSTR_ADDITIVE,   // 46: Orchestral Harp
    INSTR_ADDITIVE,   // 47: Timpani

    INSTR_ADDITIVE,   // 48: String Ensemble 1
    INSTR_ADDITIVE,   // 49: String Ensemble 2
    INSTR_ADDITIVE,   // 50: Synth Strings 1
    INSTR_ADDITIVE,   // 51: Synth Strings 2
    INSTR_ADDITIVE,   // 52: Choir Aahs
    INSTR_ADDITIVE,   // 53: Voice Oohs
    INSTR_ADDITIVE,   // 54: Synth Voice
    INSTR_ADDITIVE,   // 55: Orchestra Hit

    INSTR_FM,         // 56: Trumpet
    INSTR_FM,         // 57: Trombone
    INSTR_FM,         // 58: Tuba
    INSTR_FM,         // 59: Muted Trumpet
    INSTR_FM,         // 60: French Horn
    INSTR_FM,         // 61: Brass Section
    INSTR_FM,         // 62: Synth Brass 1
    INSTR_FM,         // 63: Synth Brass 2

    INSTR_SINE,       // 64: Soprano Sax
    INSTR_SINE,       // 65: Alto Sax
    INSTR_SINE,       // 66: Tenor Sax
    INSTR_SINE,       // 67: Baritone Sax
    INSTR_SINE,       // 68: Oboe
    INSTR_SINE,       // 69: English Horn
    INSTR_SINE,       // 70: Bassoon
    INSTR_SINE,       // 71: Clarinet

    INSTR_SINE,       // 72: Piccolo
    INSTR_SINE,       // 73: Flute
    INSTR_SINE,       // 74: Recorder
    INSTR_SINE,       // 75: Pan Flute
    INSTR_SINE,       // 76: Blown Bottle
    INSTR_SINE,       // 77: Shakuhachi
    INSTR_SINE,       // 78: Whistle
    INSTR_SINE,       // 79: Ocarina

    INSTR_SAWTOOTH,   // 80: Square Lead
    INSTR_SAWTOOTH,   // 81: Saw Lead
    INSTR_SAWTOOTH,   // 82: Calliope Lead
    INSTR_SAWTOOTH,   // 83: Chiff Lead
    INSTR_SAWTOOTH,   // 84: Charang
    INSTR_SAWTOOTH,   // 85: Voice Lead
    INSTR_SAWTOOTH,   // 86: Fifths Lead
    INSTR_SAWTOOTH,   // 87: Bass + Lead

    INSTR_ADDITIVE,   // 88: New Age Pad
    INSTR_ADDITIVE,   // 89: Warm Pad
    INSTR_ADDITIVE,   // 90: Polysynth Pad
    INSTR_ADDITIVE,   // 91: Choir Pad
    INSTR_ADDITIVE,   // 92: Bowed Pad
    INSTR_ADDITIVE,   // 93: Metallic Pad
    INSTR_ADDITIVE,   // 94: Halo Pad
    INSTR_ADDITIVE,   // 95: Sweep Pad

    INSTR_ADDITIVE,   // 96: Rain
    INSTR_ADDITIVE,   // 97: Soundtrack
    INSTR_ADDITIVE,   // 98: Crystal
    INSTR_ADDITIVE,   // 99: Atmosphere
    INSTR_ADDITIVE,   // 100: Brightness
    INSTR_ADDITIVE,   // 101: Goblins
    INSTR_ADDITIVE,   // 102: Echoes
    INSTR_ADDITIVE,   // 103: Sci-fi

    INSTR_SNARE,      // 104: Sitar
    INSTR_KARPLUS,    // 105: Banjo
    INSTR_KARPLUS,    // 106: Shamisen
    INSTR_KARPLUS,    // 107: Koto
    INSTR_KARPLUS,    // 108: Kalimba
    INSTR_SNARE,      // 109: Bagpipe
    INSTR_SNARE,      // 110: Fiddle
    INSTR_KARPLUS,    // 111: Shanai

    INSTR_SINE,       // 112: Tinkle Bell
    INSTR_SINE,       // 113: Agogo
    INSTR_SINE,       // 114: Steel Drums
    INSTR_SINE,       // 115: Woodblock
    INSTR_SNARE,      // 116: Taiko Drum
    INSTR_SNARE,      // 117: Melodic Tom
    INSTR_SNARE,      // 118: Synth Drum
    INSTR_SNARE,      // 119: Reverse Cymbal

    INSTR_ADDITIVE,   // 120: Guitar Fret Noise
    INSTR_ADDITIVE,   // 121: Breath Noise
    INSTR_ADDITIVE,   // 122: Seashore
    INSTR_ADDITIVE,   // 123: Bird Tweet
    INSTR_ADDITIVE,   // 124: Telephone Ring
    INSTR_ADDITIVE,   // 125: Helicopter
    INSTR_ADDITIVE,   // 126: Applause
    INSTR_ADDITIVE    // 127: Gunshot
};

const char* getInstrumentName(InstrumentType type) {
    switch (type) {
        case INSTR_SNARE:    return "Snare";
        case INSTR_KARPLUS:  return "Karplus";
        case INSTR_SINE:     return "Sine";
        case INSTR_SQUARE:   return "Square";
        case INSTR_TRIANGLE: return "Triangle";
        case INSTR_FM:       return "FM";
        case INSTR_ADDITIVE: return "Additive";
        case INSTR_SAWTOOTH: return "Sawtooth";
        default:             return "Unknown";
    }
}

void mixBuffers(int16_t* dest, uint8_t** srcBuffers, size_t numBuffers, size_t bytes) {
    memset(dest, 0, bytes);
    size_t samples = bytes / 2;

    for (size_t bufIdx = 0; bufIdx < numBuffers; bufIdx++) {
        int16_t* src = (int16_t*)srcBuffers[bufIdx];
        for (size_t i = 0; i < samples; i++) {
            int32_t mix = (int32_t)dest[i] + (int32_t)src[i];
            if (mix > RESOLUTION - 1) mix = RESOLUTION - 1;
            else if (mix < -RESOLUTION) mix = -RESOLUTION;
            dest[i] = (int16_t)mix;
        }
    }
}

void handleSerialMidi() {
    while (Serial.available() >= 2) {
        uint8_t cmd = Serial.read();
        uint8_t type = cmd & 0xF0;
        uint8_t channel = cmd & 0x0F;

        if (channel >= MAX_CHANNELS) continue;

        if (type == 0xC0) {
            // Program Change (2 bytes apenas)
            if (Serial.available() < 1) return;
            uint8_t program = Serial.read();
            uint8_t instrument = INSTR_SNARE;
            if (program < sizeof(programToInstrument)) {
                instrument = programToInstrument[program];
            }
            synths[channel]->changeInstrument(instrument);
            Serial.printf("Canal %d → Programa %d → Instrumento %d\n", channel, program, instrument);
        } else {
            if (Serial.available() < 2) return;
            uint8_t data1 = Serial.read();
            uint8_t data2 = Serial.read();

            if (type == 0x90 && data2 > 0) {
                synths[channel]->noteOn(data1, data2);
            } else if (type == 0x80 || (type == 0x90 && data2 == 0)) {
                synths[channel]->noteOff(data1);
            }
        }
    }
}

void taskReadMidi(void* pvParameters) {
    for (;;) {
        handleSerialMidi();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void taskAudioWrite(void* pvParameters) {
    for (;;) {
        for (int i = 0; i < MAX_CHANNELS; i++) {
            synths[i]->read(buffers[i], BUFFER_SIZE);
        }

        mixBuffers(mixedBuffer, buffers, MAX_CHANNELS, BUFFER_SIZE);
        kit.write((uint8_t*)mixedBuffer, BUFFER_SIZE);
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    setCpuFrequencyMhz(240);
    delay(1000);
    Serial.println("Iniciando PolySynth com MIDI serial e até 12 canais");
    WaveformTable::init();
    auto cfg = kit.defaultConfig(KitOutput);
    kit.begin(cfg);

    for (int i = 0; i < MAX_CHANNELS; i++) {
        buffers[i] = new uint8_t[BUFFER_SIZE];
    }

    if (psramFound()) {
        Serial.println("PSRAM disponível!");
    } else {
        Serial.println("PSRAM NÃO encontrada!");
    }

    // Inicializa cada canal com um instrumento padrão
    for (int i = 0; i < MAX_CHANNELS; i++) {
        uint8_t defaultInstrument = (i < sizeof(programToInstrument)) ? programToInstrument[i] : INSTR_SINE;
        synths[i] = synths[i] = new PolySynth(static_cast<InstrumentType>(defaultInstrument));

        synths[i]->begin(cfg.sampleRate());
    }

    xTaskCreatePinnedToCore(taskReadMidi, "ReadMidi", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskAudioWrite, "AudioWrite", 4096, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(100));
}
