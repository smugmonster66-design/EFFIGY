#include "PluginEditor.h"
#include "Presets.h"

namespace effigy
{
    using namespace effigy::params;

    juce::Slider* EffigyAudioProcessorEditor::addKnob(const juce::String& section, const char* id,
                                                      const juce::String& name, const juce::String& tip)
    {
        auto s = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag,
                                                juce::Slider::TextBoxBelow);
        s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 62, 15);
        s->setLookAndFeel(&lnf);
        s->setTooltip(tip);
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

    juce::ToggleButton* EffigyAudioProcessorEditor::addToggle(const juce::String& section, const char* id,
                                                              const juce::String& name, const juce::String& tip)
    {
        auto b = std::make_unique<juce::ToggleButton>(name);
        b->setLookAndFeel(&lnf);
        b->setTooltip(tip);
        auto* ptr = b.get();
        buttonAtts.add(new ButtonAtt(proc.apvts, id, *ptr));
        addAndMakeVisible(*ptr);
        controls.push_back({ section, std::move(b), nullptr });
        return ptr;
    }

    juce::ComboBox* EffigyAudioProcessorEditor::addCombo(const juce::String& section, const char* id,
                                                         const juce::String& name, const juce::StringArray& items,
                                                         const juce::String& tip)
    {
        auto c = std::make_unique<juce::ComboBox>();
        c->addItemList(items, 1);
        c->setLookAndFeel(&lnf);
        c->setTooltip(tip);
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
        presetBox.setTooltip("Factory presets, organised by drive circuit. Each is a finished tone: full chain including cab pairing and produce settings.");
        addAndMakeVisible(presetBox);

        // INPUT
        addKnob("INPUT", inputGain, "In Gain",
            "Input gain (-24 to +24 dB). Calibrate your DI here so presets behave consistently.");
        addKnob("INPUT", pickupTilt, "Pickup Tilt",
            "Tilt EQ around 800 Hz (+/-3 dB). Tilt negative for bright single-coils, positive for dark humbuckers, to normalise pickups before the drive.");
        // GATE / TIGHT
        addKnob("GATE / TIGHT", tightFreq, "Tight",
            "Pre-drive resonant high-pass (40-350 Hz). Controls how much low-string energy hits the distortion - raise it to tighten palm mutes and reduce intermodulation. Minimum = effectively off.");
        addKnob("GATE / TIGHT", tightReso, "Reso",
            "Resonance/Q of the Tight high-pass filter. Higher values add a bump at the cutoff for a more focused low end.");
        addToggle("GATE / TIGHT", gateOn, "Gate",
            "Enable the noise gate. Shares its envelope detector with the Tightener. Off by default on clean circuits, on for high-gain.");
        addKnob("GATE / TIGHT", gateThresh, "Thresh",
            "Gate threshold (-80 to 0 dB). Signal below this level is muted. Raise until noise/hum between notes is silenced.");
        addKnob("GATE / TIGHT", gateRelease, "Release",
            "Base gate release time (ms). Actual release is program-dependent: faster under dense palm-mutes, slower under sustained chords.");
        // DRIVE
        addCombo("DRIVE", circuit, "Circuit", circuitNames(),
            "Drive circuit, clean to extreme: WAX (clean/warm), STRAW (edge of breakup), WICKER (crunch/classic rock), BRONZE (tight modern high gain), ASH (extreme metal). Each has its own voicing, feel and tone-stack tuning.");
        addKnob("DRIVE", drive, "Drive",
            "Amount of gain into the nonlinearity. Sets how hard the circuit is driven.");
        addKnob("DRIVE", character, "Character",
            "Circuit-specific voicing control: bias asymmetry on WAX, attack looseness on STRAW, clip blend on WICKER, stage-cascade balance on BRONZE, edge/clarity on ASH.");
        addKnob("DRIVE", driveLevel, "Level",
            "Output level of the drive stage (-24 to +24 dB). Use to match volume when comparing circuits or drive settings.");
        addToggle("DRIVE", oversample, "OS 4x",
            "4x oversampling of the drive nonlinearity to reduce aliasing. Adds reported latency. Leave on for tracking; turn off only to save CPU.");
        // FEEL
        addKnob("FEEL", feel, "Feel",
            "Master Feel macro (0-100%). Scales both Sag and Bloom within each circuit's tuned range - the amp-like touch response. 0 = static waveshaping only.");
        addKnob("FEEL", sagDepth, "Sag Depth",
            "How much the drive momentarily dips on transients (power-supply sag). Deeper = more compression/bloom on pick attack. Scaled by Feel.");
        addKnob("FEEL", sagRecovery, "Sag Rec",
            "How quickly sag recovers after a transient (ms). Slower = more vintage bloom; faster = tighter modern response.");
        addKnob("FEEL", bloomKnee, "Bloom Knee",
            "Input level below which the drive cleans up (dB). Rolling back your guitar volume under this knee cleans the tone naturally.");
        // TONE
        addKnob("TONE", bass, "Bass",
            "Low shelf (+/-12 dB), post-drive. Centre frequency is voiced per circuit.");
        addKnob("TONE", mid, "Mid",
            "Midrange peak (+/-12 dB). Its centre is set by Mid Shift; its Q is voiced per circuit.");
        addKnob("TONE", treble, "Treble",
            "High shelf (+/-12 dB), post-drive. Centre frequency is voiced per circuit.");
        addKnob("TONE", presence, "Presence",
            "Upper-mid/presence peak (+/-12 dB) for bite and pick attack. Voiced per circuit.");
        addKnob("TONE", midShift, "Mid Shift",
            "Sweeps the Mid band's centre frequency (300 Hz - 1.5 kHz) - the single most genre-defining EQ move on guitar.");
        // CAB
        addCombo("CAB", irA, "IR A", factoryIRNames(),
            "Impulse response for cab slot A. Choose a factory IR or 'None', or click Load IR A for your own WAV. Pan slots apart for a wide cab.");
        addCombo("CAB", irB, "IR B", factoryIRNames(),
            "Impulse response for cab slot B. Blend against slot A. 'None' leaves the slot empty.");
        addKnob("CAB", cabBlend, "Blend",
            "Equal-power blend between cab slot A (left) and slot B (right).");
        addKnob("CAB", cabLevelA, "Lvl A",
            "Level trim for cab slot A (-24 to +6 dB).");
        addKnob("CAB", cabLevelB, "Lvl B",
            "Level trim for cab slot B (-24 to +6 dB).");
        addKnob("CAB", cabPanA, "Pan A",
            "Stereo position of cab slot A. Stereo begins here - pan A and B apart for a wide cab image.");
        addKnob("CAB", cabPanB, "Pan B",
            "Stereo position of cab slot B.");
        addKnob("CAB", cabLowCut, "Low Cut",
            "Global post-cab high-pass (20-200 Hz). Tightens boominess from the cab.");
        addKnob("CAB", cabHighCut, "High Cut",
            "Global post-cab low-pass (3-20 kHz). Tames fizz and harshness.");
        addToggle("CAB", cabBypass, "Cab Byp",
            "Bypass the cab section entirely (for external IR chains). Output passes mono to both channels.");
        // PRODUCE
        addToggle("PRODUCE", glossOn, "Gloss",
            "Enable Gloss, an optical-style compressor with auto makeup. Mainly for clean/funk/pop; off by default on high-gain (already compressed by saturation).");
        addKnob("PRODUCE", glossAmt, "Gloss Amt",
            "Gloss compression amount. Higher = lower threshold, higher ratio and more makeup gain.");
        addToggle("PRODUCE", dblOn, "Doubler",
            "Enable the ADT doubler: aperiodic micro-pitch drift + short modulated delay per side. A 'double when you don't want to track twice' tool, not a replacement for real double-tracking.");
        addKnob("PRODUCE", dblAmt, "Dbl Amt",
            "Doubler mix - how much of the doubled voice is blended in.");
        addKnob("PRODUCE", dblSpread, "Dbl Spread",
            "Doubler stereo spread and modulation depth.");
        addToggle("PRODUCE", widthOn, "Width",
            "Enable stereo Width (mid/side) on the doubled/panned image.");
        addKnob("PRODUCE", width, "Width %",
            "Stereo width, 0-150%. 100% = unchanged. Check mono compatibility with the meter.");
        addToggle("PRODUCE", roomOn, "Room",
            "Enable a short post-cab ambience - the 'recorded in a space' cue that finishes a tone.");
        addKnob("PRODUCE", roomMix, "Room Mix",
            "Wet/dry mix of the Room ambience.");
        addKnob("PRODUCE", roomSize, "Room Size",
            "Size of the Room ambience, from a tight booth to a larger room.");
        addToggle("PRODUCE", tiltOn, "Tilt",
            "Enable the final tilt EQ - the last-inch mix decision.");
        addKnob("PRODUCE", tilt, "Tilt dB",
            "Final tilt EQ around 700 Hz (+/-3 dB). Negative = darker/warmer, positive = brighter.");
        // OUTPUT
        addKnob("OUTPUT", outputTrim, "Output",
            "Master output trim (-24 to +24 dB), shown on the stereo meter at the right.");
        addToggle("OUTPUT", bypass, "Bypass",
            "Global bypass with a click-free crossfade.");

        loadIRA.setLookAndFeel(&lnf);
        loadIRB.setLookAndFeel(&lnf);
        loadIRA.setTooltip("Load your own impulse response (WAV, any sample rate) into cab slot A. Stored by file path in the session.");
        loadIRB.setTooltip("Load your own impulse response (WAV, any sample rate) into cab slot B.");
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
