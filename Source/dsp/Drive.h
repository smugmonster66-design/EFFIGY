#pragma once
#include <juce_dsp/juce_dsp.h>
#include "FeelEngine.h"

namespace effigy
{
    // Stage 3: five original drive circuits + feel engine, 4x oversampled nonlinearity.
    class Drive
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();
        void process(float* data, int numSamples);

        void setCircuit(int c);
        void setParams(float drive01_100, float levelDb, float character01_100)
        {
            driveAmt = drive01_100; levelDb_ = levelDb; character = character01_100;
        }

        FeelEngine& feel() { return feelEngine; }

        int getLatencySamples() const { return oversampler ? (int) oversampler->getLatencyInSamples() : 0; }
        bool oversamplingEnabled() const { return osEnabled; }
        void setOversampling(bool on) { osEnabled = on; }

    private:
        float shape(float x) const;      // circuit waveshaper
        void updateFilters();

        double sampleRate = 44100.0;
        int    circuit = 0;
        float  driveAmt = 25.f, levelDb_ = 0.f, character = 50.f;
        bool   osEnabled = true;

        FeelEngine feelEngine;

        std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
        juce::dsp::IIR::Filter<float> preHPF, preTilt, postLPF, postPresence;
        float lastCircuit = -1.f;
    };
}
