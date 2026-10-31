#include "FactoryPresets.h"
#include "../ParameterIds.h"

namespace
{
    using namespace ParamIds;

    constexpr float hiFi = 0.0f, tube300 = 1.0f, crossover = 2.0f, tweed = 3.0f, flipTop = 4.0f, solidPunch = 5.0f, tubeDi = 6.0f;
    constexpr float cabOff = 0.0f, cab810 = 1.0f, cab410 = 2.0f, cab115 = 3.0f, cab212 = 4.0f, cab210 = 5.0f;
    constexpr float on = 1.0f, advanced = 1.0f;
}

const std::vector<FactoryPreset>& getFactoryPresets()
{
    static const std::vector<FactoryPreset> presets {
        // Classic tube rock: big 8x10, low-mid push from the mid selector at 450 Hz.
        { "Tube 300 Rock", {
            { ampModel, tube300 }, { ampCab, cab810 }, { ampGain, 5.0f }, { ampBass, 3.0f }, { ampMid, 2.0f },
            { ampMidFreq, 450.0f }, { ampTreble, -1.0f }, { ampBlend, 0.3f }, { ampMaster, -3.5f },
            { compOn, on }, { compThreshold, -18.0f }, { compRatio, 3.0f }, { compAttack, 15.0f }, { compMakeup, 4.0f } } },

        // The same head with both voicing switches: deep lows, scooped low-mids, sparkling top.
        { "Tube 300 Ultra", {
            { ampModel, tube300 }, { ampCab, cab810 }, { ampGain, 4.0f }, { ampBass, 2.0f }, { ampTreble, 2.0f },
            { ampDeep, on }, { ampBright, on }, { ampBlend, 0.3f }, { ampMaster, -4.0f },
            { compOn, on }, { compThreshold, -18.0f }, { compRatio, 3.0f }, { compAttack, 15.0f }, { compMakeup, 4.0f } } },

        // Small studio tube amp, flatwound thump: round lows, treble rolled off, slow compressor.
        { "Motown Flip Top", {
            { ampModel, flipTop }, { ampCab, cab115 }, { ampGain, 3.0f }, { ampBass, 3.0f }, { ampMid, 1.0f },
            { ampMidFreq, 300.0f }, { ampTreble, -6.0f }, { ampBlend, 0.15f }, { ampMaster, -4.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 3.0f }, { compAttack, 25.0f }, { compRelease, 300.0f },
            { compMakeup, 4.0f } } },

        // Tweed amp pushed into break-up through a 2x12.
        { "Fifties Tweed", {
            { ampModel, tweed }, { ampCab, cab212 }, { ampGain, 6.5f }, { ampBass, 6.0f }, { ampMid, 2.0f },
            { ampTreble, 2.0f }, { ampBlend, 0.25f }, { ampMaster, -2.2f } } },

        // Solid-state rock head: contour scoop, high boost, punchy 4x10.
        { "Rock Punch 4x10", {
            { ampModel, solidPunch }, { ampCab, cab410 }, { ampGain, 5.0f }, { ampBass, 3.0f }, { ampMid, -2.0f },
            { ampMidFreq, 500.0f }, { ampTreble, 4.0f }, { ampContour, 0.4f }, { ampBright, on }, { ampBlend, 0.3f },
            { ampMaster, 0.4f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 4.0f }, { compAttack, 5.0f }, { compMakeup, 5.0f } } },

        // Hi-fi head with the pre-shape contour: the slap smile curve.
        { "Slap Hi-Fi", {
            { ampModel, hiFi }, { ampCab, cab210 }, { ampGain, 3.0f }, { ampBass, 2.0f }, { ampTreble, 3.0f },
            { ampContour, 0.7f }, { ampBlend, 0.4f }, { ampMaster, 0.0f },
            { gateOn, on }, { gateThreshold, -65.0f },
            { compOn, on }, { compThreshold, -24.0f }, { compRatio, 6.0f }, { compAttack, 2.0f }, { compRelease, 100.0f },
            { compMakeup, 8.0f }, { compMix, 0.7f } } },

        // Hi-fi head with a vintage high cut: warm, woody fingerstyle tone.
        { "Jazz Warm", {
            { ampModel, hiFi }, { ampCab, cab115 }, { ampGain, 2.5f }, { ampBass, 2.0f }, { ampMid, 1.5f },
            { ampMidFreq, 300.0f }, { ampTreble, -3.0f }, { ampBlend, 0.3f }, { ampMaster, -3.9f },
            { eqOn, on }, { eqMode, advanced }, { peqHighCut, 3500.0f }, { peqLowGain, 2.0f }, { peqLowFreq, 70.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 2.5f }, { compAttack, 20.0f }, { compMakeup, 3.0f } } },

        // Tube-emulation DI straight to the desk, as on countless records.
        { "Tube DI Studio", {
            { ampModel, tubeDi }, { ampCab, cabOff }, { ampGain, 4.0f }, { ampBass, 2.0f }, { ampMid, 2.0f },
            { ampMidFreq, 1000.0f }, { ampTreble, 1.0f }, { ampMaster, -4.1f },
            { compOn, on }, { compThreshold, -18.0f }, { compRatio, 3.0f }, { compAttack, 10.0f }, { compMakeup, 4.0f } } },

        // The same DI driven hard for punk and garage rock.
        { "Tube DI Grit", {
            { ampModel, tubeDi }, { ampCab, cab410 }, { ampGain, 7.0f }, { ampBass, 1.0f }, { ampMid, 3.0f },
            { ampMidFreq, 800.0f }, { ampTreble, 2.0f }, { ampBlend, 0.35f }, { ampMaster, 0.1f },
            { gateOn, on }, { gateThreshold, -62.0f } } },

        // Crossover preamp: tight clean lows under a growling top, upper-mid attack from the EQ.
        { "Crossover Metal", {
            { ampModel, crossover }, { ampCab, cab410 }, { ampGain, 7.0f }, { ampBass, 2.0f }, { ampMid, 3.0f },
            { ampMidFreq, 1000.0f }, { ampTreble, 3.0f }, { ampBlend, 0.35f }, { ampMaster, -2.2f },
            { gateOn, on }, { gateThreshold, -58.0f }, { gateRelease, 60.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 4.0f }, { compAttack, 8.0f }, { compMakeup, 5.0f },
            { eqOn, on }, { eqMode, advanced }, { peqLowCut, 35.0f }, { peqMid2Freq, 2500.0f }, { peqMid2Gain, 3.0f } } },

        // Modern djent: scooped graphic EQ, clanky top, gated.
        { "Crossover Djent", {
            { ampModel, crossover }, { ampCab, cab810 }, { ampGain, 8.5f }, { ampBass, 3.0f }, { ampMid, -2.0f },
            { ampMidFreq, 700.0f }, { ampTreble, 5.0f }, { ampBlend, 0.4f }, { ampMaster, -4.6f },
            { gateOn, on }, { gateThreshold, -55.0f }, { gateRelease, 50.0f },
            { compOn, on }, { compThreshold, -22.0f }, { compRatio, 5.0f }, { compAttack, 5.0f }, { compMakeup, 6.0f },
            { eqOn, on }, { graphicEq[1], 3.0f }, { graphicEq[4], -3.0f }, { graphicEq[6], 2.0f }, { graphicEq[7], 4.0f } } },

        // Tube head cranked with a pick: honky mids, bright top.
        { "Punk Pick", {
            { ampModel, tube300 }, { ampCab, cab410 }, { ampGain, 7.0f }, { ampBass, 1.0f }, { ampMid, 4.0f },
            { ampMidFreq, 800.0f }, { ampTreble, 3.0f }, { ampBright, on }, { ampBlend, 0.3f }, { ampMaster, -1.7f } } },

        { "Fuzz Lead", {
            { ampModel, tube300 }, { ampCab, cab410 }, { ampGain, 4.0f }, { ampBass, 2.0f }, { ampMid, 3.0f },
            { ampMidFreq, 800.0f }, { ampBlend, 0.25f }, { ampMaster, -1.5f },
            { fuzzOn, on }, { fuzzAmount, 0.8f }, { fuzzTone, 0.6f }, { fuzzBlend, 0.7f },
            { delayOn, on }, { delayTime, 380.0f }, { delayFeedback, 0.3f }, { delayMix, 0.25f } } },

        { "Funk Envelope", {
            { ampModel, hiFi }, { ampCab, cab410 }, { ampGain, 3.0f }, { ampBass, 2.0f }, { ampBlend, 0.3f },
            { ampMaster, 0.0f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 4.0f }, { compAttack, 10.0f }, { compMakeup, 5.0f },
            { envOn, on }, { envSensitivity, 0.6f }, { envResonance, 4.0f }, { envMix, 1.0f } } },

        { "Synth Octave", {
            { ampModel, hiFi }, { ampCab, cab115 }, { ampGain, 3.0f }, { ampBass, 2.0f }, { ampBlend, 0.3f },
            { ampMaster, -4.0f },
            { gateOn, on }, { gateThreshold, -60.0f },
            { octOn, on }, { octSub, 0.9f }, { octDry, 0.3f },
            { envOn, on }, { envSensitivity, 0.5f }, { envResonance, 3.5f }, { envMix, 1.0f },
            { fuzzOn, on }, { fuzzAmount, 0.5f }, { fuzzTone, 0.3f }, { fuzzBlend, 0.4f } } },

        { "Dub Deep", {
            { ampModel, tube300 }, { ampCab, cab115 }, { ampGain, 3.0f }, { ampBass, 5.0f }, { ampMid, -2.0f },
            { ampMidFreq, 400.0f }, { ampTreble, -9.0f }, { ampDeep, on }, { ampBlend, 0.2f }, { ampMaster, -4.7f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 3.0f }, { compMakeup, 4.0f },
            { delayOn, on }, { delayTime, 450.0f }, { delayFeedback, 0.45f }, { delayMix, 0.2f },
            { reverbOn, on }, { reverbSize, 0.6f }, { reverbMix, 0.15f } } },

        // Fretless singing tone: mid push, phaser and a touch of chorus.
        { "Fretless Phase", {
            { ampModel, hiFi }, { ampCab, cab210 }, { ampGain, 3.0f }, { ampBass, 1.0f }, { ampMid, 3.0f },
            { ampMidFreq, 700.0f }, { ampTreble, -2.0f }, { ampBlend, 0.3f }, { ampMaster, 2.9f },
            { compOn, on }, { compThreshold, -20.0f }, { compRatio, 3.0f }, { compAttack, 15.0f }, { compMakeup, 4.0f },
            { phaserOn, on }, { phaserRate, 0.3f }, { phaserDepth, 0.5f }, { phaserFeedback, 0.3f },
            { chorusOn, on }, { chorusRate, 0.4f }, { chorusDepth, 0.3f }, { chorusMix, 0.3f },
            { reverbOn, on }, { reverbSize, 0.5f }, { reverbMix, 0.15f } } },

        { "Ambient Chorus", {
            { ampModel, hiFi }, { ampCab, cab410 }, { ampGain, 2.5f }, { ampBass, 1.0f }, { ampTreble, 2.0f },
            { ampBlend, 0.3f }, { ampMaster, 0.5f },
            { compOn, on }, { compThreshold, -18.0f }, { compRatio, 3.0f }, { compMakeup, 4.0f },
            { chorusOn, on }, { chorusRate, 0.5f }, { chorusDepth, 0.6f }, { chorusMix, 0.5f },
            { delayOn, on }, { delayTime, 500.0f }, { delayFeedback, 0.4f }, { delayMix, 0.3f },
            { reverbOn, on }, { reverbSize, 0.8f }, { reverbMix, 0.35f } } },
    };

    return presets;
}
