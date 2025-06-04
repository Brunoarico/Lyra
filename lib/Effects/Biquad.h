#ifndef BIQUAD_FILTER_H
#define BIQUAD_FILTER_H

#include <math.h>

enum FilterType {
    LOW_PASS,
    HIGH_PASS,
    BAND_PASS
};

class BiquadFilter {
public:
    BiquadFilter(FilterType type, float freq, float q, float sampleRate)
        : type(type), freq(freq), q(q), sampleRate(sampleRate) {
        reset();
        updateCoefficients();
    }

    void setParams(FilterType type, float freq, float q) {
        this->type = type;
        this->freq = freq;
        this->q = q;
        updateCoefficients();
    }

    void setSampleRate(float sr) {
        sampleRate = sr;
        updateCoefficients();
    }

    float process(float input) {
        float output = a0 * input + a1 * x1 + a2 * x2
                       - b1 * y1 - b2 * y2;

        // Shift delay line
        x2 = x1;
        x1 = input;
        y2 = y1;
        y1 = output;

        return output;
    }

    void reset() {
        x1 = x2 = y1 = y2 = 0.0f;
    }

private:
    FilterType type;
    float freq;
    float q;
    float sampleRate;

    // Coefficients
    float a0, a1, a2, b1, b2;

    // State
    float x1, x2, y1, y2;

    void updateCoefficients() {
        float w0 = 2.0f * M_PI * freq / sampleRate;
        float alpha = sinf(w0) / (2.0f * q);
        float cos_w0 = cosf(w0);

        switch (type) {
            case LOW_PASS:
                a0 = (1 - cos_w0) / 2;
                a1 = 1 - cos_w0;
                a2 = (1 - cos_w0) / 2;
                b1 = -2 * cos_w0;
                b2 = 1 - alpha;
                break;

            case HIGH_PASS:
                a0 = (1 + cos_w0) / 2;
                a1 = -(1 + cos_w0);
                a2 = (1 + cos_w0) / 2;
                b1 = -2 * cos_w0;
                b2 = 1 - alpha;
                break;

            case BAND_PASS:
                a0 = alpha;
                a1 = 0;
                a2 = -alpha;
                b1 = -2 * cos_w0;
                b2 = 1 - alpha;
                break;
        }

        // Normalize
        float a0_inv = 1.0f / (1 + alpha);
        a0 *= a0_inv;
        a1 *= a0_inv;
        a2 *= a0_inv;
        b1 *= a0_inv;
        b2 *= a0_inv;
    }
};

#endif
