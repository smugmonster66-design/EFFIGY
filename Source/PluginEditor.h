#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "gui/LookAndFeel.h"

namespace effigy
{
    class EffigyAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
    {
    public:
        explicit EffigyAudioProcessorEditor(EffigyAudioProcessor&);
        ~EffigyAudioProcessorEditor() override;

        void paint(juce::Graphics&) override;
        void resized() override;

    private:
        using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
        using ButtonAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;
        using ComboAtt  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

        struct Control
        {
            juce::String section;
            std::unique_ptr<juce::Component> comp;
            std::unique_ptr<juce::Label> label;
        };

        juce::Slider*   addKnob(const juce::String& section, const char* id, const juce::String& name,
                                const juce::String& tip);
        juce::ToggleButton* addToggle(const juce::String& section, const char* id, const juce::String& name,
                                      const juce::String& tip);
        juce::ComboBox* addCombo(const juce::String& section, const char* id, const juce::String& name,
                                 const juce::StringArray& items, const juce::String& tip);

        void timerCallback() override;
        void loadIR(bool slotB);

        EffigyAudioProcessor& proc;
        EffigyLookAndFeel lnf;
        juce::TooltipWindow tooltipWindow { this, 600 };

        std::vector<Control> controls;
        juce::OwnedArray<SliderAtt> sliderAtts;
        juce::OwnedArray<ButtonAtt> buttonAtts;
        juce::OwnedArray<ComboAtt>  comboAtts;

        juce::ComboBox presetBox;
        juce::TextButton loadIRA { "Load IR A" }, loadIRB { "Load IR B" };
        std::unique_ptr<juce::FileChooser> chooser;

        juce::StringArray sectionOrder {
            "INPUT", "GATE / TIGHT", "DRIVE", "FEEL", "TONE", "CAB", "PRODUCE", "OUTPUT" };

        float meterL = 0.f, meterR = 0.f;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffigyAudioProcessorEditor)
    };
}
