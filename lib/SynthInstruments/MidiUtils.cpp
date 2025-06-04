#include "MidiUtils.h"
#include <cmath>

float noteToFreq(uint8_t note) {
    return 440.0f * powf(2.0f, float(note - 69) / 12.0f);
}

float amplitudeNormalized(uint8_t velocity) {
    return velocity / 127.0f; // Normaliza de 0-127 para 0.0-1.0
}
