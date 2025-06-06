#include "WaveformTable.h"

float WaveformTable::sineTable[WAVEFORM_TABLE_SIZE];
float WaveformTable::squareTable[WAVEFORM_TABLE_SIZE];
float WaveformTable::triangleTable[WAVEFORM_TABLE_SIZE];
float WaveformTable::sawTable[WAVEFORM_TABLE_SIZE];

void WaveformTable::init() {
    for (int i = 0; i < WAVEFORM_TABLE_SIZE; ++i) {
        float t = (float)i / WAVEFORM_TABLE_SIZE;
        sineTable[i] = sinf(2.0f * M_PI * t);
        squareTable[i] = (t < 0.5f) ? 1.0f : -1.0f;
        triangleTable[i] = 4.0f * fabsf(t - 0.5f) - 1.0f;
        sawTable[i] = 2.0f * t - 1.0f;
    }
}

float WaveformTable::getSample(WaveformType type, float phase) {
    int idx = (int)(phase * WAVEFORM_TABLE_SIZE) % WAVEFORM_TABLE_SIZE;
    return 
        type == SINE ? sineTable[idx] :
        type == SQUARE ? squareTable[idx] :
        type == TRIANGLE ? triangleTable[idx] :
        type == SAWTOOTH ? sawTable[idx] : 0.0f;
}
