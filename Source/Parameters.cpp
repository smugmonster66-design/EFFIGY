#include "Parameters.h"
#include "dsp/FactoryIRs.h"

namespace effigy::params
{
    using APF   = juce::AudioParameterFloat;
    using APC   = juce::AudioParameterChoice;
    using APB   = juce::AudioParameterBool;
    using Range = juce::NormalisableRange<float>;

    juce::StringArray circuitNames()
    {
        return { "WAX", "STRAW", "WICKER", "BRONZE", "ASH" };
    }

    juce::StringArray factoryIRNames()
    {
        juce::StringArray names;
        names.add("None");
        names.addArray(FactoryIRs::displayNames());
        return names;
    }

    static Range freqRange(float lo, float hi, float skewCentre)
    {
        Range r(lo, hi);
        r.setSkewForCentre(skewCentre);
        return r;
    }

    juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
    {
        using P = std::vector<std::unique_ptr<juce::RangedAudioParameter>>;
        P p;

        auto pid = [](const char* id) { return juce::ParameterID { id, 1 }; };

        // Input ------------------------------------------------------------
        p.push_back(std::make_unique<APF>(pid(inputGain),  "Input Gain",  Range(-24.f, 24.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(pickupTilt), "Pickup Tilt", Range(-3.f, 3.f, 0.1f), 0.f));

        // Gate + Tightener -------------------------------------------------
        p.push_back(std::make_unique<APF>(pid(tightFreq),  "Tight",       freqRange(40.f, 350.f, 120.f), 40.f));
        p.push_back(std::make_unique<APF>(pid(tightReso),  "Tight Reso",  Range(0.3f, 2.5f, 0.01f), 0.9f));
        p.push_back(std::make_unique<APB>(pid(gateOn),     "Gate",        false));
        p.push_back(std::make_unique<APF>(pid(gateThresh), "Gate Thresh", Range(-80.f, 0.f, 0.1f), -60.f));
        p.push_back(std::make_unique<APF>(pid(gateRelease),"Gate Release",freqRange(20.f, 500.f, 120.f), 120.f));

        // Drive ------------------------------------------------------------
        p.push_back(std::make_unique<APC>(pid(circuit),    "Circuit",     circuitNames(), 0));
        p.push_back(std::make_unique<APF>(pid(drive),      "Drive",       Range(0.f, 100.f, 0.1f), 25.f));
        p.push_back(std::make_unique<APF>(pid(driveLevel), "Level",       Range(-24.f, 24.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(character),  "Character",   Range(0.f, 100.f, 0.1f), 50.f));
        p.push_back(std::make_unique<APB>(pid(oversample), "Oversample",  true));

        // Feel -------------------------------------------------------------
        p.push_back(std::make_unique<APF>(pid(feel),        "Feel",       Range(0.f, 100.f, 0.1f), 40.f));
        p.push_back(std::make_unique<APF>(pid(sagDepth),    "Sag Depth",  Range(0.f, 100.f, 0.1f), 50.f));
        p.push_back(std::make_unique<APF>(pid(sagRecovery), "Sag Recover",freqRange(20.f, 400.f, 120.f), 120.f));
        p.push_back(std::make_unique<APF>(pid(bloomKnee),   "Bloom Knee", Range(-60.f, -6.f, 0.1f), -30.f));

        // Tone stack -------------------------------------------------------
        p.push_back(std::make_unique<APF>(pid(bass),     "Bass",     Range(-12.f, 12.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(mid),      "Mid",      Range(-12.f, 12.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(treble),   "Treble",   Range(-12.f, 12.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(presence), "Presence", Range(-12.f, 12.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(midShift), "Mid Shift",freqRange(300.f, 1500.f, 700.f), 650.f));

        // Cab --------------------------------------------------------------
        p.push_back(std::make_unique<APB>(pid(cabBypass), "Cab Bypass", false));
        p.push_back(std::make_unique<APC>(pid(irA), "IR A", factoryIRNames(), 1));
        p.push_back(std::make_unique<APC>(pid(irB), "IR B", factoryIRNames(), 0));
        p.push_back(std::make_unique<APF>(pid(cabBlend),  "Cab Blend",  Range(0.f, 100.f, 0.1f), 50.f));
        p.push_back(std::make_unique<APF>(pid(cabLevelA), "Cab Level A",Range(-24.f, 6.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(cabLevelB), "Cab Level B",Range(-24.f, 6.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APF>(pid(cabPanA),   "Cab Pan A",  Range(-1.f, 1.f, 0.01f), -0.4f));
        p.push_back(std::make_unique<APF>(pid(cabPanB),   "Cab Pan B",  Range(-1.f, 1.f, 0.01f),  0.4f));
        p.push_back(std::make_unique<APF>(pid(cabLowCut), "Cab Low Cut",freqRange(20.f, 200.f, 80.f), 70.f));
        p.push_back(std::make_unique<APF>(pid(cabHighCut),"Cab High Cut",freqRange(3000.f, 20000.f, 8000.f), 15000.f));

        // Produce ----------------------------------------------------------
        p.push_back(std::make_unique<APB>(pid(glossOn),  "Gloss",       false));
        p.push_back(std::make_unique<APF>(pid(glossAmt), "Gloss Amount",Range(0.f, 100.f, 0.1f), 40.f));
        p.push_back(std::make_unique<APB>(pid(dblOn),    "Doubler",     false));
        p.push_back(std::make_unique<APF>(pid(dblAmt),   "Doubler Amt", Range(0.f, 100.f, 0.1f), 50.f));
        p.push_back(std::make_unique<APF>(pid(dblSpread),"Doubler Spread",Range(0.f, 100.f, 0.1f), 60.f));
        p.push_back(std::make_unique<APB>(pid(widthOn),  "Width On",    false));
        p.push_back(std::make_unique<APF>(pid(width),    "Width",       Range(0.f, 150.f, 0.1f), 100.f));
        p.push_back(std::make_unique<APB>(pid(roomOn),   "Room On",     false));
        p.push_back(std::make_unique<APF>(pid(roomMix),  "Room Mix",    Range(0.f, 100.f, 0.1f), 20.f));
        p.push_back(std::make_unique<APF>(pid(roomSize), "Room Size",   Range(0.f, 100.f, 0.1f), 40.f));
        p.push_back(std::make_unique<APB>(pid(tiltOn),   "Tilt On",     false));
        p.push_back(std::make_unique<APF>(pid(tilt),     "Tilt",        Range(-3.f, 3.f, 0.1f), 0.f));

        // Output -----------------------------------------------------------
        p.push_back(std::make_unique<APF>(pid(outputTrim), "Output", Range(-24.f, 24.f, 0.1f), 0.f));
        p.push_back(std::make_unique<APB>(pid(bypass),     "Bypass", false));

        return { p.begin(), p.end() };
    }
}
