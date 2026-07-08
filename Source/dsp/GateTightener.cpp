#include "GateTightener.h"

namespace effigy
{
    void GateTightener::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        juce::dsp::ProcessSpec mono { spec.sampleRate, spec.maximumBlockSize, 1 };
        tightHPF.prepare(mono);
        lastFreq = lastReso = -1.f;
        updateTight();
        reset();
    }

    void GateTightener::reset()
    {
        tightHPF.reset();
        env = slowEnv = 0.f;
        gateGain = gateOn ? 0.f : 1.f;
    }

    void GateTightener::updateTight()
    {
        if (juce::approximatelyEqual(tightFreq, lastFreq) && juce::approximatelyEqual(tightReso, lastReso))
            return;
        lastFreq = tightFreq; lastReso = tightReso;
        tightHPF.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
            sampleRate, juce::jlimit(20.0, sampleRate * 0.45, (double) tightFreq), tightReso);
    }

    void GateTightener::process(float* data, int numSamples)
    {
        updateTight();

        // detector time constants
        const float atk  = std::exp(-1.0f / (0.001f * 5.0f  * (float) sampleRate));   // 5 ms attack
        const float rel  = std::exp(-1.0f / (0.001f * 60.0f * (float) sampleRate));   // 60 ms release
        const float slowRel = std::exp(-1.0f / (0.001f * 250.0f * (float) sampleRate));

        const float threshLin = juce::Decibels::decibelsToGain(gateThreshDb);
        // program-dependent gate release: denser transients (high fast/slow ratio) => faster release
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = data[i];
            const float rectified = std::abs(in);

            // shared detector
            env = rectified > env ? atk * env + (1.f - atk) * rectified
                                  : rel * env + (1.f - rel) * rectified;
            slowEnv = rectified > slowEnv ? env : slowRel * slowEnv + (1.f - slowRel) * rectified;

            // Tightener (pre-drive resonant HPF)
            float x = tightHPF.processSample(in);

            // Gate
            if (gateOn)
            {
                const float density = juce::jlimit(0.f, 1.f, (env - slowEnv) * 8.f);
                // faster release under palm-mute density
                const float relMs = juce::jmap(density, gateReleaseMs, gateReleaseMs * 0.35f);
                const float gRel = std::exp(-1.0f / (0.001f * juce::jmax(5.f, relMs) * (float) sampleRate));
                const float target = env > threshLin ? 1.f : 0.f;
                gateGain = target > gateGain ? 0.7f * gateGain + 0.3f * target       // quick open
                                             : gRel * gateGain + (1.f - gRel) * target; // program release
                x *= gateGain;
            }
            else
            {
                gateGain = 1.f;
            }

            data[i] = x;
        }
    }
}
