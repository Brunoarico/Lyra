#ifndef MIDIUTILS_H
#define MIDIUTILS_H

#include <stdint.h>

float noteToFreq(uint8_t note);
float amplitudeNormalized(uint8_t velocity);

#endif