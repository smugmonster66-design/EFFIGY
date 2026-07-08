#include "LookAndFeel.h"

namespace effigy
{
    const juce::Colour EffigyLookAndFeel::stone      { 0xff3a3a38 };
    const juce::Colour EffigyLookAndFeel::stoneDark  { 0xff232322 };
    const juce::Colour EffigyLookAndFeel::stoneLight { 0xff56564f };
    const juce::Colour EffigyLookAndFeel::ember      { 0xffc8632a };
    const juce::Colour EffigyLookAndFeel::engrave    { 0xff9a9a90 };

    EffigyLookAndFeel::EffigyLookAndFeel()
    {
        setColour(juce::Slider::textBoxTextColourId, engrave);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Label::textColourId, engrave);
        setColour(juce::ComboBox::backgroundColourId, stoneDark);
        setColour(juce::ComboBox::textColourId, engrave);
        setColour(juce::ComboBox::outlineColourId, stoneLight);
        setColour(juce::PopupMenu::backgroundColourId, stoneDark);
        setColour(juce::PopupMenu::textColourId, engrave);
    }

    void EffigyLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                                             float pos, float startAngle, float endAngle,
                                             juce::Slider&)
    {
        auto bounds = juce::Rectangle<float>((float) x, (float) y, (float) w, (float) h).reduced(4.f);
        const auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto cx = bounds.getCentreX(), cy = bounds.getCentreY();
        const auto angle = startAngle + pos * (endAngle - startAngle);

        // carved socket
        g.setColour(stoneDark);
        g.fillEllipse(cx - radius, cy - radius, radius * 2.f, radius * 2.f);
        g.setColour(stoneLight.withAlpha(0.4f));
        g.drawEllipse(cx - radius, cy - radius, radius * 2.f, radius * 2.f, 1.5f);

        // knob face
        const auto kr = radius * 0.72f;
        juce::ColourGradient grad(stoneLight, cx, cy - kr, stone, cx, cy + kr, false);
        g.setGradientFill(grad);
        g.fillEllipse(cx - kr, cy - kr, kr * 2.f, kr * 2.f);

        // value arc
        juce::Path arc;
        arc.addCentredArc(cx, cy, radius * 0.88f, radius * 0.88f, 0.f, startAngle, angle, true);
        g.setColour(ember);
        g.strokePath(arc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // pointer
        juce::Path p;
        p.addRoundedRectangle(-1.5f, -kr, 3.f, kr * 0.55f, 1.5f);
        g.setColour(ember.brighter(0.2f));
        g.fillPath(p, juce::AffineTransform::rotation(angle).translated(cx, cy));
    }

    void EffigyLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b,
                                                 const juce::Colour&, bool over, bool down)
    {
        auto r = b.getLocalBounds().toFloat().reduced(1.f);
        const bool on = b.getToggleState();
        g.setColour(on ? ember.withAlpha(0.85f) : stoneDark);
        g.fillRoundedRectangle(r, 3.f);
        g.setColour(on ? ember.brighter(0.3f) : stoneLight.withAlpha(over ? 0.7f : 0.4f));
        g.drawRoundedRectangle(r, 3.f, 1.2f);
        juce::ignoreUnused(down);
    }

    juce::Font EffigyLookAndFeel::getLabelFont(juce::Label&)
    {
        return juce::Font(juce::FontOptions(13.0f, juce::Font::bold));
    }
}
