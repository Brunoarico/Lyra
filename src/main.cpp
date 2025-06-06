#include <Arduino.h>
#include <AudioKitHAL.h>
#include "PolySynth.h"
#include "Constants.h"
#include "WaveformTable.h"

AudioKit kit;

// Array de ponteiros para PolySynth (um por canal MIDI)
PolySynth* synths[NUM_CHANNELS];

// Buffers para cada synth e buffer final mixado
uint8_t* buffers[NUM_CHANNELS];
int16_t mixedBuffer[BUFFER_SIZE / 2]; // 16-bit samples, stereo = BUFFER_SIZE/2 samples

// Função para misturar buffers de saída (16-bit stereo)
void mixBuffers(int16_t* dest, uint8_t** srcBuffers, size_t numBuffers, size_t bytes) {
    // Zera o destino
    memset(dest, 0, bytes);

    size_t samples = bytes / 2; // 2 bytes por amostra (int16_t)

    for (size_t bufIdx = 0; bufIdx < numBuffers; bufIdx++) {
        int16_t* src = (int16_t*)srcBuffers[bufIdx];
        for (size_t i = 0; i < samples; i++) {
            int32_t mix = (int32_t)dest[i] + (int32_t)src[i];
            if (mix > RESOLUTION-1) mix = RESOLUTION-1;
            else if (mix < -RESOLUTION) mix = -RESOLUTION;
            dest[i] = (int16_t)mix;
        }
    }
}

void handleSerialMidi() {
    while (Serial.available() >= 3) {
        uint8_t cmd = Serial.read();
        uint8_t note = Serial.read();
        uint8_t vel  = Serial.read();

        uint8_t channel = cmd & 0x0F;
        uint8_t type = cmd & 0xF0;

        if (channel >= NUM_CHANNELS) {
            continue; // Ignora canal inválido
        }

        // Debug básico
        /*Serial.printf("Canal: %d - Nota: %d - Vel: %d - Instrumento: %d\n",
                      channel, note, vel, synths[channel]->instrumentId());*/

        // Note On
        if (type == 0x90 && vel > 0) {
            synths[channel]->noteOn(note, vel);
        }
        // Note Off (inclui Note On com velocity zero)
        else if (type == 0x80 || (type == 0x90 && vel == 0)) {
            synths[channel]->noteOff(note);
        }
        // Polyphonic Key Pressure (Aftertouch)
        /*else if (type == 0xA0) {
            synths[channel]->aftertouch(note, vel); // Implementar se necessário
        }
        // Control Change
        else if (type == 0xB0) {
            synths[channel]->controlChange(note, vel); // note = controller, vel = value
        }
        // Program Change (usa apenas 2 bytes, precisamos tratar isso)
        else if (type == 0xC0) {
            synths[channel]->programChange(note); // note aqui é o número do programa
            // descarta 1 byte extra
            vel = Serial.read();
        }
        // Channel Pressure (monofônico) – também usa 2 bytes
        else if (type == 0xD0) {
            synths[channel]->channelPressure(note); // note = pressure value
            // descarta 1 byte extra
            vel = Serial.read();
        }
        // Pitch Bend Change
        else if (type == 0xE0) {
            uint16_t pitch = (vel << 7) | note; // valor de 14 bits
            synths[channel]->pitchBend(pitch);
        }
        */
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
        // Lê áudio de todos os synths para seus buffers
        for (int i = 0; i < NUM_CHANNELS; i++) {
            synths[i]->read(buffers[i], BUFFER_SIZE);
        }

        // Mistura todos os buffers em mixedBuffer
        mixBuffers(mixedBuffer, buffers, NUM_CHANNELS, BUFFER_SIZE);

        // Envia buffer mixado para o AudioKit
        kit.write((uint8_t*)mixedBuffer, BUFFER_SIZE);
    }
}

void setup() {
    Serial.begin(BAUD_RATE);
    setCpuFrequencyMhz(240);
    delay(1000);
    Serial.println("Iniciando PolySynth com MIDI serial e 8 canais");
    WaveformTable::init();
    auto cfg = kit.defaultConfig(KitOutput);
    kit.begin(cfg);

    // Aloca os buffers e inicializa cada synth
    for (int i = 0; i < NUM_CHANNELS; i++) {
        buffers[i] = new uint8_t[BUFFER_SIZE];
    }

    if (psramFound()) {
        Serial.println("PSRAM disponível!");
    } else {
        Serial.println("PSRAM NÃO encontrada!");
    }

    // Inicializa synths com instrumentos diferentes para exemplo
    synths[0] = new PolySynth(INSTR_KARPLUS);
    synths[1] = new PolySynth(INSTR_SINE);
    synths[2] = new PolySynth(INSTR_SQUARE);
    synths[3] = new PolySynth(INSTR_KARPLUS);
    synths[4] = new PolySynth(INSTR_SINE);
    synths[5] = new PolySynth(INSTR_SAWTOOTH);
    synths[6] = new PolySynth(INSTR_SNARE);
    synths[7] = new PolySynth(INSTR_SINE);
    synths[8] = new PolySynth(INSTR_SINE);
    synths[9] = new PolySynth(INSTR_SINE);
    synths[10] = new PolySynth(INSTR_SINE);
    synths[11] = new PolySynth(INSTR_SNARE);
    synths[12] = new PolySynth(INSTR_FM);
    
    // Inicia cada synth
    for (int i = 0; i < NUM_CHANNELS; i++) {
        synths[i]->begin(cfg.sampleRate());
    }

    xTaskCreatePinnedToCore(taskReadMidi, "ReadMidi", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(taskAudioWrite, "AudioWrite", 4096, NULL, 1, NULL, 1);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(100));
}