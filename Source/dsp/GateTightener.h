#pragma once
#include <juce_dsp/juce_dsp.h>

namespace effigy
{
    // Stage 2: integrated gate + tightener sharing one envelope detector.
    class GateTightener
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();
        void process(float* data, int numSamples);

        void setTight(float freqHz, float reso) { tightFreq = freqHz; tightReso = reso; }
        void setGate(bool on, float threshDb, float releaseMs)
        {
            gateOn = on; gateThreshDb = threshDb; gateReleaseMs = releaseMs;
        }

    private:
        void updateTight();

        double sampleRate = 44100.0;
        float tightFreq = 40.f, tightReso = 0.9f, lastFreq = -1.f, lastReso = -1.f;
        bool  gateOn = false;
        float gateThreshDb = -60.f, gateReleaseMs = 120.f;

        juce::dsp::IIR::Filter<float> tightHPF;

        // shared detector
        float env = 0.f;        // fast envelope (transient/density)
        float slowEnv = 0.f;    // slow envelope (gate level)
        float gateGain = 0.f;   // smoothed gate multiplier
    };
}
