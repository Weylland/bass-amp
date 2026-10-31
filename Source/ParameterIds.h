#pragma once

#include <array>

namespace ParamIds
{
    inline constexpr auto gateOn        = "gate_on";
    inline constexpr auto gateThreshold = "gate_threshold";
    inline constexpr auto gateRelease   = "gate_release";

    inline constexpr auto compOn        = "comp_on";
    inline constexpr auto compThreshold = "comp_threshold";
    inline constexpr auto compRatio     = "comp_ratio";
    inline constexpr auto compAttack    = "comp_attack";
    inline constexpr auto compRelease   = "comp_release";
    inline constexpr auto compMakeup    = "comp_makeup";
    inline constexpr auto compMix       = "comp_mix";

    inline constexpr auto octOn  = "oct_on";
    inline constexpr auto octSub = "oct_sub";
    inline constexpr auto octDry = "oct_dry";
    inline constexpr auto octUp  = "oct_up";

    inline constexpr auto envOn          = "env_on";
    inline constexpr auto envSensitivity = "env_sens";
    inline constexpr auto envResonance   = "env_reso";
    inline constexpr auto envMix         = "env_mix";

    inline constexpr auto fuzzOn     = "fuzz_on";
    inline constexpr auto fuzzAmount = "fuzz_amount";
    inline constexpr auto fuzzTone   = "fuzz_tone";
    inline constexpr auto fuzzBlend  = "fuzz_blend";

    inline constexpr auto distOn    = "dist_on";
    inline constexpr auto distDrive = "dist_drive";
    inline constexpr auto distTone  = "dist_tone";
    inline constexpr auto distBlend = "dist_blend";

    inline constexpr auto ampModel   = "amp_model";
    inline constexpr auto ampCab     = "amp_cab";
    inline constexpr auto ampGain    = "amp_gain";
    inline constexpr auto ampBass    = "amp_bass";
    inline constexpr auto ampMid     = "amp_mid";
    inline constexpr auto ampMidFreq = "amp_mid_freq";
    inline constexpr auto ampTreble  = "amp_treble";
    inline constexpr auto ampMaster  = "amp_master";
    inline constexpr auto ampBlend   = "amp_blend";
    inline constexpr auto ampContour = "amp_contour";
    inline constexpr auto ampDeep    = "amp_deep";
    inline constexpr auto ampBright  = "amp_bright";

    inline constexpr auto eqOn   = "eq_on";
    inline constexpr auto eqMode = "eq_mode";

    inline constexpr std::array<const char*, 9> graphicEq {
        "geq_33", "geq_63", "geq_125", "geq_250", "geq_500", "geq_1k", "geq_2k", "geq_4k", "geq_8k"
    };

    inline constexpr auto peqLowCut    = "peq_low_cut";
    inline constexpr auto peqLowFreq   = "peq_low_freq";
    inline constexpr auto peqLowGain   = "peq_low_gain";
    inline constexpr auto peqMid1Freq  = "peq_mid1_freq";
    inline constexpr auto peqMid1Gain  = "peq_mid1_gain";
    inline constexpr auto peqMid1Q     = "peq_mid1_q";
    inline constexpr auto peqMid2Freq  = "peq_mid2_freq";
    inline constexpr auto peqMid2Gain  = "peq_mid2_gain";
    inline constexpr auto peqMid2Q     = "peq_mid2_q";
    inline constexpr auto peqHighFreq  = "peq_high_freq";
    inline constexpr auto peqHighGain  = "peq_high_gain";
    inline constexpr auto peqHighCut   = "peq_high_cut";

    inline constexpr auto phaserOn       = "phaser_on";
    inline constexpr auto phaserRate     = "phaser_rate";
    inline constexpr auto phaserDepth    = "phaser_depth";
    inline constexpr auto phaserFeedback = "phaser_feedback";

    inline constexpr auto chorusOn    = "chorus_on";
    inline constexpr auto chorusRate  = "chorus_rate";
    inline constexpr auto chorusDepth = "chorus_depth";
    inline constexpr auto chorusMix   = "chorus_mix";

    inline constexpr auto delayOn       = "delay_on";
    inline constexpr auto delayTime     = "delay_time";
    inline constexpr auto delayFeedback = "delay_feedback";
    inline constexpr auto delayMix      = "delay_mix";

    inline constexpr auto reverbOn      = "reverb_on";
    inline constexpr auto reverbSize    = "reverb_size";
    inline constexpr auto reverbDamping = "reverb_damping";
    inline constexpr auto reverbMix     = "reverb_mix";
}
