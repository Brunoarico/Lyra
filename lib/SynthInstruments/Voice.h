#ifndef VOICE_H
#define VOICE_H

#include <stdint.h>

class Voice {
public:
    virtual void setSampleRate(uint32_t sr) = 0;
    virtual void noteOn(uint8_t note, uint8_t velocity) = 0;
    virtual void noteOff() = 0;
    virtual float readSample() = 0;
    virtual bool isActive() = 0;
    virtual void resetPhase() = 0; 
    virtual ~Voice() {}
};

#endif // VOICE_H