#pragma once
#include <juce_dsp/juce_dsp.h>

namespace effigy
{
    // Stage 1: Input gain, pickup tilt (~800 Hz pivot, +/-3 dB), 25 Hz subsonic HPF.
    class InputConditioning
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();
        // Mono in-place processing on a single channel pointer.
        void process(float* data, int numSamples);

        void setInputGainDb(float db)   { gainDb = db; }
        void setTiltDb(float db)        { tiltDb = db; }

    private:
        void updateTilt();

        double sampleRate = 44100.0;
        float  gainDb = 0.f, tiltDb = 0.f, lastTilt = -999.f;

        juce::dsp::IIR::Filter<float> subsonic;   // 25 Hz HPF
        juce::dsp::IIR::Filter<float> lowShelf;    // tilt low
        juce::dsp::IIR::Filter<float> highShelf;   // tilt high
    };
}
