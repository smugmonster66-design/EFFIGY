#include "Drive.h"

namespace effigy
{
    void Drive::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        oversampler = std::make_unique<juce::dsp::Oversampling<float>>(
            1, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
        oversampler->initProcessing(spec.maximumBlockSize);
        oversampler->reset();

        juce::dsp::ProcessSpec mono { spec.sampleRate, spec.maximumBlockSize, 1 };
        preHPF.prepare(mono);
        preTilt.prepare(mono);
        postLPF.prepare(mono);
        postPresence.prepare(mono);

        lastCircuit = -1.f;
        feelEngine.prepare(spec);
        updateFilters();
        reset();
    }

    void Drive::reset()
    {
        if (oversampler) oversampler->reset();
        preHPF.reset(); preTilt.reset(); postLPF.reset(); postPresence.reset();
        feelEngine.reset();
    }

    void Drive::setCircuit(int c)
    {
        circuit = juce::jlimit(0, 4, c);
        feelEngine.setCircuit(circuit);
    }

    void Drive::updateFilters()
    {
        if (juce::approximatelyEqual((float) circuit, lastCircuit))
            return;
        lastCircuit = (float) circuit;
        auto& C = juce::dsp::IIR::Coefficients<float>::makeHighPass;
        auto& LP = juce::dsp::IIR::Coefficients<float>::makeLowPass;
        auto& PK = juce::dsp::IIR::Coefficients<float>::makePeakFilter;

        switch (circuit)
        {
            case 0: // WAX - clean/warm, gentle top roll
                preHPF.coefficients  = C(sampleRate, 30.0);
                preTilt.coefficients = PK(sampleRate, 500.0, 0.7f, 1.05f);
                postLPF.coefficients = LP(sampleRate, 12000.0);
                postPresence.coefficients = PK(sampleRate, 3000.0, 0.7f, 1.0f);
                break;
            case 1: // STRAW - loose low, lively
                preHPF.coefficients  = C(sampleRate, 45.0);
                preTilt.coefficients = PK(sampleRate, 800.0, 0.6f, 1.1f);
                postLPF.coefficients = LP(sampleRate, 9000.0);
                postPresence.coefficients = PK(sampleRate, 2600.0, 0.8f, 1.1f);
                break;
            case 2: // WICKER - midrange complex crunch
                preHPF.coefficients  = C(sampleRate, 70.0);
                preTilt.coefficients = PK(sampleRate, 1000.0, 0.9f, 1.25f);
                postLPF.coefficients = LP(sampleRate, 7000.0);
                postPresence.coefficients = PK(sampleRate, 2800.0, 0.9f, 1.15f);
                break;
            case 3: // BRONZE - tight modern, firm low
                preHPF.coefficients  = C(sampleRate, 95.0);
                preTilt.coefficients = PK(sampleRate, 1200.0, 1.0f, 0.9f);
                postLPF.coefficients = LP(sampleRate, 6000.0);
                postPresence.coefficients = PK(sampleRate, 3200.0, 1.0f, 1.2f);
                break;
            case 4: // ASH - extreme, fizz management post
                preHPF.coefficients  = C(sampleRate, 120.0);
                preTilt.coefficients = PK(sampleRate, 1400.0, 1.1f, 0.85f);
                postLPF.coefficients = LP(sampleRate, 5000.0);       // built-in fizz control
                postPresence.coefficients = PK(sampleRate, 3500.0, 1.1f, 1.15f);
                break;
        }
    }

    float Drive::shape(float x) const
    {
        const float ch = character * 0.01f;   // 0..1
        switch (circuit)
        {
            case 0: // WAX: soft asymmetric, generous headroom, bias = character
            {
                const float bias = (ch - 0.5f) * 0.4f;
                return std::tanh(x * 0.8f + bias) - std::tanh(bias);
            }
            case 1: // STRAW: touch-sensitive soft clip, attack looseness = character
            {
                const float k = 1.0f + ch * 2.0f;
                float y = x / (1.0f + std::abs(x * k));
                return y * 1.2f;
            }
            case 2: // WICKER: chewy symmetric-ish, blend tanh + cubic by character
            {
                const float t = std::tanh(x * 1.4f);
                const float c = x - (x * x * x) / 3.0f;
                return juce::jlimit(-1.5f, 1.5f, (1.f - ch) * t + ch * c);
            }
            case 3: // BRONZE: cascaded stages, stage balance = character
            {
                const float s1 = std::tanh(x * 1.6f);
                const float s2 = std::tanh(s1 * (1.2f + ch * 1.5f));
                return s2;
            }
            case 4: // ASH: high gain hard-ish clip, clarity retained
            {
                const float k = 2.0f + ch * 3.0f;
                float y = std::tanh(x * k);
                // subtle even-harmonic asymmetry for body
                y += 0.05f * (y * y);
                return juce::jlimit(-1.2f, 1.2f, y);
            }
        }
        return x;
    }

    void Drive::process(float* data, int numSamples)
    {
        updateFilters();

        const float driveGain = juce::Decibels::decibelsToGain(juce::jmap(driveAmt, 0.f, 100.f, -6.f, 36.f));
        const float outGain   = juce::Decibels::decibelsToGain(levelDb_);

        // Pre-filter + feel modulation at base rate.
        for (int i = 0; i < numSamples; ++i)
        {
            float x = preHPF.processSample(data[i]);
            x = preTilt.processSample(x);
            const auto mod = feelEngine.process(std::abs(x));
            data[i] = x * driveGain * mod.driveMult + mod.biasShift;
        }

        // Oversampled nonlinearity.
        juce::dsp::AudioBlock<float> block(&data, 1, (size_t) numSamples);
        if (osEnabled)
        {
            auto up = oversampler->processSamplesUp(block);
            for (size_t i = 0; i < up.getNumSamples(); ++i)
                up.setSample(0, (int) i, shape(up.getSample(0, (int) i)));
            oversampler->processSamplesDown(block);
        }
        else
        {
            for (int i = 0; i < numSamples; ++i)
                data[i] = shape(data[i]);
        }

        // Post-filter voicing + output level.
        for (int i = 0; i < numSamples; ++i)
        {
            float x = postLPF.processSample(data[i]);
            x = postPresence.processSample(x);
            data[i] = x * outGain;
        }
    }
}
