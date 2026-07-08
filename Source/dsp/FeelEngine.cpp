#include "FeelEngine.h"

namespace effigy
{
    void FeelEngine::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        reset();
    }

    void FeelEngine::reset()
    {
        fastEnv = sagState = slowEnv = 0.f;
    }

    void FeelEngine::setCircuit(int circuit)
    {
        // vintage circuits sag deeper/slower; modern shallow/fast.
        switch (circuit)
        {
            case 0: circuitSagDepth = 0.6f; circuitSagRec = 1.0f; circuitBloom = 0.7f; break; // WAX
            case 1: circuitSagDepth = 1.0f; circuitSagRec = 1.4f; circuitBloom = 1.2f; break; // STRAW
            case 2: circuitSagDepth = 0.9f; circuitSagRec = 1.2f; circuitBloom = 1.0f; break; // WICKER
            case 3: circuitSagDepth = 0.5f; circuitSagRec = 0.7f; circuitBloom = 0.6f; break; // BRONZE
            case 4: circuitSagDepth = 0.35f;circuitSagRec = 0.5f; circuitBloom = 0.4f; break; // ASH
            default: circuitSagDepth = circuitSagRec = circuitBloom = 1.f; break;
        }
    }

    FeelEngine::Mod FeelEngine::process(float inputMag)
    {
        const float atk = std::exp(-1.0f / (0.001f * 2.0f * (float) sampleRate));
        fastEnv = inputMag > fastEnv ? atk * fastEnv + (1.f - atk) * inputMag
                                     : 0.9995f * fastEnv;
        const float slowA = std::exp(-1.0f / (0.001f * 50.0f * (float) sampleRate));
        slowEnv = slowA * slowEnv + (1.f - slowA) * inputMag;

        Mod m;
        if (feel <= 0.0001f)
            return m;

        // --- Sag: transient spike drives a momentary dip, recovering ---
        const float transient = juce::jmax(0.f, fastEnv - slowEnv);
        const float recMs = juce::jmax(10.f, sagRecMs * circuitSagRec);
        const float recCoef = std::exp(-1.0f / (0.001f * recMs * (float) sampleRate));
        const float sagDepth = feel * sagDepthUser * circuitSagDepth;
        // charge sag on transient, discharge (recover) exponentially
        sagState = juce::jmax(sagState * recCoef, juce::jlimit(0.f, 1.f, transient * 6.f));
        m.driveMult = 1.f - sagDepth * 0.45f * sagState;
        m.biasShift = sagDepth * 0.15f * sagState;

        // --- Bloom: below the knee, reduce effective drive (cleanup) ---
        const float lvlDb = juce::Decibels::gainToDecibels(slowEnv + 1.0e-6f);
        const float knee = bloomKneeDb;
        if (lvlDb < knee)
        {
            const float below = juce::jlimit(0.f, 1.f, (knee - lvlDb) / 24.f);
            const float bloom = feel * circuitBloom * below;
            m.driveMult *= (1.f - 0.6f * bloom);
        }
        return m;
    }
}
