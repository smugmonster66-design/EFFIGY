#include "FactoryIRs.h"
#include "BinaryData.h"

namespace effigy::FactoryIRs
{
    // Order the library by family, clean -> extreme voicing feel.
    // Each entry: { display name, source wav filename stem }.
    struct Entry { const char* name; const char* file; };

    static const Entry entries[] = {
        { "VESSEL Core",   "EFFIGY_VESSEL_CORE.wav" },
        { "VESSEL Edge",   "EFFIGY_VESSEL_EDGE.wav" },
        { "VESSEL Soft",   "EFFIGY_VESSEL_SOFT.wav" },
        { "RELIC Core",    "EFFIGY_RELIC_CORE.wav" },
        { "RELIC Edge",    "EFFIGY_RELIC_EDGE.wav" },
        { "RELIC Soft",    "EFFIGY_RELIC_SOFT.wav" },
        { "RELIC Twin",    "EFFIGY_RELIC_TWIN.wav" },
        { "IDOL Core",     "EFFIGY_IDOL_CORE.wav" },
        { "IDOL Edge",     "EFFIGY_IDOL_EDGE.wav" },
        { "IDOL Soft",     "EFFIGY_IDOL_SOFT.wav" },
        { "PYRE Core",     "EFFIGY_PYRE_CORE.wav" },
        { "PYRE Edge",     "EFFIGY_PYRE_EDGE.wav" },
        { "PYRE Soft",     "EFFIGY_PYRE_SOFT.wav" },
        { "PYRE Twin",     "EFFIGY_PYRE_TWIN.wav" },
        { "MONUMENT Core", "EFFIGY_MONUMENT_CORE.wav" },
        { "MONUMENT Edge", "EFFIGY_MONUMENT_EDGE.wav" },
        { "MONUMENT Soft", "EFFIGY_MONUMENT_SOFT.wav" },
        { "MONUMENT Twin", "EFFIGY_MONUMENT_TWIN.wav" },
    };

    static constexpr int numEntries = (int) (sizeof(entries) / sizeof(entries[0]));

    juce::StringArray displayNames()
    {
        juce::StringArray names;
        for (auto& e : entries)
            names.add(e.name);
        return names;
    }

    // Convert a filename into the mangled symbol BinaryData uses.
    static juce::String mangle(const juce::String& file)
    {
        juce::String s;
        for (auto c : file)
            s += (juce::CharacterFunctions::isLetterOrDigit((juce::juce_wchar) c)) ? juce::String::charToString(c)
                                                                                   : juce::String("_");
        return s;
    }

    const char* data(int index, int& sizeOut)
    {
        sizeOut = 0;
        if (index < 0 || index >= numEntries)
            return nullptr;

        const auto target = mangle(entries[index].file);

        for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        {
            if (target == juce::String(BinaryData::namedResourceList[i]))
            {
                int sz = 0;
                const char* d = BinaryData::getNamedResource(BinaryData::namedResourceList[i], sz);
                sizeOut = sz;
                return d;
            }
        }
        return nullptr;
    }
}
