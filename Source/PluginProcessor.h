#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters.h"
#include "dsp/InputConditioning.h"
#include "dsp/GateTightener.h"
#include "dsp/Drive.h"
#include "dsp/ToneStack.h"
#include "dsp/CabSection.h"
#include "dsp/Produce.h"

namespace effigy
{
    class EffigyAudioProcessor : public juce::AudioProcessor
    {
    public:
        EffigyAudioProcessor();
        ~EffigyAudioProcessor() override = default;

        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void releaseResources() override {}
        bool isBusesLayoutSupported(const BusesLayout&) const override;
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

        juce::AudioProcessorEditor* createEditor() override;
        bool hasEditor() const override { return true; }

        const juce::String getName() const override { return "EFFIGY"; }
        bool acceptsMidi() const override { return false; }
        bool producesMidi() const override { return false; }
        bool isMidiEffect() const override { return false; }
        double getTailLengthSeconds() const override { return 2.0; }

        int getNumPrograms() override;
        int getCurrentProgram() override { return currentProgram; }
        void setCurrentProgram(int index) override;
        const juce::String getProgramName(int index) override;
        void changeProgramName(int, const juce::String&) override {}

        void getStateInformation(juce::MemoryBlock&) override;
        void setStateInformation(const void*, int sizeInBytes) override;

        juce::AudioProcessorValueTreeState apvts;

        // User IR loading (called from editor / message thread).
        void loadUserIR(bool slotB, const juce::File& f);

        // Metering (stereo peak) for the editor.
        std::atomic<float> meterL { 0.f }, meterR { 0.f };

        juce::String userIRPathA, userIRPathB;

    private:
        void pullParams();

        InputConditioning input;
        GateTightener     gate;
        Drive             drive;
        ToneStack         tone;
        CabSection        cab;
        Produce           produce;

        juce::AudioBuffer<float> monoBuf, stereoBuf;

        int   currentProgram = 0;
        int   lastIrA = -999, lastIrB = -999;
        bool  userActiveA = false, userActiveB = false;
        bool  lastOversample = true;

        juce::LinearSmoothedValue<float> bypassGain { 1.f };

        static juce::AudioProcessor::BusesProperties buses();

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffigyAudioProcessor)
    };
}
