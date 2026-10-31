#include "Parameters.h"
#include "dsp/AmpModels.h"

namespace
{
    using juce::NormalisableRange;

    juce::AudioParameterFloatAttributes withUnit (const char* unit, int decimals)
    {
        return juce::AudioParameterFloatAttributes().withStringFromValueFunction (
            [unit, decimals] (float value, int)
            {
                const juce::String text (value, decimals);
                return *unit == 0 ? text : text + " " + unit;
            });
    }

    juce::AudioParameterFloatAttributes asPercent()
    {
        return juce::AudioParameterFloatAttributes().withStringFromValueFunction (
            [] (float value, int) { return juce::String (juce::roundToInt (value * 100.0f)) + " %"; });
    }

    NormalisableRange<float> skewed (float min, float max, float interval, float centre)
    {
        NormalisableRange<float> range { min, max, interval };
        range.setSkewForCentre (centre);
        return range;
    }

    const NormalisableRange<float> unitRange { 0.0f, 1.0f, 0.001f };
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto addFloat = [&layout] (const char* id, int version, const char* name, NormalisableRange<float> range,
                               float defaultValue, juce::AudioParameterFloatAttributes attributes)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, version }, name, range,
                                                                 defaultValue, attributes));
    };

    auto addToggle = [&layout] (const char* id, int version, const char* name)
    {
        layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { id, version }, name, false));
    };

    auto addChoice = [&layout] (const char* id, int version, const char* name, const juce::StringArray& choices, int defaultIndex)
    {
        layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { id, version }, name, choices, defaultIndex));
    };

    addToggle (ParamIds::gateOn, 2, "Gate On");
    addFloat (ParamIds::gateThreshold, 2, "Gate Threshold", { -90.0f, -20.0f, 0.1f }, -60.0f, withUnit ("dB", 1));
    addFloat (ParamIds::gateRelease,   2, "Gate Release", skewed (10.0f, 500.0f, 1.0f, 100.0f), 100.0f, withUnit ("ms", 0));

    addToggle (ParamIds::compOn, 2, "Comp On");
    addFloat (ParamIds::compThreshold, 2, "Comp Threshold", { -60.0f, 0.0f, 0.1f }, -20.0f, withUnit ("dB", 1));
    addFloat (ParamIds::compRatio,     2, "Comp Ratio", skewed (1.0f, 20.0f, 0.1f, 4.0f), 4.0f,
              juce::AudioParameterFloatAttributes().withStringFromValueFunction (
                  [] (float value, int) { return juce::String (value, 1) + ":1"; }));
    addFloat (ParamIds::compAttack,    2, "Comp Attack", skewed (0.1f, 100.0f, 0.1f, 10.0f), 10.0f, withUnit ("ms", 1));
    addFloat (ParamIds::compRelease,   2, "Comp Release", skewed (10.0f, 1000.0f, 1.0f, 150.0f), 150.0f, withUnit ("ms", 0));
    addFloat (ParamIds::compMakeup,    2, "Comp Makeup", { 0.0f, 24.0f, 0.1f }, 6.0f, withUnit ("dB", 1));
    addFloat (ParamIds::compMix,       5, "Comp Mix", unitRange, 1.0f, asPercent());

    addToggle (ParamIds::octOn, 2, "Octaver On");
    addFloat (ParamIds::octSub, 2, "Octaver Sub", unitRange, 0.7f, asPercent());
    addFloat (ParamIds::octDry, 2, "Octaver Dry", unitRange, 0.8f, asPercent());
    addFloat (ParamIds::octUp,  5, "Octaver Up", unitRange, 0.0f, asPercent());

    addToggle (ParamIds::envOn, 2, "Env Filter On");
    addFloat (ParamIds::envSensitivity, 2, "Env Sensitivity", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::envResonance,   2, "Env Resonance", skewed (0.7f, 8.0f, 0.01f, 2.5f), 3.0f, withUnit ("Q", 1));
    addFloat (ParamIds::envMix,         2, "Env Mix", unitRange, 1.0f, asPercent());
    addChoice (ParamIds::envType,      6, "Env Type", { "LP", "BP", "HP" }, 0);
    addChoice (ParamIds::envDirection, 6, "Env Sweep", { "Up", "Down" }, 0);

    addToggle (ParamIds::fuzzOn, 2, "Fuzz On");
    addFloat (ParamIds::fuzzAmount, 2, "Fuzz", unitRange, 0.6f, asPercent());
    addFloat (ParamIds::fuzzTone,   2, "Fuzz Tone", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::fuzzBlend,  2, "Fuzz Blend", unitRange, 0.6f, asPercent());

    addToggle (ParamIds::distOn, 1, "Disto On");
    addFloat (ParamIds::distDrive, 1, "Drive", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::distTone,  1, "Tone", skewed (400.0f, 8000.0f, 1.0f, 2000.0f), 2500.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::distBlend, 1, "Blend", unitRange, 0.5f, asPercent());

    addToggle (ParamIds::filterOn, 6, "Filter On");
    addChoice (ParamIds::filterType, 6, "Filter Type", { "LP 12", "LP 24", "BP 12", "BP 24", "HP 12", "HP 24" }, 1);
    addFloat (ParamIds::filterCutoff,    6, "Filter Cutoff", skewed (40.0f, 8000.0f, 1.0f, 600.0f), 800.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::filterResonance, 6, "Filter Resonance", unitRange, 0.4f, asPercent());
    addFloat (ParamIds::filterRate,      6, "Filter LFO Rate", skewed (0.05f, 10.0f, 0.01f, 1.0f), 0.5f, withUnit ("Hz", 2));
    addFloat (ParamIds::filterDepth,     6, "Filter LFO Depth", unitRange, 0.0f, asPercent());

    addToggle (ParamIds::wahOn, 6, "Wah On");
    addFloat (ParamIds::wahPedal,     6, "Wah Pedal", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::wahResonance, 6, "Wah Resonance", skewed (1.0f, 12.0f, 0.01f, 4.0f), 5.0f, withUnit ("Q", 1));
    addFloat (ParamIds::wahBlend,     6, "Wah Clean Blend", unitRange, 0.25f, asPercent());

    addToggle (ParamIds::synthOn, 6, "Synth On");
    addChoice (ParamIds::synthWave, 6, "Synth Wave", { "Square", "Saw" }, 0);
    addFloat (ParamIds::synthCutoff,    6, "Synth Cutoff", skewed (80.0f, 4000.0f, 1.0f, 500.0f), 400.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::synthResonance, 6, "Synth Resonance", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::synthEnvelope,  6, "Synth Envelope", unitRange, 0.6f, asPercent());
    addFloat (ParamIds::synthMix,       6, "Synth Mix", unitRange, 0.7f, asPercent());

    addToggle (ParamIds::pitchOn, 6, "Pitch On");
    addFloat (ParamIds::pitchShift,  6, "Pitch Shift", { -12.0f, 12.0f, 1.0f }, 12.0f,
              juce::AudioParameterFloatAttributes().withStringFromValueFunction (
                  [] (float value, int) { return (value > 0.0f ? "+" : "") + juce::String (juce::roundToInt (value)) + " st"; }));
    addFloat (ParamIds::pitchDetune, 6, "Pitch Detune", { -50.0f, 50.0f, 1.0f }, 0.0f, withUnit ("ct", 0));
    addFloat (ParamIds::pitchMix,    6, "Pitch Mix", unitRange, 0.5f, asPercent());

    juce::StringArray modelNames, cabNames;

    for (const auto& model : ampModels)
        modelNames.add (model.name);

    for (const auto& cab : cabModels)
        cabNames.add (cab.name);

    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { ParamIds::ampModel, 3 }, "Amp Model", modelNames, 1));
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { ParamIds::ampCab, 3 }, "Cabinet", cabNames, 1));
    addFloat (ParamIds::ampGain,    1, "Gain",     { 0.0f, 10.0f, 0.01f }, 4.0f, withUnit ("", 1));
    addFloat (ParamIds::ampBass,    1, "Bass",     { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampMid,     1, "Mid",      { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampMidFreq, 3, "Mid Freq", skewed (200.0f, 3000.0f, 1.0f, 700.0f), 500.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::ampTreble,  1, "Treble",   { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampMaster,  1, "Master",   { -48.0f, 6.0f, 0.1f }, -6.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampBlend,   3, "DI Blend", unitRange, 0.3f, asPercent());
    addFloat (ParamIds::ampContour, 5, "Contour",  unitRange, 0.0f, asPercent());
    addToggle (ParamIds::ampDeep,   5, "Deep");
    addToggle (ParamIds::ampBright, 5, "Bright");
    addFloat (ParamIds::ampLowCut,  6, "Low Cut",  skewed (20.0f, 250.0f, 1.0f, 60.0f), 20.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::ampHighCut, 6, "High Cut", skewed (1500.0f, 20000.0f, 1.0f, 6000.0f), 20000.0f, withUnit ("Hz", 0));

    addToggle (ParamIds::eqOn, 5, "EQ On");
    addChoice (ParamIds::eqMode, 5, "EQ Mode", { "Basic", "Advanced" }, 0);
    addToggle (ParamIds::cutLow, 7, "Low Cut 80 Hz");
    addToggle (ParamIds::cutHigh, 7, "High Cut 5 kHz");
    addToggle (ParamIds::killLow, 7, "Kill Low");
    addToggle (ParamIds::killMid, 7, "Kill Mid");
    addToggle (ParamIds::killHigh, 7, "Kill High");

    const char* graphicNames[] { "EQ 33 Hz", "EQ 63 Hz", "EQ 125 Hz", "EQ 250 Hz", "EQ 500 Hz", "EQ 1 kHz", "EQ 2 kHz", "EQ 4 kHz", "EQ 8 kHz" };

    for (size_t i = 0; i < ParamIds::graphicEq.size(); ++i)
        addFloat (ParamIds::graphicEq[i], 5, graphicNames[i], { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));

    const auto gainRange = NormalisableRange<float> { -18.0f, 18.0f, 0.1f };
    const auto qRange = skewed (0.3f, 8.0f, 0.01f, 1.2f);
    addFloat (ParamIds::peqLowCut,   5, "EQ Low Cut",    skewed (20.0f, 300.0f, 1.0f, 60.0f), 20.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::peqLowFreq,  5, "EQ Low Freq",   skewed (30.0f, 400.0f, 1.0f, 100.0f), 80.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::peqLowGain,  5, "EQ Low Gain",   gainRange, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::peqMid1Freq, 5, "EQ Lo-Mid Freq", skewed (80.0f, 2000.0f, 1.0f, 400.0f), 250.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::peqMid1Gain, 5, "EQ Lo-Mid Gain", gainRange, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::peqMid1Q,    5, "EQ Lo-Mid Q",    qRange, 1.0f, withUnit ("", 2));
    addFloat (ParamIds::peqMid2Freq, 5, "EQ Hi-Mid Freq", skewed (300.0f, 8000.0f, 1.0f, 1500.0f), 1500.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::peqMid2Gain, 5, "EQ Hi-Mid Gain", gainRange, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::peqMid2Q,    5, "EQ Hi-Mid Q",    qRange, 1.0f, withUnit ("", 2));
    addFloat (ParamIds::peqHighFreq, 5, "EQ High Freq",  skewed (1000.0f, 12000.0f, 1.0f, 4000.0f), 5000.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::peqHighGain, 5, "EQ High Gain",  gainRange, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::peqHighCut,  5, "EQ High Cut",   skewed (2000.0f, 20000.0f, 1.0f, 8000.0f), 20000.0f, withUnit ("Hz", 0));

    addToggle (ParamIds::phaserOn, 5, "Phaser On");
    addFloat (ParamIds::phaserRate,     5, "Phaser Rate", skewed (0.05f, 5.0f, 0.01f, 0.6f), 0.5f, withUnit ("Hz", 2));
    addFloat (ParamIds::phaserDepth,    5, "Phaser Depth", unitRange, 0.6f, asPercent());
    addFloat (ParamIds::phaserFeedback, 5, "Phaser Feedback", { 0.0f, 0.9f, 0.001f }, 0.3f, asPercent());

    addToggle (ParamIds::flangerOn, 6, "Flanger On");
    addFloat (ParamIds::flangerRate,     6, "Flanger Rate", skewed (0.05f, 5.0f, 0.01f, 0.5f), 0.3f, withUnit ("Hz", 2));
    addFloat (ParamIds::flangerDepth,    6, "Flanger Depth", unitRange, 0.7f, asPercent());
    addFloat (ParamIds::flangerFeedback, 6, "Flanger Feedback", { 0.0f, 0.9f, 0.001f }, 0.5f, asPercent());
    addFloat (ParamIds::flangerMix,      6, "Flanger Mix", unitRange, 0.5f, asPercent());

    addToggle (ParamIds::chorusOn, 1, "Chorus On");
    addFloat (ParamIds::chorusRate,  1, "Rate", skewed (0.1f, 5.0f, 0.01f, 1.0f), 0.8f, withUnit ("Hz", 2));
    addFloat (ParamIds::chorusDepth, 1, "Depth", unitRange, 0.4f, asPercent());
    addFloat (ParamIds::chorusMix,   1, "Mix", unitRange, 0.5f, asPercent());

    addToggle (ParamIds::tremoloOn, 6, "Tremolo On");
    addFloat (ParamIds::tremoloRate,  6, "Tremolo Rate", skewed (0.5f, 15.0f, 0.01f, 4.0f), 5.0f, withUnit ("Hz", 2));
    addFloat (ParamIds::tremoloDepth, 6, "Tremolo Depth", unitRange, 0.6f, asPercent());
    addFloat (ParamIds::tremoloShape, 6, "Tremolo Shape", unitRange, 0.0f, asPercent());

    addToggle (ParamIds::delayOn, 2, "Delay On");
    addFloat (ParamIds::delayTime,     2, "Delay Time", skewed (20.0f, 1500.0f, 1.0f, 350.0f), 350.0f, withUnit ("ms", 0));
    addFloat (ParamIds::delayFeedback, 2, "Delay Feedback", { 0.0f, 0.9f, 0.001f }, 0.35f, asPercent());
    addFloat (ParamIds::delayMix,      2, "Delay Mix", unitRange, 0.3f, asPercent());

    addToggle (ParamIds::reverbOn, 1, "Reverb On");
    addFloat (ParamIds::reverbSize,    1, "Size", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::reverbDamping, 1, "Damping", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::reverbMix,     1, "Mix", unitRange, 0.25f, asPercent());

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { ParamIds::limiterOn, 6 }, "Output Limiter", true));

    return layout;
}
