class Envelope {
public:
    float attackTime = 0.05f;   // segundos
    float decayTime = 0.05f;    // segundos
    float sustainLevel = 0.3f;  // 0.0 a 1.0
    float releaseTime = 0.5f;   // segundos

    enum State { OFF, ATTACK, DECAY, SUSTAIN, RELEASE };
    State state = OFF;

    float value = 0.0f;
    float sampleRate = 44100.0f;
    float releaseStart = 0.0f;

    void setSampleRate(float sr) {
        sampleRate = sr;
    }

    void noteOn() {
        state = ATTACK;
    }

    void noteOff() {
        state = RELEASE;
        releaseStart = value;
    }

    float next() {
        float delta = 1.0f / sampleRate;

        switch (state) {
            case ATTACK:
                value += delta / attackTime;
                if (value >= 1.0f) {
                    value = 1.0f;
                    state = DECAY;
                }
                break;
            case DECAY:
                value -= delta * (1.0f - sustainLevel) / decayTime;
                if (value <= sustainLevel) {
                    value = sustainLevel;
                    state = SUSTAIN;
                }
                break;
            case SUSTAIN:
                // mantém o nível
                break;
            case RELEASE:
                value -= delta * releaseStart / releaseTime;
                if (value <= 0.0f) {
                    value = 0.0f;
                    state = OFF;
                }
                break;
            case OFF:
                value = 0.0f;
                break;
        }

        return value;
    }

    bool isActive() {
        return state != OFF;
    }
};
