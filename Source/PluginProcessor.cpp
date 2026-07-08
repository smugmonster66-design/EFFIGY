#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets.h"

namespace effigy
{
    using namespace effigy::params;

    juce::AudioProcessor::BusesProperties EffigyAudioProcessor::buses()
    {
        return BusesProperties()
            .withInput ("Input",  juce::AudioChannelSet::mono(),   true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true);
    }

    EffigyAudioProcessor::EffigyAudioProcessor()
        : AudioProcessor(buses()),
          apvts(*this, nullptr, "EFFIGY", createLayout())
    {
    }

    bool EffigyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
    {
        const auto out = layout.getMainOutputChannelSet();
        const auto in  = layout.getMainInputChannelSet();
        if (out != juce::AudioChannelSet::stereo())
            return false;
        // Accept mono-in (primary) or stereo-in (summed internally).
        return in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo();
    }

    void EffigyAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        juce::dsp::ProcessSpec monoSpec { sampleRate, (juce::uint32) samplesPerBlock, 1 };
        juce::dsp::ProcessSpec stereoSpec { sampleRate, (juce::uint32) samplesPerBlock, 2 };

        input.prepare(monoSpec);
        gate.prepare(monoSpec);
        drive.prepare(monoSpec);
        tone.prepare(monoSpec);
        cab.prepare(stereoSpec);
        produce.prepare(stereoSpec);

        monoBuf.setSize(1, samplesPerBlock);
        stereoBuf.setSize(2, samplesPerBlock);

        lastIrA = lastIrB = -999;
        bypassGain.reset(sampleRate, 0.01);

        const bool os = apvts.getRawParameterValue(oversample)->load() > 0.5f;
        lastOversample = os;
        drive.setOversampling(os);
        setLatencySamples(os ? drive.getLatencySamples() : 0);
    }

    void EffigyAudioProcessor::pullParams()
    {
        auto g = [this](const char* id) { return apvts.getRawParameterValue(id)->load(); };

        input.setInputGainDb(g(inputGain));
        input.setTiltDb(g(pickupTilt));

        gate.setTight(g(tightFreq), g(tightReso));
        gate.setGate(g(gateOn) > 0.5f, g(gateThresh), g(gateRelease));

        const int circ = (int) g(circuit);
        drive.setCircuit(circ);
        drive.setParams(g(params::drive), g(driveLevel), g(character));
        drive.feel().setFeel(g(feel) * 0.01f);
        drive.feel().setSag(g(sagDepth) * 0.01f, g(sagRecovery));
        drive.feel().setBloomKnee(g(bloomKnee));

        tone.setCircuit(circ);
        tone.setControls(g(bass), g(mid), g(treble), g(presence), g(midShift));

        cab.setBypass(g(cabBypass) > 0.5f);
        cab.setBlend(g(cabBlend) * 0.01f);
        cab.setLevels(g(cabLevelA), g(cabLevelB));
        cab.setPans(g(cabPanA), g(cabPanB));
        cab.setCuts(g(cabLowCut), g(cabHighCut));

        produce.setGloss(g(glossOn) > 0.5f, g(glossAmt) * 0.01f);
        produce.setDoubler(g(dblOn) > 0.5f, g(dblAmt) * 0.01f, g(dblSpread) * 0.01f);
        produce.setWidth(g(widthOn) > 0.5f, g(width));
        produce.setRoom(g(roomOn) > 0.5f, g(roomMix) * 0.01f, g(roomSize) * 0.01f);
        produce.setTilt(g(tiltOn) > 0.5f, g(tilt));

        // IR selection (factory) — respect active user IRs.
        const int irAsel = (int) g(irA);
        const int irBsel = (int) g(irB);
        if (irAsel != lastIrA)
        {
            lastIrA = irAsel;
            if (! userActiveA || irAsel != 0) { userActiveA = false; cab.loadFactoryA(irAsel - 1); }
        }
        if (irBsel != lastIrB)
        {
            lastIrB = irBsel;
            if (! userActiveB || irBsel != 0) { userActiveB = false; cab.loadFactoryB(irBsel - 1); }
        }

        // Oversampling toggle -> latency update.
        const bool os = g(oversample) > 0.5f;
        if (os != lastOversample)
        {
            lastOversample = os;
            drive.setOversampling(os);
            setLatencySamples(os ? drive.getLatencySamples() : 0);
        }
    }

    void EffigyAudioProcessor::loadUserIR(bool slotB, const juce::File& f)
    {
        if (slotB) { cab.loadUserB(f); userActiveB = true; userIRPathB = f.getFullPathName(); }
        else       { cab.loadUserA(f); userActiveA = true; userIRPathA = f.getFullPathName(); }
    }

    void EffigyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
    {
        juce::ScopedNoDenormals noDenormals;
        const int numSamples = buffer.getNumSamples();
        const int numIn = getTotalNumInputChannels();

        pullParams();

        // ---- Collapse input to mono ----
        monoBuf.setSize(1, numSamples, false, false, true);
        auto* m = monoBuf.getWritePointer(0);
        if (numIn >= 2)
        {
            auto* l = buffer.getReadPointer(0);
            auto* r = buffer.getReadPointer(1);
            for (int i = 0; i < numSamples; ++i) m[i] = 0.5f * (l[i] + r[i]);
        }
        else
        {
            juce::FloatVectorOperations::copy(m, buffer.getReadPointer(0), numSamples);
        }

        // ---- Mono chain ----
        input.process(m, numSamples);
        gate.process(m, numSamples);
        drive.process(m, numSamples);
        tone.process(m, numSamples);

        // ---- Cab: mono -> stereo ----
        stereoBuf.setSize(2, numSamples, false, false, true);
        auto* sl = stereoBuf.getWritePointer(0);
        auto* sr = stereoBuf.getWritePointer(1);
        cab.process(m, sl, sr, numSamples);

        // ---- Produce ----
        produce.process(sl, sr, numSamples);

        // ---- Output trim + bypass crossfade ----
        const float trim = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(outputTrim)->load());
        const bool  byp  = apvts.getRawParameterValue(bypass)->load() > 0.5f;
        bypassGain.setTargetValue(byp ? 0.f : 1.f);

        auto* outL = buffer.getWritePointer(0);
        auto* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : outL;

        float peakL = 0.f, peakR = 0.f;
        for (int i = 0; i < numSamples; ++i)
        {
            const float bg = bypassGain.getNextValue();
            const float dry = m[i]; // mono dry as bypass source (pre-chain post-mono)
            float L = sl[i] * trim, R = sr[i] * trim;
            L = L * bg + dry * (1.f - bg);
            R = R * bg + dry * (1.f - bg);
            outL[i] = L; outR[i] = R;
            peakL = juce::jmax(peakL, std::abs(L));
            peakR = juce::jmax(peakR, std::abs(R));
        }

        meterL.store(peakL);
        meterR.store(peakR);
    }

    // ---- Programs / presets ----
    int EffigyAudioProcessor::getNumPrograms() { return (int) factoryPresets().size(); }

    void EffigyAudioProcessor::setCurrentProgram(int index)
    {
        const auto& presets = factoryPresets();
        if (index < 0 || index >= (int) presets.size()) return;
        currentProgram = index;
        applyPreset(apvts, presets[(size_t) index]);
    }

    const juce::String EffigyAudioProcessor::getProgramName(int index)
    {
        const auto& presets = factoryPresets();
        if (index < 0 || index >= (int) presets.size()) return {};
        return presets[(size_t) index].name;
    }

    // ---- State ----
    void EffigyAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
    {
        auto state = apvts.copyState();
        // store user IR references by path
        state.setProperty("userIRPathA", userIRPathA, nullptr);
        state.setProperty("userIRPathB", userIRPathB, nullptr);
        std::unique_ptr<juce::XmlElement> xml(state.createXml());
        copyXmlToBinary(*xml, dest);
    }

    void EffigyAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
    {
        std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
        if (xml == nullptr || ! xml->hasTagName(apvts.state.getType())) return;

        auto tree = juce::ValueTree::fromXml(*xml);
        apvts.replaceState(tree);

        userIRPathA = tree.getProperty("userIRPathA", "").toString();
        userIRPathB = tree.getProperty("userIRPathB", "").toString();
        if (userIRPathA.isNotEmpty()) { juce::File f(userIRPathA); if (f.existsAsFile()) loadUserIR(false, f); }
        if (userIRPathB.isNotEmpty()) { juce::File f(userIRPathB); if (f.existsAsFile()) loadUserIR(true, f); }
        lastIrA = lastIrB = -999; // force factory reload for any factory slots
    }

    juce::AudioProcessorEditor* EffigyAudioProcessor::createEditor()
    {
        return new EffigyAudioProcessorEditor(*this);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new effigy::EffigyAudioProcessor();
}
