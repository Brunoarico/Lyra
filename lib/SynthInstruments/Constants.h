#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <cmath>


#define NUM_CHANNELS 13  // Number of PolySynth instances
#define MAX_VOICES 8     // Maximum voices per PolySynth

#define MAX_HARMONICS 8  // Maximum harmonics for additive synthesis

#define RESOLUTION 32768.0f // 16-bit audio
#define BAUD_RATE 500000 // Serial communication speed
#define BUFFER_SIZE 1024 // Buffer size for audio processing

enum InstrumentType {
    INSTR_FM,
    INSTR_SNARE,
    INSTR_SINE,
    INSTR_SQUARE,
    INSTR_SAWTOOTH,
    INSTR_TRIANGLE,
    INSTR_KARPLUS,
    INSTR_ADDITIVE
};

#endif // CONSTANTS_H