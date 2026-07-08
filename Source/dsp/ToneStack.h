#pragma once
#include <juce_dsp/juce_dsp.h>

namespace effigy
{
    // Stage 4: global 4-band tone stack, per-circuit voicing tables + Mid Shift.
    class ToneStack
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();
        void process(float* data, int numSamples);

        void setCircuit(int c) { circuit = juce::jlimit(0, 4, c); dirty = true; }
        void setControls(float bassDb, float midDb, float trebleDb, float presenceDb, float midShiftHz)
        {
            bass = bassDb; mid = midDb; treble = trebleDb; presence = presenceDb; midCentre = midShiftHz; dirty = true;
        }

    private:
        void update();

        double sampleRate = 44100.0;
        int circuit = 0;
        float bass = 0, mid = 0, treble = 0, presence = 0, midCentre = 650.f;
        bool dirty = true;

        juce::dsp::IIR::Filter<float> bassF, midF, trebleF, presenceF;
    };
}
