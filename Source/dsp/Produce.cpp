#include "Produce.h"

namespace effigy
{
    void Produce::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;

        delayL.prepare(spec); delayR.prepare(spec);
        delayL.setMaximumDelayInSamples(4096);
        delayR.setMaximumDelayInSamples(4096);

        reverb.prepare(spec);
        roomDirty = tiltDirty = true;

        juce::dsp::ProcessSpec mono { spec.sampleRate, spec.maximumBlockSize, 1 };
        tiltLoL.prepare(mono); tiltLoR.prepare(mono); tiltHiL.prepare(mono); tiltHiR.prepare(mono);

        reset();
    }

    void Produce::reset()
    {
        delayL.reset(); delayR.reset(); reverb.reset();
        tiltLoL.reset(); tiltLoR.reset(); tiltHiL.reset(); tiltHiR.reset();
        glossEnv = 0.f; driftL = driftR = 0.f;
    }

    void Produce::process(float* left, float* right, int numSamples)
    {
        // ---- Gloss (optical-style leveler, stereo-linked) ----
        if (glossOn)
        {
            const float thresh = juce::Decibels::decibelsToGain(juce::jmap(glossAmt, 0.f, 1.f, -6.f, -30.f));
            const float ratio  = juce::jmap(glossAmt, 0.f, 1.f, 1.5f, 4.f);
            const float atk = std::exp(-1.0f / (0.010f * (float) sampleRate));
            const float rel = std::exp(-1.0f / (0.200f * (float) sampleRate));
            const float makeup = juce::Decibels::decibelsToGain(glossAmt * 6.f);
            for (int i = 0; i < numSamples; ++i)
            {
                const float mag = juce::jmax(std::abs(left[i]), std::abs(right[i]));
                glossEnv = mag > glossEnv ? atk * glossEnv + (1.f - atk) * mag
                                          : rel * glossEnv + (1.f - rel) * mag;
                float gain = 1.f;
                if (glossEnv > thresh && glossEnv > 1e-6f)
                {
                    const float over = glossEnv / thresh;
                    const float compressed = std::pow(over, 1.f / ratio);
                    gain = compressed / over;
                }
                left[i]  *= gain * makeup;
                right[i] *= gain * makeup;
            }
        }

        // ---- Doubler (ADT: aperiodic micro-pitch + short delay per side) ----
        if (dblOn)
        {
            const float baseL = 0.011f * (float) sampleRate; // ~11 ms
            const float baseR = 0.017f * (float) sampleRate; // ~17 ms
            const float depth = (2.f + dblSpread * 8.f) * 0.001f * (float) sampleRate;
            const float rateL = 0.7f, rateR = 0.53f; // Hz-ish, deliberately incommensurate
            const float mix = dblAmt;
            for (int i = 0; i < numSamples; ++i)
            {
                // aperiodic drift: smoothed noise nudging the LFO
                driftL = 0.9995f * driftL + 0.0005f * (rng.nextFloat() * 2.f - 1.f);
                driftR = 0.9995f * driftR + 0.0005f * (rng.nextFloat() * 2.f - 1.f);
                lfoPhaseL += rateL / (float) sampleRate; if (lfoPhaseL > 1.f) lfoPhaseL -= 1.f;
                lfoPhaseR += rateR / (float) sampleRate; if (lfoPhaseR > 1.f) lfoPhaseR -= 1.f;
                const float modL = std::sin(juce::MathConstants<float>::twoPi * lfoPhaseL + driftL * 6.f);
                const float modR = std::sin(juce::MathConstants<float>::twoPi * lfoPhaseR + driftR * 6.f);

                const float dl = baseL + depth * (0.5f + 0.5f * modL);
                const float dr = baseR + depth * (0.5f + 0.5f * modR);

                delayL.pushSample(0, left[i]);
                delayR.pushSample(0, right[i]);
                const float wetL = delayL.popSample(0, dl, true);
                const float wetR = delayR.popSample(0, dr, true);

                left[i]  = left[i]  * (1.f - 0.5f * mix) + wetL * mix;
                right[i] = right[i] * (1.f - 0.5f * mix) + wetR * mix;
            }
        }

        // ---- Width (mid/side) ----
        if (widthOn)
        {
            const float w = widthAmt * 0.01f; // 0..1.5
            for (int i = 0; i < numSamples; ++i)
            {
                const float mid  = 0.5f * (left[i] + right[i]);
                const float side = 0.5f * (left[i] - right[i]) * w;
                left[i]  = mid + side;
                right[i] = mid - side;
            }
        }

        // ---- Room (short ambience, post-cab) ----
        if (roomOn)
        {
            if (roomDirty)
            {
                juce::dsp::Reverb::Parameters p;
                p.roomSize   = juce::jmap(roomSize, 0.f, 1.f, 0.1f, 0.5f);
                p.damping    = 0.5f;
                p.width      = 1.f;
                p.wetLevel   = roomMix;
                p.dryLevel   = 1.f - 0.3f * roomMix;
                p.freezeMode = 0.f;
                reverb.setParameters(p);
                roomDirty = false;
            }
            reverb.processStereo(left, right, numSamples);
        }

        // ---- Tilt (final +/-3 dB, pivot ~700 Hz) ----
        if (tiltOn)
        {
            if (tiltDirty)
            {
                using C = juce::dsp::IIR::Coefficients<float>;
                auto lo = C::makeLowShelf (sampleRate, 700.0, 0.5f, juce::Decibels::decibelsToGain(tiltDbV));
                auto hi = C::makeHighShelf(sampleRate, 700.0, 0.5f, juce::Decibels::decibelsToGain(-tiltDbV));
                tiltLoL.coefficients = lo; tiltLoR.coefficients = lo;
                tiltHiL.coefficients = hi; tiltHiR.coefficients = hi;
                tiltDirty = false;
            }
            for (int i = 0; i < numSamples; ++i)
            {
                left[i]  = tiltHiL.processSample(tiltLoL.processSample(left[i]));
                right[i] = tiltHiR.processSample(tiltLoR.processSample(right[i]));
            }
        }
    }
}
