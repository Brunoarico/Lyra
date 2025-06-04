#include "Effect.h"
#include <vector>

class DelayEffect : public Effect {
public:
    DelayEffect(float sr, float delayMs = 300.0f, float feedback = 0.5f)
        : sampleRate(sr), feedback(feedback) {
        setDelayTime(delayMs);
    }

    void setDelayTime(float ms) {
        delaySamples = (int)(sampleRate * ms / 1000.0f);
        buffer.resize(delaySamples, 0.0f);
        index = 0;
    }

    float process(float input) override {
        float delayed = buffer[index];
        buffer[index] = input + delayed * feedback;

        index = (index + 1) % delaySamples;
        return delayed;
    }

    void reset() override {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
    }

private:
    std::vector<float> buffer;
    int index = 0;
    int delaySamples;
    float sampleRate;
    float feedback;
};
