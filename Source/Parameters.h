#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace effigy::params
{
    // ---- Parameter IDs ----------------------------------------------------
    // Input / DI
    constexpr auto inputGain   = "inputGain";
    constexpr auto pickupTilt  = "pickupTilt";

    // Gate + Tightener
    constexpr auto tightFreq   = "tightFreq";
    constexpr auto tightReso   = "tightReso";
    constexpr auto gateOn      = "gateOn";
    constexpr auto gateThresh  = "gateThresh";
    constexpr auto gateRelease = "gateRelease";

    // Drive
    constexpr auto circuit     = "circuit";     // 0..4 WAX STRAW WICKER BRONZE ASH
    constexpr auto drive       = "drive";
    constexpr auto driveLevel  = "driveLevel";
    constexpr auto character   = "character";
    constexpr auto oversample  = "oversample";

    // Feel
    constexpr auto feel        = "feel";
    constexpr auto sagDepth    = "sagDepth";
    constexpr auto sagRecovery = "sagRecovery";
    constexpr auto bloomKnee   = "bloomKnee";

    // Tone stack
    constexpr auto bass        = "bass";
    constexpr auto mid         = "mid";
    constexpr auto treble      = "treble";
    constexpr auto presence    = "presence";
    constexpr auto midShift    = "midShift";

    // Cab
    constexpr auto cabBypass   = "cabBypass";
    constexpr auto irA         = "irA";
    constexpr auto irB         = "irB";
    constexpr auto cabBlend    = "cabBlend";
    constexpr auto cabLevelA   = "cabLevelA";
    constexpr auto cabLevelB   = "cabLevelB";
    constexpr auto cabPanA     = "cabPanA";
    constexpr auto cabPanB     = "cabPanB";
    constexpr auto cabLowCut   = "cabLowCut";
    constexpr auto cabHighCut  = "cabHighCut";

    // Produce
    constexpr auto glossOn     = "glossOn";
    constexpr auto glossAmt    = "glossAmt";
    constexpr auto dblOn       = "dblOn";
    constexpr auto dblAmt      = "dblAmt";
    constexpr auto dblSpread   = "dblSpread";
    constexpr auto widthOn     = "widthOn";
    constexpr auto width       = "width";
    constexpr auto roomOn      = "roomOn";
    constexpr auto roomMix     = "roomMix";
    constexpr auto roomSize    = "roomSize";
    constexpr auto tiltOn      = "tiltOn";
    constexpr auto tilt        = "tilt";

    // Output
    constexpr auto outputTrim  = "outputTrim";
    constexpr auto bypass      = "bypass";

    juce::StringArray circuitNames();
    juce::StringArray factoryIRNames();

    juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
}
