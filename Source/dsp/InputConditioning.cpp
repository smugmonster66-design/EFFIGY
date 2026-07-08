#include "InputConditioning.h"

namespace effigy
{
    void InputConditioning::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        juce::dsp::ProcessSpec mono { spec.sampleRate, spec.maximumBlockSize, 1 };
        subsonic.prepare(mono);
        lowShelf.prepare(mono);
        highShelf.prepare(mono);
        subsonic.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 25.0);
        lastTilt = -999.f;
        updateTilt();
        reset();
    }

    void InputConditioning::reset()
    {
        subsonic.reset();
        lowShelf.reset();
        highShelf.reset();
    }

    void InputConditioning::updateTilt()
    {
        if (juce::approximatelyEqual(tiltDb, lastTilt))
            return;
        lastTilt = tiltDb;
        // Tilt around 800 Hz: low shelf +t, high shelf -t (t = tiltDb, negative tilts bright->dark)
        const double pivot = 800.0;
        const float g = tiltDb;
        lowShelf.coefficients  = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            sampleRate, pivot, 0.5f, juce::Decibels::decibelsToGain(g));
        highShelf.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            sampleRate, pivot, 0.5f, juce::Decibels::decibelsToGain(-g));
    }

    void InputConditioning::process(float* data, int numSamples)
    {
        updateTilt();
        const float lin = juce::Decibels::decibelsToGain(gainDb);
        for (int i = 0; i < numSamples; ++i)
        {
            float x = data[i] * lin;
            x = subsonic.processSample(x);
            x = lowShelf.processSample(x);
            x = highShelf.processSample(x);
            data[i] = x;
        }
    }
}
