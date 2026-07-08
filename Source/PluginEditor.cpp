#include "PluginEditor.h"
#include "Presets.h"

namespace effigy
{
    using namespace effigy::params;

    juce::Slider* EffigyAudioProcessorEditor::addKnob(const juce::String& section, const char* id, const juce::String& name)
    {
        auto s = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag,
                                                juce::Slider::TextBoxBelow);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 62, 15);
        s->setLookAndFeel(&lnf);
        auto* ptr = s.get();
        sliderAtts.add(new SliderAtt(proc.apvts, id, *ptr));

        auto lbl = std::make_unique<juce::Label>();
        lbl->setText(name, juce::dontSendNotification);
        lbl->setJustificationType(juce::Justification::centred);
        lbl->setFont(juce::Font(juce::FontOptions(11.f, juce::Font::bold)));

        addAndMakeVisible(*ptr);
        addAndMakeVisible(*lbl);
        controls.push_back({ section, std::move(s), std::move(lbl) });
        return ptr;
    }

    juce::ToggleButton* EffigyAudioProcessorEditor::addToggle(const juce::String& section, const char* id, const juce::String& name)
    {
        auto b = std::make_unique<juce::ToggleButton>(name);
        b->setLookAndFeel(&lnf);
        auto* ptr = b.get();
        buttonAtts.add(new ButtonAtt(proc.apvts, id, *ptr));
        addAndMakeVisible(*ptr);
        controls.push_back({ section, std::move(b), nullptr });
        return ptr;
    }

    juce::ComboBox* EffigyAudioProcessorEditor::addCombo(const juce::String& section, const char* id,
                                                         const juce::String& name, const juce::StringArray& items)
    {
        auto c = std::make_unique<juce::ComboBox>();
        c->addItemList(items, 1);
        c->setLookAndFeel(&lnf);
        auto* ptr = c.get();
        comboAtts.add(new ComboAtt(proc.apvts, id, *ptr));

        auto lbl = std::make_unique<juce::Label>();
        lbl->setText(name, juce::dontSendNotification);
        lbl->setFont(juce::Font(juce::FontOptions(11.f, juce::Font::bold)));
        addAndMakeVisible(*ptr);
        addAndMakeVisible(*lbl);
        controls.push_back({ section, std::move(c), std::move(lbl) });
        return ptr;
    }

    EffigyAudioProcessorEditor::EffigyAudioProcessorEditor(EffigyAudioProcessor& p)
        : AudioProcessorEditor(&p), proc(p)
    {
        setLookAndFeel(&lnf);

        // Preset selector
        for (int i = 0; i < (int) factoryPresets().size(); ++i)
            presetBox.addItem(factoryPresets()[(size_t) i].name, i + 1);
        presetBox.setSelectedId(proc.getCurrentProgram() + 1, juce::dontSendNotification);
        presetBox.onChange = [this]
        {
            const int idx = presetBox.getSelectedId() - 1;
            if (idx >= 0) proc.setCurrentProgram(idx);
        };
        presetBox.setLookAndFeel(&lnf);
        addAndMakeVisible(presetBox);

        // INPUT
        addKnob("INPUT", inputGain, "In Gain");
        addKnob("INPUT", pickupTilt, "Pickup Tilt");
        // GATE / TIGHT
        addKnob("GATE / TIGHT", tightFreq, "Tight");
        addKnob("GATE / TIGHT", tightReso, "Reso");
        addToggle("GATE / TIGHT", gateOn, "Gate");
        addKnob("GATE / TIGHT", gateThresh, "Thresh");
        addKnob("GATE / TIGHT", gateRelease, "Release");
        // DRIVE
        addCombo("DRIVE", circuit, "Circuit", circuitNames());
        addKnob("DRIVE", drive, "Drive");
        addKnob("DRIVE", character, "Character");
        addKnob("DRIVE", driveLevel, "Level");
        addToggle("DRIVE", oversample, "OS 4x");
        // FEEL
        addKnob("FEEL", feel, "Feel");
        addKnob("FEEL", sagDepth, "Sag Depth");
        addKnob("FEEL", sagRecovery, "Sag Rec");
        addKnob("FEEL", bloomKnee, "Bloom Knee");
        // TONE
        addKnob("TONE", bass, "Bass");
        addKnob("TONE", mid, "Mid");
        addKnob("TONE", treble, "Treble");
        addKnob("TONE", presence, "Presence");
        addKnob("TONE", midShift, "Mid Shift");
        // CAB
        addCombo("CAB", irA, "IR A", factoryIRNames());
        addCombo("CAB", irB, "IR B", factoryIRNames());
        addKnob("CAB", cabBlend, "Blend");
        addKnob("CAB", cabLevelA, "Lvl A");
        addKnob("CAB", cabLevelB, "Lvl B");
        addKnob("CAB", cabPanA, "Pan A");
        addKnob("CAB", cabPanB, "Pan B");
        addKnob("CAB", cabLowCut, "Low Cut");
        addKnob("CAB", cabHighCut, "High Cut");
        addToggle("CAB", cabBypass, "Cab Byp");
        // PRODUCE
        addToggle("PRODUCE", glossOn, "Gloss");
        addKnob("PRODUCE", glossAmt, "Gloss Amt");
        addToggle("PRODUCE", dblOn, "Doubler");
        addKnob("PRODUCE", dblAmt, "Dbl Amt");
        addKnob("PRODUCE", dblSpread, "Dbl Spread");
        addToggle("PRODUCE", widthOn, "Width");
        addKnob("PRODUCE", width, "Width %");
        addToggle("PRODUCE", roomOn, "Room");
        addKnob("PRODUCE", roomMix, "Room Mix");
        addKnob("PRODUCE", roomSize, "Room Size");
        addToggle("PRODUCE", tiltOn, "Tilt");
        addKnob("PRODUCE", tilt, "Tilt dB");
        // OUTPUT
        addKnob("OUTPUT", outputTrim, "Output");
        addToggle("OUTPUT", bypass, "Bypass");

        loadIRA.setLookAndFeel(&lnf);
        loadIRB.setLookAndFeel(&lnf);
        loadIRA.onClick = [this] { loadIR(false); };
        loadIRB.onClick = [this] { loadIR(true); };
        addAndMakeVisible(loadIRA);
        addAndMakeVisible(loadIRB);

        setResizable(true, true);
        setResizeLimits(1000, 560, 1600, 900);
        setSize(1180, 620);
        startTimerHz(24);
    }

    EffigyAudioProcessorEditor::~EffigyAudioProcessorEditor()
    {
        for (auto& c : controls)
            if (auto* s = dynamic_cast<juce::Slider*>(c.comp.get())) s->setLookAndFeel(nullptr);
        setLookAndFeel(nullptr);
    }

    void EffigyAudioProcessorEditor::loadIR(bool slotB)
    {
        chooser = std::make_unique<juce::FileChooser>("Select an impulse response (WAV)",
                                                      juce::File{}, "*.wav");
        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        chooser->launchAsync(flags, [this, slotB](const juce::FileChooser& fc)
        {
            auto f = fc.getResult();
            if (f.existsAsFile()) proc.loadUserIR(slotB, f);
        });
    }

    void EffigyAudioProcessorEditor::timerCallback()
    {
        meterL = meterL * 0.7f + proc.meterL.load() * 0.3f;
        meterR = meterR * 0.7f + proc.meterR.load() * 0.3f;
        repaint(getLocalBounds().removeFromRight(24));
    }

    void EffigyAudioProcessorEditor::paint(juce::Graphics& g)
    {
        g.fillAll(EffigyLookAndFeel::stoneDark);

        // header
        auto header = getLocalBounds().removeFromTop(46);
        g.setColour(EffigyLookAndFeel::stone);
        g.fillRect(header);
        g.setColour(EffigyLookAndFeel::ember);
        g.setFont(juce::Font(juce::FontOptions(26.f, juce::Font::bold)));
        g.drawText("EFFIGY", header.reduced(16, 0).removeFromLeft(200),
                   juce::Justification::centredLeft);
        g.setColour(EffigyLookAndFeel::engrave);
        g.setFont(juce::Font(juce::FontOptions(12.f, juce::Font::italic)));
        g.drawText("Guitar DI in. Finished record out.",
                   header.reduced(230, 0), juce::Justification::centredLeft);

        // section panels
        auto area = getLocalBounds().withTrimmedTop(52).withTrimmedRight(26).reduced(6, 0);
        const int n = sectionOrder.size();
        const int w = area.getWidth() / n;
        for (int i = 0; i < n; ++i)
        {
            auto col = area.withX(area.getX() + i * w).withWidth(w - 4);
            g.setColour(EffigyLookAndFeel::stone.withAlpha(0.35f));
            g.fillRoundedRectangle(col.toFloat().reduced(2.f), 5.f);
            g.setColour(EffigyLookAndFeel::stoneLight.withAlpha(0.5f));
            g.drawRoundedRectangle(col.toFloat().reduced(2.f), 5.f, 1.f);
            g.setColour(EffigyLookAndFeel::ember.withAlpha(0.9f));
            g.setFont(juce::Font(juce::FontOptions(12.f, juce::Font::bold)));
            g.drawText(sectionOrder[i], col.removeFromTop(20), juce::Justification::centred);
        }

        // meter
        auto meterArea = getLocalBounds().removeFromRight(24).withTrimmedTop(52).reduced(4);
        g.setColour(EffigyLookAndFeel::stoneDark);
        g.fillRect(meterArea);
        auto drawMeter = [&](juce::Rectangle<int> r, float v)
        {
            const float db = juce::Decibels::gainToDecibels(juce::jmax(1e-4f, v));
            const float norm = juce::jlimit(0.f, 1.f, (db + 60.f) / 60.f);
            auto fill = r.removeFromBottom((int) (r.getHeight() * norm));
            g.setColour(v > 0.98f ? juce::Colours::red : EffigyLookAndFeel::ember);
            g.fillRect(fill);
        };
        auto mL = meterArea.removeFromLeft(meterArea.getWidth() / 2).reduced(1);
        auto mR = meterArea.reduced(1);
        drawMeter(mL, meterL);
        drawMeter(mR, meterR);
    }

    void EffigyAudioProcessorEditor::resized()
    {
        auto header = getLocalBounds().removeFromTop(46);
        presetBox.setBounds(header.removeFromRight(260).reduced(8, 10));

        auto area = getLocalBounds().withTrimmedTop(52).withTrimmedRight(26).reduced(6, 0);
        const int n = sectionOrder.size();
        const int w = area.getWidth() / n;

        for (int i = 0; i < n; ++i)
        {
            const auto& sec = sectionOrder[i];
            auto col = area.withX(area.getX() + i * w).withWidth(w - 4).reduced(6);
            col.removeFromTop(24); // title space

            int y = col.getY();
            const int knobH = 74, knobW = col.getWidth();
            for (auto& c : controls)
            {
                if (c.section != sec) continue;
                if (auto* s = dynamic_cast<juce::Slider*>(c.comp.get()))
                {
                    c.label->setBounds(col.getX(), y, knobW, 14);
                    s->setBounds(col.getX(), y + 14, knobW, knobH - 14);
                    y += knobH;
                }
                else if (auto* cb = dynamic_cast<juce::ComboBox*>(c.comp.get()))
                {
                    c.label->setBounds(col.getX(), y, knobW, 14);
                    cb->setBounds(col.getX(), y + 15, knobW, 22);
                    y += 42;
                }
                else if (auto* t = dynamic_cast<juce::ToggleButton*>(c.comp.get()))
                {
                    t->setBounds(col.getX(), y, knobW, 24);
                    y += 28;
                }
            }

            if (sec == "CAB")
            {
                loadIRA.setBounds(col.getX(), y, knobW / 2 - 2, 22);
                loadIRB.setBounds(col.getX() + knobW / 2 + 2, y, knobW / 2 - 2, 22);
            }
        }
    }
}
