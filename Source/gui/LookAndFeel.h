#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace effigy
{
    // Carved / monumental aesthetic: stone greys, chiselled bevels, engraved labels.
    class EffigyLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        EffigyLookAndFeel();

        void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
                              float pos, float startAngle, float endAngle,
                              juce::Slider&) override;

        void drawButtonBackground(juce::Graphics&, juce::Button&,
                                  const juce::Colour&, bool, bool) override;

        juce::Font getLabelFont(juce::Label&) override;

        static const juce::Colour stone, stoneDark, stoneLight, ember, engrave;
    };
}
