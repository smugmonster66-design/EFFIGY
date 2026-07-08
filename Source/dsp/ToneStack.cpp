#include "ToneStack.h"

namespace effigy
{
    // Per-circuit centre frequencies { bassHz, trebleHz, presenceHz, midQ }
    struct Voicing { double bassHz, trebleHz, presenceHz; float midQ; };
    static const Voicing voicings[5] = {
        { 100.0, 3500.0, 5200.0, 0.7f }, // WAX
        {  90.0, 3200.0, 4800.0, 0.8f }, // STRAW
        { 110.0, 3000.0, 4600.0, 0.9f }, // WICKER
        { 120.0, 2800.0, 4400.0, 1.0f }, // BRONZE
        { 140.0, 2600.0, 4200.0, 1.1f }, // ASH
    };

    void ToneStack::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        juce::dsp::ProcessSpec mono { spec.sampleRate, spec.maximumBlockSize, 1 };
        bassF.prepare(mono); midF.prepare(mono); trebleF.prepare(mono); presenceF.prepare(mono);
        dirty = true;
        update();
        reset();
    }

    void ToneStack::reset()
    {
        bassF.reset(); midF.reset(); trebleF.reset(); presenceF.reset();
    }

    void ToneStack::update()
    {
        if (! dirty) return;
        dirty = false;
        const auto& v = voicings[circuit];
        using C = juce::dsp::IIR::Coefficients<float>;
        bassF.coefficients     = C::makeLowShelf (sampleRate, v.bassHz, 0.7f, juce::Decibels::decibelsToGain(bass));
        midF.coefficients      = C::makePeakFilter(sampleRate,
                                     juce::jlimit(200.0, 2000.0, (double) midCentre), v.midQ,
                                     juce::Decibels::decibelsToGain(mid));
        trebleF.coefficients   = C::makeHighShelf(sampleRate, v.trebleHz, 0.7f, juce::Decibels::decibelsToGain(treble));
        presenceF.coefficients = C::makePeakFilter(sampleRate, v.presenceHz, 0.7f, juce::Decibels::decibelsToGain(presence));
    }

    void ToneStack::process(float* data, int numSamples)
    {
        update();
        for (int i = 0; i < numSamples; ++i)
        {
            float x = bassF.processSample(data[i]);
            x = midF.processSample(x);
            x = trebleF.processSample(x);
            x = presenceF.processSample(x);
            data[i] = x;
        }
    }
}
