#pragma once
#include <juce_dsp/juce_dsp.h>

namespace effigy
{
    // Envelope-driven modulation feeding the Drive stage. Per-circuit tuned.
    class FeelEngine
    {
    public:
        struct Mod { float driveMult = 1.f; float biasShift = 0.f; };

        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();

        // Per-circuit ranges (0..4). Sets base depth/recovery scaling.
        void setCircuit(int circuit);

        // User controls.
        void setFeel(float macro01)        { feel = macro01; }
        void setSag(float depth01, float recoveryMs) { sagDepthUser = depth01; sagRecMs = recoveryMs; }
        void setBloomKnee(float kneeDb)    { bloomKneeDb = kneeDb; }

        // Called per sample with the (pre-drive) input magnitude.
        Mod process(float inputMag);

    private:
        double sampleRate = 44100.0;
        float feel = 0.4f;
        float sagDepthUser = 0.5f, sagRecMs = 120.f, bloomKneeDb = -30.f;

        // per-circuit scalers
        float circuitSagDepth = 1.f, circuitSagRec = 1.f, circuitBloom = 1.f;

        float fastEnv = 0.f, sagState = 0.f, slowEnv = 0.f;
    };
}
