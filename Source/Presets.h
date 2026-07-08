#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace effigy
{
    struct Preset
    {
        juce::String name;
        std::vector<std::pair<juce::String, float>> values; // param id -> raw value
    };

    // Returns the full factory preset library (25-30 finished tones).
    const std::vector<Preset>& factoryPresets();

    // Apply a preset onto an APVTS (values are raw, not normalised).
    void applyPreset(juce::AudioProcessorValueTreeState& apvts, const Preset& p);
}
