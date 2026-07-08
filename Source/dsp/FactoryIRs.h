#pragma once
#include <juce_core/juce_core.h>

namespace effigy::FactoryIRs
{
    // Ordered display names for the factory synthetic library.
    juce::StringArray displayNames();

    // Returns the raw embedded WAV bytes for a given 0-based library index.
    // Returns nullptr / size 0 if out of range.
    const char* data(int index, int& sizeOut);
}
