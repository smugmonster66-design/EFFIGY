#pragma once
#include <juce_dsp/juce_dsp.h>

namespace effigy
{
    // Stage 5: dual-slot convolution cab. Mono in -> stereo out (stereo begins here).
    class CabSection
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& stereoSpec);
        void reset();

        // monoIn: numSamples mono. Writes stereo result into outL/outR.
        void process(const float* monoIn, float* outL, float* outR, int numSamples);

        void setBypass(bool b)     { bypassed = b; }
        void setBlend(float b01)   { blend = b01; }
        void setLevels(float aDb, float bDb) { levelA = aDb; levelB = bDb; }
        void setPans(float aPan, float bPan) { panA = aPan; panB = bPan; }
        void setCuts(float lowHz, float highHz);

        // IR selection. factoryIndex >= 0 loads factory; -1 = empty slot.
        void loadFactoryA(int factoryIndex);
        void loadFactoryB(int factoryIndex);
        void loadUserA(const juce::File& f);
        void loadUserB(const juce::File& f);

    private:
        double sampleRate = 44100.0;
        juce::uint32 blockSize = 512;

        juce::dsp::Convolution convA, convB;
        bool loadedA = false, loadedB = false;
        int  currentA = -2, currentB = -2;

        bool  bypassed = false;
        float blend = 0.5f, levelA = 0.f, levelB = 0.f, panA = -0.4f, panB = 0.4f;

        juce::AudioBuffer<float> scratchA, scratchB;

        juce::dsp::IIR::Filter<float> lowCutL, lowCutR, highCutL, highCutR;
        float lowHzCur = 70.f, highHzCur = 15000.f;
        bool cutsDirty = true;
    };
}
