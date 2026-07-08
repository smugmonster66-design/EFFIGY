#include "CabSection.h"
#include "FactoryIRs.h"

namespace effigy
{
    void CabSection::prepare(const juce::dsp::ProcessSpec& stereoSpec)
    {
        sampleRate = stereoSpec.sampleRate;
        blockSize  = stereoSpec.maximumBlockSize;

        juce::dsp::ProcessSpec mono { sampleRate, blockSize, 1 };
        convA.prepare(mono);
        convB.prepare(mono);

        juce::dsp::ProcessSpec monoFilt { sampleRate, blockSize, 1 };
        lowCutL.prepare(monoFilt);  lowCutR.prepare(monoFilt);
        highCutL.prepare(monoFilt); highCutR.prepare(monoFilt);

        scratchA.setSize(1, (int) blockSize);
        scratchB.setSize(1, (int) blockSize);

        cutsDirty = true;
        reset();
    }

    void CabSection::reset()
    {
        convA.reset(); convB.reset();
        lowCutL.reset(); lowCutR.reset(); highCutL.reset(); highCutR.reset();
    }

    void CabSection::setCuts(float lowHz, float highHz)
    {
        if (! juce::approximatelyEqual(lowHz, lowHzCur) || ! juce::approximatelyEqual(highHz, highHzCur))
        {
            lowHzCur = lowHz; highHzCur = highHz; cutsDirty = true;
        }
    }

    void CabSection::loadFactoryA(int idx)
    {
        if (idx == currentA) return;
        currentA = idx;
        if (idx < 0) { loadedA = false; return; }
        int sz = 0; const char* d = FactoryIRs::data(idx, sz);
        if (d != nullptr && sz > 0)
        {
            convA.loadImpulseResponse(d, (size_t) sz,
                juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0,
                juce::dsp::Convolution::Normalise::yes);
            loadedA = true;
        }
    }

    void CabSection::loadFactoryB(int idx)
    {
        if (idx == currentB) return;
        currentB = idx;
        if (idx < 0) { loadedB = false; return; }
        int sz = 0; const char* d = FactoryIRs::data(idx, sz);
        if (d != nullptr && sz > 0)
        {
            convB.loadImpulseResponse(d, (size_t) sz,
                juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, 0,
                juce::dsp::Convolution::Normalise::yes);
            loadedB = true;
        }
    }

    void CabSection::loadUserA(const juce::File& f)
    {
        if (! f.existsAsFile()) return;
        currentA = -100;
        convA.loadImpulseResponse(f, juce::dsp::Convolution::Stereo::no,
            juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        loadedA = true;
    }

    void CabSection::loadUserB(const juce::File& f)
    {
        if (! f.existsAsFile()) return;
        currentB = -100;
        convB.loadImpulseResponse(f, juce::dsp::Convolution::Stereo::no,
            juce::dsp::Convolution::Trim::no, 0, juce::dsp::Convolution::Normalise::yes);
        loadedB = true;
    }

    void CabSection::process(const float* monoIn, float* outL, float* outR, int numSamples)
    {
        if (bypassed)
        {
            for (int i = 0; i < numSamples; ++i) { outL[i] = monoIn[i]; outR[i] = monoIn[i]; }
            return;
        }

        if (cutsDirty)
        {
            using C = juce::dsp::IIR::Coefficients<float>;
            auto lo = C::makeHighPass(sampleRate, juce::jlimit(20.0, 500.0, (double) lowHzCur));
            auto hi = C::makeLowPass (sampleRate, juce::jlimit(1000.0, sampleRate * 0.45, (double) highHzCur));
            lowCutL.coefficients = lo;  lowCutR.coefficients = lo;
            highCutL.coefficients = hi; highCutR.coefficients = hi;
            cutsDirty = false;
        }

        scratchA.setSize(1, numSamples, false, false, true);
        scratchB.setSize(1, numSamples, false, false, true);
        auto* a = scratchA.getWritePointer(0);
        auto* b = scratchB.getWritePointer(0);

        for (int i = 0; i < numSamples; ++i) { a[i] = monoIn[i]; b[i] = monoIn[i]; }

        if (loadedA)
        {
            juce::dsp::AudioBlock<float> blk(scratchA);
            juce::dsp::ProcessContextReplacing<float> ctx(blk);
            convA.process(ctx);
        }
        else juce::FloatVectorOperations::clear(a, numSamples);

        if (loadedB)
        {
            juce::dsp::AudioBlock<float> blk(scratchB);
            juce::dsp::ProcessContextReplacing<float> ctx(blk);
            convB.process(ctx);
        }
        else juce::FloatVectorOperations::clear(b, numSamples);

        const float gA = juce::Decibels::decibelsToGain(levelA);
        const float gB = juce::Decibels::decibelsToGain(levelB);
        // blend: 0 = only A, 1 = only B
        const float wA = std::cos(blend * juce::MathConstants<float>::halfPi);
        const float wB = std::sin(blend * juce::MathConstants<float>::halfPi);

        auto panGains = [](float pan, float& l, float& r)
        {
            const float p = (pan + 1.f) * 0.5f; // 0..1
            l = std::cos(p * juce::MathConstants<float>::halfPi);
            r = std::sin(p * juce::MathConstants<float>::halfPi);
        };
        float aL, aR, bL, bR;
        panGains(panA, aL, aR);
        panGains(panB, bL, bR);

        for (int i = 0; i < numSamples; ++i)
        {
            const float sa = a[i] * gA * wA;
            const float sb = b[i] * gB * wB;
            float l = sa * aL + sb * bL;
            float r = sa * aR + sb * bR;
            l = lowCutL.processSample(l);  l = highCutL.processSample(l);
            r = lowCutR.processSample(r);  r = highCutR.processSample(r);
            outL[i] = l; outR[i] = r;
        }
    }
}
