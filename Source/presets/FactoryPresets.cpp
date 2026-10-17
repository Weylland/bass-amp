#include "FactoryPresets.h"
#include "../ParameterIds.h"

namespace
{
    using namespace ParamIds;

    constexpr float clean = 0.0f, vintage = 1.0f, modern = 2.0f;
    constexpr float cabOff = 0.0f, cab810 = 1.0f, cab410 = 2.0f, cab115 = 3.0f;
    constexpr float on = 1.0f;
}

const std::vector<FactoryPreset>& getFactoryPresets()
{
    static const std::vector<FactoryPreset> presets {
        { "Vintage Tube", {
            { ampModel, vintage }, { ampCab, cab810 }, { ampGain, 4.5f }, { ampBass, 3.0f }, { ampMid, 2.0f },
            { ampMidFreq, 450.0f }, { ampTreble, -1.0f }, { ampBlend, 0.3f }, { ampMaster, -3.5f },
            { compOn, on }, { compThreshold, -18.0f }, { compRatio, 3.0f }, { compAttack, 15.0f }, { compMakeup, 4.0f } } },

        { "Clean DI", {
            { ampModel, clean }, { ampCab, cabOff }, { ampGain, 2.0f }, { ampBass, 1.0f }, { ampTreble, 1.0f },
            { ampMaster, -1.5f },
            { compOn, on }, { compThreshold, -18.0f }, { compRatio, 3.0f }, { compAttack, 15.0f }, { compMakeup, 4.0f } } },

        { "Motown Flats", {
            { ampModel, vintage }, { ampCab, cab115 }, { ampGain, 3.0f }, { ampBass, 4.0f }, { ampMid, 1.5f },
            { ampMidFreq, 300.0f }, { ampTreble, -8.0f }, { ampBlend, 0.2f }, { ampMaster, -6.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 4.0f }, { compAttack, 20.0f }, { compMakeup, 5.0f } } },

        { "Slap Funk", {
            { ampModel, clean }, { ampCab, cab410 }, { ampGain, 2.5f }, { ampBass, 5.0f }, { ampMid, -6.0f },
            { ampMidFreq, 600.0f }, { ampTreble, 6.0f }, { ampBlend, 0.4f }, { ampMaster, 1.0f },
            { gateOn, on }, { gateThreshold, -65.0f },
            { compOn, on }, { compThreshold, -24.0f }, { compRatio, 6.0f }, { compAttack, 2.0f }, { compRelease, 100.0f },
            { compMakeup, 8.0f } } },

        { "Modern Growl", {
            { ampModel, modern }, { ampCab, cab410 }, { ampGain, 6.0f }, { ampBass, 2.0f }, { ampMid, 3.0f },
            { ampMidFreq, 1000.0f }, { ampTreble, 2.0f }, { ampBlend, 0.35f }, { ampMaster, -1.5f },
            { gateOn, on }, { gateThreshold, -60.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 4.0f }, { compAttack, 8.0f }, { compMakeup, 5.0f } } },

        { "Metal Grind", {
            { ampModel, modern }, { ampCab, cab810 }, { ampGain, 8.0f }, { ampBass, 3.0f }, { ampMid, -2.0f },
            { ampMidFreq, 700.0f }, { ampTreble, 4.0f }, { ampBlend, 0.4f }, { ampMaster, -7.5f },
            { gateOn, on }, { gateThreshold, -55.0f }, { gateRelease, 60.0f },
            { compOn, on }, { compThreshold, -22.0f }, { compRatio, 5.0f }, { compAttack, 5.0f }, { compMakeup, 6.0f },
            { distOn, on }, { distDrive, 0.6f }, { distTone, 3500.0f }, { distBlend, 0.4f } } },

        { "Fuzz Lead", {
            { ampModel, vintage }, { ampCab, cab410 }, { ampGain, 4.0f }, { ampBass, 2.0f }, { ampMid, 3.0f },
            { ampMidFreq, 800.0f }, { ampBlend, 0.25f }, { ampMaster, -1.5f },
            { fuzzOn, on }, { fuzzAmount, 0.8f }, { fuzzTone, 0.6f }, { fuzzBlend, 0.7f },
            { delayOn, on }, { delayTime, 380.0f }, { delayFeedback, 0.3f }, { delayMix, 0.25f } } },

        { "Funk Envelope", {
            { ampModel, clean }, { ampCab, cab410 }, { ampGain, 3.0f }, { ampBass, 2.0f }, { ampBlend, 0.3f },
            { ampMaster, 0.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 4.0f }, { compAttack, 10.0f }, { compMakeup, 5.0f },
            { envOn, on }, { envSensitivity, 0.6f }, { envResonance, 4.0f }, { envMix, 1.0f } } },

        { "Synth Octave", {
            { ampModel, clean }, { ampCab, cab115 }, { ampGain, 3.0f }, { ampBass, 2.0f }, { ampBlend, 0.3f },
            { ampMaster, -5.5f },
            { gateOn, on }, { gateThreshold, -60.0f },
            { octOn, on }, { octSub, 0.9f }, { octDry, 0.3f },
            { envOn, on }, { envSensitivity, 0.5f }, { envResonance, 3.5f }, { envMix, 1.0f },
            { fuzzOn, on }, { fuzzAmount, 0.5f }, { fuzzTone, 0.3f }, { fuzzBlend, 0.4f } } },

        { "Dub Deep", {
            { ampModel, vintage }, { ampCab, cab115 }, { ampGain, 3.0f }, { ampBass, 6.0f }, { ampMid, -2.0f },
            { ampMidFreq, 400.0f }, { ampTreble, -9.0f }, { ampBlend, 0.2f }, { ampMaster, -8.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 3.0f }, { compMakeup, 4.0f },
            { delayOn, on }, { delayTime, 450.0f }, { delayFeedback, 0.45f }, { delayMix, 0.2f },
            { reverbOn, on }, { reverbSize, 0.6f }, { reverbMix, 0.15f } } },

        { "Ambient Chorus", {
            { ampModel, clean }, { ampCab, cab410 }, { ampGain, 2.5f }, { ampBass, 1.0f }, { ampTreble, 2.0f },
            { ampBlend, 0.3f }, { ampMaster, 0.5f },
            { compOn, on }, { compThreshold, -18.0f }, { compRatio, 3.0f }, { compMakeup, 4.0f },
            { chorusOn, on }, { chorusRate, 0.5f }, { chorusDepth, 0.6f }, { chorusMix, 0.5f },
            { delayOn, on }, { delayTime, 500.0f }, { delayFeedback, 0.4f }, { delayMix, 0.3f },
            { reverbOn, on }, { reverbSize, 0.8f }, { reverbMix, 0.35f } } },
    };

    return presets;
}
