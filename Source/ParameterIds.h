#pragma once

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

    inline constexpr auto octOn  = "oct_on";
    inline constexpr auto octSub = "oct_sub";
    inline constexpr auto octDry = "oct_dry";

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

    inline constexpr auto ampGain   = "amp_gain";
    inline constexpr auto ampBass   = "amp_bass";
    inline constexpr auto ampMid    = "amp_mid";
    inline constexpr auto ampTreble = "amp_treble";
    inline constexpr auto ampMaster = "amp_master";

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
