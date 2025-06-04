#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <stdint.h>
#include <math.h>

class MidiHandler {
public:
    struct MidiNote {
        uint8_t channel;
        uint8_t note;
        uint8_t velocity;
        bool isNoteOn;
    };

    MidiHandler() {}

    // Chamada para processar um byte MIDI por vez
    bool processByte(uint8_t byte, MidiNote& outNote) {
        if (byte & 0x80) {
            status = byte;
            byteIndex = 0;
            return false;
        }

        data[byteIndex++] = byte;

        // Note On/Off espera dois bytes: note number e velocity
        if ((status & 0xF0) == 0x90 && byteIndex == 2) { // Note On
            outNote.channel = status & 0x0F;
            outNote.note = data[0];
            outNote.velocity = data[1];
            outNote.isNoteOn = data[1] > 0;
            byteIndex = 0;
            return true;
        }

        if ((status & 0xF0) == 0x80 && byteIndex == 2) { // Note Off
            outNote.channel = status & 0x0F;
            outNote.note = data[0];
            outNote.velocity = data[1];
            outNote.isNoteOn = false;
            byteIndex = 0;
            return true;
        }

        return false;
    }

    // Converte número de nota MIDI para frequência (A440)
    static float noteToFrequency(uint8_t note) {
        return 440.0f * powf(2.0f, (note - 69) / 12.0f);
    }

private:
    uint8_t status = 0;
    uint8_t data[2];
    uint8_t byteIndex = 0;
};

#endif // MIDIHANDLER_H