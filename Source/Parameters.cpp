#include "Parameters.h"

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

    addToggle (ParamIds::octOn, 2, "Octaver On");
    addFloat (ParamIds::octSub, 2, "Octaver Sub", unitRange, 0.7f, asPercent());
    addFloat (ParamIds::octDry, 2, "Octaver Dry", unitRange, 0.8f, asPercent());

    addToggle (ParamIds::envOn, 2, "Env Filter On");
    addFloat (ParamIds::envSensitivity, 2, "Env Sensitivity", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::envResonance,   2, "Env Resonance", skewed (0.7f, 8.0f, 0.01f, 2.5f), 3.0f, withUnit ("Q", 1));
    addFloat (ParamIds::envMix,         2, "Env Mix", unitRange, 1.0f, asPercent());

    addToggle (ParamIds::fuzzOn, 2, "Fuzz On");
    addFloat (ParamIds::fuzzAmount, 2, "Fuzz", unitRange, 0.6f, asPercent());
    addFloat (ParamIds::fuzzTone,   2, "Fuzz Tone", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::fuzzBlend,  2, "Fuzz Blend", unitRange, 0.6f, asPercent());

    addToggle (ParamIds::distOn, 1, "Disto On");
    addFloat (ParamIds::distDrive, 1, "Drive", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::distTone,  1, "Tone", skewed (400.0f, 8000.0f, 1.0f, 2000.0f), 2500.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::distBlend, 1, "Blend", unitRange, 0.5f, asPercent());

    addFloat (ParamIds::ampGain,   1, "Gain",   { 0.0f, 10.0f, 0.01f }, 3.0f, withUnit ("", 1));
    addFloat (ParamIds::ampBass,   1, "Bass",   { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampMid,    1, "Mid",    { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampTreble, 1, "Treble", { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampMaster, 1, "Master", { -48.0f, 6.0f, 0.1f }, -6.0f, withUnit ("dB", 1));

    addToggle (ParamIds::chorusOn, 1, "Chorus On");
    addFloat (ParamIds::chorusRate,  1, "Rate", skewed (0.1f, 5.0f, 0.01f, 1.0f), 0.8f, withUnit ("Hz", 2));
    addFloat (ParamIds::chorusDepth, 1, "Depth", unitRange, 0.4f, asPercent());
    addFloat (ParamIds::chorusMix,   1, "Mix", unitRange, 0.5f, asPercent());

    addToggle (ParamIds::delayOn, 2, "Delay On");
    addFloat (ParamIds::delayTime,     2, "Delay Time", skewed (20.0f, 1500.0f, 1.0f, 350.0f), 350.0f, withUnit ("ms", 0));
    addFloat (ParamIds::delayFeedback, 2, "Delay Feedback", { 0.0f, 0.9f, 0.001f }, 0.35f, asPercent());
    addFloat (ParamIds::delayMix,      2, "Delay Mix", unitRange, 0.3f, asPercent());

    addToggle (ParamIds::reverbOn, 1, "Reverb On");
    addFloat (ParamIds::reverbSize,    1, "Size", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::reverbDamping, 1, "Damping", unitRange, 0.5f, asPercent());
    addFloat (ParamIds::reverbMix,     1, "Mix", unitRange, 0.25f, asPercent());

    return layout;
}
