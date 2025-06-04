#include "Effect.h"
#include "Biquad.h"

class FilterEffect : public Effect {
public:
    FilterEffect(FilterType type, float freq, float q, float sr)
        : filter(type, freq, q, sr) {}

    float process(float input) override {
        return filter.process(input);
    }

    void reset() override {
        filter.reset();
    }

    void setParams(FilterType type, float freq, float q) {
        filter.setParams(type, freq, q);
    }

private:
    BiquadFilter filter;
};
