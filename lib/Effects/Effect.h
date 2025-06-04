#ifndef EFFECT_H
#define EFFECT_H

class Effect {
public:
    virtual ~Effect() {}
    virtual float process(float input) = 0;
    virtual void reset() {}
};

#endif