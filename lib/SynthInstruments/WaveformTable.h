#ifndef WAVEFORM_TABLE_H
#define WAVEFORM_TABLE_H

#include <math.h>
#include <stdint.h>

#define WAVEFORM_TABLE_SIZE 1024

enum WaveformType {
    SINE,
    SQUARE,
    TRIANGLE,
    SAWTOOTH
};

class WaveformTable {
public:
    static void init();
    static float getSample(WaveformType type, float phase);

private:
    static float sineTable[WAVEFORM_TABLE_SIZE];
    static float squareTable[WAVEFORM_TABLE_SIZE];
    static float triangleTable[WAVEFORM_TABLE_SIZE];
    static float sawTable[WAVEFORM_TABLE_SIZE];
};

#endif
