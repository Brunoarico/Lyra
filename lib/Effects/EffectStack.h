#ifndef EFFECT_STACK_H
#define EFFECT_STACK_H

#include <vector>
#include "Effect.h"

class EffectStack {
public:
    void addEffect(Effect* effect) {
        effects.push_back(effect);
    }

    void clear() {
        effects.clear();
    }

    void reset() {
        for (auto* e : effects)
            e->reset();
    }

    float process(float input) {
        float output = input;
        for (auto* e : effects)
            output = e->process(output);
        return output;
    }

private:
    std::vector<Effect*> effects;
};

#endif
