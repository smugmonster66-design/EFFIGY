#pragma once
#include <juce_dsp/juce_dsp.h>

namespace effigy
{
    // Stage 6: Produce chain. Serial: Gloss -> Doubler -> Width -> Room -> Tilt.
    class Produce
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& stereoSpec);
        void reset();
        void process(float* left, float* right, int numSamples);

        void setGloss(bool on, float amount01)          { glossOn = on; glossAmt = amount01; }
        void setDoubler(bool on, float amt01, float spread01) { dblOn = on; dblAmt = amt01; dblSpread = spread01; }
        void setWidth(bool on, float width01_150)       { widthOn = on; widthAmt = width01_150; }
        void setRoom(bool on, float mix01, float size01) { roomOn = on; roomMix = mix01; roomSize = size01; roomDirty = true; }
        void setTilt(bool on, float tiltDb)             { tiltOn = on; tiltDbV = tiltDb; tiltDirty = true; }

    private:
        double sampleRate = 44100.0;

        // Gloss
        bool  glossOn = false; float glossAmt = 0.4f;
        float glossEnv = 0.f;

        // Doubler
        bool  dblOn = false; float dblAmt = 0.5f, dblSpread = 0.6f;
        juce::dsp::DelayLine<float> delayL { 4096 }, delayR { 4096 };
        float lfoPhaseL = 0.f, lfoPhaseR = 0.37f;
        float driftL = 0.f, driftR = 0.f;
        juce::Random rng;

        // Width
        bool  widthOn = false; float widthAmt = 100.f;

        // Room
        bool  roomOn = false; float roomMix = 0.2f, roomSize = 0.4f; bool roomDirty = true;
        juce::dsp::Reverb reverb;

        // Tilt
        bool  tiltOn = false; float tiltDbV = 0.f; bool tiltDirty = true;
        juce::dsp::IIR::Filter<float> tiltLoL, tiltLoR, tiltHiL, tiltHiR;
    };
}
