#include "Presets.h"
#include "Parameters.h"

namespace effigy
{
    using namespace effigy::params;
    using V = std::vector<std::pair<juce::String, float>>;

    // Helper: build a preset. Only overrides listed; the rest keep param defaults.
    static Preset mk(const char* name, int circ, float driveV, float charV,
                     V tone, int irAV, int irBV, V produce, V extra = {})
    {
        Preset p; p.name = name;
        p.values.push_back({ circuit, (float) circ });
        p.values.push_back({ drive, driveV });
        p.values.push_back({ character, charV });
        p.values.push_back({ irA, (float) irAV });
        p.values.push_back({ irB, (float) irBV });
        for (auto& t : tone)    p.values.push_back(t);
        for (auto& t : produce) p.values.push_back(t);
        for (auto& t : extra)   p.values.push_back(t);
        return p;
    }

    const std::vector<Preset>& factoryPresets()
    {
        static const std::vector<Preset> presets = [] {
            std::vector<Preset> v;

            // ---- WAX (clean/warm) ----
            v.push_back(mk("WAX — Jazz Archtop", 0, 12, 40,
                { {bass,2.f},{mid,1.f},{treble,-1.f},{presence,-2.f},{midShift,500.f} },
                3, 0, { {glossOn,1.f},{glossAmt,30.f},{roomOn,1.f},{roomMix,18.f} }));
            v.push_back(mk("WAX — Funk Single-Coil", 0, 18, 55,
                { {bass,-1.f},{mid,2.f},{treble,3.f},{presence,2.f},{midShift,800.f} },
                1, 2, { {glossOn,1.f},{glossAmt,55.f},{widthOn,1.f},{width,110.f} }));
            v.push_back(mk("WAX — Ambient Pad Clean", 0, 8, 45,
                { {bass,3.f},{mid,0.f},{treble,1.f},{presence,0.f} },
                17, 3, { {roomOn,1.f},{roomMix,45.f},{roomSize,70.f},{widthOn,1.f},{width,130.f},{dblOn,1.f},{dblAmt,40.f} }));
            v.push_back(mk("WAX — Pop Sheen", 0, 22, 60,
                { {bass,1.f},{mid,-1.f},{treble,3.f},{presence,3.f} },
                1, 15, { {glossOn,1.f},{glossAmt,50.f},{widthOn,1.f},{width,120.f},{tiltOn,1.f},{tilt,1.f} }));

            // ---- STRAW (edge of breakup) ----
            v.push_back(mk("STRAW — Blues Breakup", 1, 45, 55,
                { {bass,1.f},{mid,3.f},{treble,1.f},{presence,1.f},{midShift,700.f} },
                4, 0, { {roomOn,1.f},{roomMix,20.f} }, { {feel,60.f} }));
            v.push_back(mk("STRAW — Indie Jangle-Grit", 1, 38, 45,
                { {bass,-1.f},{mid,2.f},{treble,3.f},{presence,2.f} },
                5, 3, { {widthOn,1.f},{width,115.f},{roomOn,1.f},{roomMix,22.f} }, { {feel,55.f} }));
            v.push_back(mk("STRAW — Roots Crunch", 1, 55, 65,
                { {bass,2.f},{mid,3.f},{treble,0.f},{presence,1.f} },
                4, 6, { {glossOn,0.f},{roomOn,1.f},{roomMix,15.f} }, { {feel,65.f},{tightFreq,60.f} }));

            // ---- WICKER (crunch/classic rock) ----
            v.push_back(mk("WICKER — Classic Rock Rhythm", 2, 55, 50,
                { {bass,2.f},{mid,3.f},{treble,1.f},{presence,2.f},{midShift,650.f} },
                8, 7, { {widthOn,1.f},{width,110.f},{roomOn,1.f},{roomMix,18.f} },
                { {feel,45.f},{tightFreq,80.f},{gateOn,1.f},{gateThresh,-55.f} }));
            v.push_back(mk("WICKER — Classic Lead", 2, 68, 60,
                { {bass,1.f},{mid,4.f},{treble,2.f},{presence,3.f},{midShift,900.f} },
                9, 0, { {roomOn,1.f},{roomMix,28.f},{dblOn,1.f},{dblAmt,35.f} }, { {feel,50.f} }));
            v.push_back(mk("WICKER — Punk", 2, 72, 45,
                { {bass,2.f},{mid,2.f},{treble,3.f},{presence,3.f} },
                8, 5, { {widthOn,1.f},{width,120.f} }, { {feel,35.f},{tightFreq,100.f},{gateOn,1.f} }));
            v.push_back(mk("WICKER — Power Pop", 2, 58, 55,
                { {bass,1.f},{mid,2.f},{treble,3.f},{presence,2.f} },
                7, 3, { {glossOn,1.f},{glossAmt,35.f},{widthOn,1.f},{width,125.f},{tiltOn,1.f},{tilt,0.5f} }));

            // ---- BRONZE (tight modern high gain) ----
            v.push_back(mk("BRONZE — Hard Rock", 3, 65, 50,
                { {bass,2.f},{mid,2.f},{treble,2.f},{presence,3.f},{midShift,700.f} },
                11, 14, { {widthOn,1.f},{width,115.f},{roomOn,1.f},{roomMix,15.f} },
                { {feel,35.f},{tightFreq,110.f},{gateOn,1.f},{gateThresh,-50.f} }));
            v.push_back(mk("BRONZE — Thrash Rhythm", 3, 78, 60,
                { {bass,1.f},{mid,1.f},{treble,3.f},{presence,3.f},{midShift,600.f} },
                11, 15, { {widthOn,1.f},{width,120.f} },
                { {feel,25.f},{tightFreq,150.f},{tightReso,1.4f},{gateOn,1.f},{gateThresh,-48.f} }));
            v.push_back(mk("BRONZE — Modern Lead", 3, 80, 55,
                { {bass,1.f},{mid,4.f},{treble,2.f},{presence,4.f},{midShift,1000.f} },
                12, 0, { {roomOn,1.f},{roomMix,30.f},{dblOn,1.f},{dblAmt,40.f},{widthOn,1.f},{width,110.f} },
                { {feel,30.f},{gateOn,1.f} }));
            v.push_back(mk("BRONZE — Prog Chug", 3, 74, 65,
                { {bass,2.f},{mid,0.f},{treble,2.f},{presence,3.f},{midShift,550.f} },
                14, 11, { {widthOn,1.f},{width,118.f} },
                { {feel,28.f},{tightFreq,140.f},{gateOn,1.f},{gateThresh,-46.f} }));

            // ---- ASH (extreme/metal) ----
            v.push_back(mk("ASH — Djent", 4, 82, 55,
                { {bass,2.f},{mid,-1.f},{treble,3.f},{presence,3.f},{midShift,550.f} },
                15, 18, { {widthOn,1.f},{width,125.f} },
                { {feel,20.f},{tightFreq,200.f},{tightReso,1.6f},{gateOn,1.f},{gateThresh,-42.f} }));
            v.push_back(mk("ASH — Modern Metal Rhythm", 4, 85, 60,
                { {bass,3.f},{mid,-1.f},{treble,2.f},{presence,3.f},{midShift,600.f} },
                15, 16, { {widthOn,1.f},{width,120.f},{tiltOn,1.f},{tilt,0.5f} },
                { {feel,18.f},{tightFreq,180.f},{gateOn,1.f},{gateThresh,-44.f} }));
            v.push_back(mk("ASH — Doom", 4, 70, 45,
                { {bass,4.f},{mid,1.f},{treble,0.f},{presence,1.f},{midShift,500.f} },
                18, 11, { {roomOn,1.f},{roomMix,30.f},{roomSize,60.f},{widthOn,1.f},{width,115.f} },
                { {feel,40.f},{tightFreq,90.f},{gateOn,1.f},{gateThresh,-50.f} }));
            v.push_back(mk("ASH — Extreme Lead", 4, 90, 70,
                { {bass,1.f},{mid,3.f},{treble,3.f},{presence,4.f},{midShift,1100.f} },
                16, 0, { {roomOn,1.f},{roomMix,32.f},{dblOn,1.f},{dblAmt,45.f},{widthOn,1.f},{width,115.f} },
                { {feel,22.f},{gateOn,1.f},{gateThresh,-42.f} }));

            // ---- Extra span presets ----
            v.push_back(mk("WAX — Console Warmth", 0, 15, 50,
                { {bass,1.f},{mid,0.f},{treble,1.f},{presence,1.f} }, 4, 0,
                { {glossOn,1.f},{glossAmt,25.f} }));
            v.push_back(mk("STRAW — Country Twang", 1, 40, 70,
                { {bass,-1.f},{mid,1.f},{treble,4.f},{presence,3.f} }, 5, 3,
                { {glossOn,1.f},{glossAmt,40.f},{roomOn,1.f},{roomMix,20.f} }, { {feel,55.f} }));
            v.push_back(mk("WICKER — Garage Fuzz", 2, 85, 30,
                { {bass,2.f},{mid,4.f},{treble,1.f},{presence,2.f} }, 8, 0,
                { {widthOn,1.f},{width,105.f} }, { {feel,45.f},{gateOn,1.f} }));
            v.push_back(mk("BRONZE — Metalcore Rhythm", 3, 80, 62,
                { {bass,2.f},{mid,0.f},{treble,3.f},{presence,3.f},{midShift,620.f} }, 11, 15,
                { {widthOn,1.f},{width,122.f} }, { {feel,24.f},{tightFreq,160.f},{gateOn,1.f},{gateThresh,-45.f} }));
            v.push_back(mk("ASH — Deathcore", 4, 92, 65,
                { {bass,4.f},{mid,-2.f},{treble,2.f},{presence,3.f},{midShift,520.f} }, 15, 18,
                { {widthOn,1.f},{width,128.f} }, { {feel,16.f},{tightFreq,220.f},{tightReso,1.8f},{gateOn,1.f},{gateThresh,-40.f} }));
            v.push_back(mk("WAX — Clean Chorus Bed", 0, 10, 50,
                { {bass,2.f},{mid,0.f},{treble,2.f},{presence,1.f} }, 3, 17,
                { {dblOn,1.f},{dblAmt,55.f},{dblSpread,80.f},{widthOn,1.f},{width,140.f},{roomOn,1.f},{roomMix,30.f} }));
            v.push_back(mk("STRAW — Southern Lead", 1, 58, 60,
                { {bass,2.f},{mid,4.f},{treble,2.f},{presence,3.f},{midShift,850.f} }, 4, 6,
                { {roomOn,1.f},{roomMix,26.f},{dblOn,1.f},{dblAmt,30.f} }, { {feel,60.f} }));
            v.push_back(mk("WICKER — Stadium Rhythm", 2, 62, 55,
                { {bass,3.f},{mid,2.f},{treble,2.f},{presence,3.f} }, 8, 14,
                { {widthOn,1.f},{width,120.f},{roomOn,1.f},{roomMix,20.f},{glossOn,1.f},{glossAmt,30.f} },
                { {feel,42.f},{gateOn,1.f} }));
            v.push_back(mk("BRONZE — Djent-Lite", 3, 76, 58,
                { {bass,2.f},{mid,0.f},{treble,3.f},{presence,3.f},{midShift,580.f} }, 14, 15,
                { {widthOn,1.f},{width,120.f} }, { {feel,26.f},{tightFreq,170.f},{gateOn,1.f},{gateThresh,-44.f} }));

            return v;
        }();
        return presets;
    }

    void applyPreset(juce::AudioProcessorValueTreeState& apvts, const Preset& p)
    {
        // Start from defaults, then apply overrides.
        for (auto* param : apvts.processor.getParameters())
            if (auto* pp = dynamic_cast<juce::RangedAudioParameter*>(param))
                pp->setValueNotifyingHost(pp->getDefaultValue());

        for (auto& kv : p.values)
            if (auto* param = apvts.getParameter(kv.first))
            {
                auto& range = apvts.getParameterRange(kv.first);
                param->setValueNotifyingHost(range.convertTo0to1(kv.second));
            }
    }
}
