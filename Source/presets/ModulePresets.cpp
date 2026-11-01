#include "ModulePresets.h"
#include "PresetManager.h"
#include "../PedalChain.h"

#include <map>
#include <optional>

namespace
{
    using namespace ParamIds;

    constexpr float hiFi = 0.0f, superValve = 1.0f, nordicDrive = 2.0f, tweed = 3.0f, flipTop = 4.0f, westCoast = 5.0f, stompDi = 6.0f;
    constexpr float cabOff = 0.0f, cab810 = 1.0f, cab410 = 2.0f, cab115 = 3.0f, cab212 = 4.0f, cab210 = 5.0f;
    constexpr float on = 1.0f, advanced = 1.0f;
    constexpr float lp12 = 0.0f, lp24 = 1.0f, bp24 = 3.0f, hp24 = 5.0f;
    constexpr float envLp = 0.0f, envBp = 1.0f, sweepDown = 1.0f;
    constexpr float square = 0.0f, saw = 1.0f;

    using PresetList = std::vector<FactoryPreset>;

    const PresetList ampPresets {
        { "Clean Hi-Fi", { { ampModel, hiFi }, { ampCab, cab410 }, { ampGain, 3.0f }, { ampBass, 1.0f }, { ampTreble, 1.0f },
                           { ampBlend, 0.3f }, { ampMaster, -2.0f } } },
        { "Tube Rock", { { ampModel, superValve }, { ampCab, cab810 }, { ampGain, 5.0f }, { ampBass, 3.0f }, { ampMid, 2.0f },
                         { ampMidFreq, 450.0f }, { ampTreble, -1.0f }, { ampBlend, 0.3f }, { ampMaster, -3.0f } } },
        { "Tube Ultra", { { ampModel, superValve }, { ampCab, cab810 }, { ampGain, 4.0f }, { ampBass, 2.0f }, { ampTreble, 2.0f },
                          { ampDeep, on }, { ampBright, on }, { ampBlend, 0.3f }, { ampMaster, -2.1f } } },
        { "Motown", { { ampModel, flipTop }, { ampCab, cab115 }, { ampGain, 3.0f }, { ampBass, 3.0f }, { ampMid, 1.0f },
                      { ampMidFreq, 300.0f }, { ampTreble, -6.0f }, { ampBlend, 0.15f }, { ampMaster, -1.2f } } },
        { "Tweed Break-Up", { { ampModel, tweed }, { ampCab, cab212 }, { ampGain, 6.5f }, { ampBass, 6.0f }, { ampMid, 2.0f },
                              { ampTreble, 2.0f }, { ampBlend, 0.25f }, { ampMaster, -0.6f } } },
        { "Rock Punch", { { ampModel, westCoast }, { ampCab, cab410 }, { ampGain, 5.0f }, { ampBass, 3.0f }, { ampMid, -2.0f },
                          { ampMidFreq, 500.0f }, { ampTreble, 4.0f }, { ampContour, 0.4f }, { ampBright, on }, { ampBlend, 0.3f },
                          { ampMaster, -1.2f } } },
        { "Slap Smile", { { ampModel, hiFi }, { ampCab, cab210 }, { ampGain, 3.0f }, { ampBass, 2.0f }, { ampTreble, 3.0f },
                          { ampContour, 0.7f }, { ampBlend, 0.4f }, { ampMaster, -0.9f } } },
        { "Jazz Warm", { { ampModel, hiFi }, { ampCab, cab115 }, { ampGain, 2.5f }, { ampBass, 2.0f }, { ampMid, 1.5f },
                         { ampMidFreq, 300.0f }, { ampTreble, -3.0f }, { ampBlend, 0.3f }, { ampMaster, -2.8f },
                         { ampHighCut, 5000.0f } } },
        { "Studio DI", { { ampModel, stompDi }, { ampCab, cabOff }, { ampGain, 4.0f }, { ampBass, 2.0f }, { ampMid, 2.0f },
                         { ampMidFreq, 1000.0f }, { ampTreble, 1.0f }, { ampMaster, -6.6f } } },
        { "DI Grit", { { ampModel, stompDi }, { ampCab, cab410 }, { ampGain, 7.0f }, { ampBass, 1.0f }, { ampMid, 3.0f },
                       { ampMidFreq, 800.0f }, { ampTreble, 2.0f }, { ampBlend, 0.35f }, { ampMaster, -4.9f } } },
        { "Metal Growl", { { ampModel, nordicDrive }, { ampCab, cab410 }, { ampGain, 7.0f }, { ampBass, 2.0f }, { ampMid, 3.0f },
                           { ampMidFreq, 1000.0f }, { ampTreble, 3.0f }, { ampBlend, 0.35f }, { ampMaster, -3.9f },
                           { ampLowCut, 35.0f } } },
    };

    const PresetList eqPresets {
        { "Flat", {} },
        { "Vocals", { { eqMode, advanced }, { peqLowCut, 90.0f }, { peqMid1Freq, 300.0f }, { peqMid1Gain, -3.0f },
                      { peqMid2Freq, 3000.0f }, { peqMid2Gain, 3.0f }, { peqHighFreq, 10000.0f }, { peqHighGain, 2.0f } } },
        { "Electric Guitar", { { eqMode, advanced }, { peqLowCut, 90.0f }, { peqMid1Freq, 400.0f }, { peqMid1Gain, -2.0f },
                               { peqMid2Freq, 2500.0f }, { peqMid2Gain, 3.0f }, { peqHighCut, 9000.0f } } },
        { "Acoustic Guitar", { { eqMode, advanced }, { peqLowCut, 80.0f }, { peqMid1Freq, 250.0f }, { peqMid1Gain, -4.0f },
                               { peqMid1Q, 1.2f }, { peqMid2Freq, 3500.0f }, { peqMid2Gain, 2.0f }, { peqHighFreq, 8000.0f },
                               { peqHighGain, 3.0f } } },
        { "Round Bass", { { graphicEq[1], 3.0f }, { graphicEq[2], 2.0f }, { graphicEq[3], -1.0f }, { graphicEq[7], -2.0f },
                          { graphicEq[8], -4.0f } } },
        { "Slap Smile", { { graphicEq[0], 2.0f }, { graphicEq[1], 4.0f }, { graphicEq[2], 2.0f }, { graphicEq[3], -2.0f },
                          { graphicEq[4], -4.0f }, { graphicEq[5], -3.0f }, { graphicEq[6], 2.0f }, { graphicEq[7], 4.0f },
                          { graphicEq[8], 3.0f } } },
        { "Rock Growl", { { graphicEq[0], -3.0f }, { graphicEq[2], 1.0f }, { graphicEq[4], 2.0f }, { graphicEq[5], 4.0f },
                          { graphicEq[6], 3.0f } } },
        { "Fingerstyle Warm", { { graphicEq[1], 2.0f }, { graphicEq[2], 3.0f }, { graphicEq[6], -2.0f }, { graphicEq[7], -3.0f },
                                { graphicEq[8], -6.0f } } },
        { "Anti-Mud", { { eqMode, advanced }, { peqLowCut, 40.0f }, { peqMid1Freq, 250.0f }, { peqMid1Gain, -5.0f },
                        { peqMid1Q, 1.5f } } },
        { "Old Radio", { { eqMode, advanced }, { peqLowCut, 300.0f }, { peqMid2Freq, 1500.0f }, { peqMid2Gain, 6.0f },
                         { peqMid2Q, 0.8f }, { peqHighCut, 3500.0f } } },
    };

    const std::map<juce::String, PresetList> pedalPresets {
        { "gate", {
            { "Light", { { gateThreshold, -70.0f }, { gateRelease, 150.0f } } },
            { "Tight", { { gateThreshold, -55.0f }, { gateRelease, 40.0f } } },
            { "Hard", { { gateThreshold, -45.0f }, { gateRelease, 20.0f } } } } },
        { "comp", {
            { "Gentle", { { compThreshold, -20.0f }, { compRatio, 2.5f }, { compAttack, 20.0f }, { compRelease, 200.0f },
                          { compMakeup, 3.0f } } },
            { "Studio", { { compThreshold, -18.0f }, { compRatio, 3.0f }, { compAttack, 15.0f }, { compMakeup, 4.0f } } },
            { "Slap", { { compThreshold, -24.0f }, { compRatio, 6.0f }, { compAttack, 2.0f }, { compRelease, 100.0f },
                        { compMakeup, 8.0f }, { compMix, 0.7f } } },
            { "Parallel", { { compThreshold, -35.0f }, { compRatio, 8.0f }, { compAttack, 5.0f }, { compMakeup, 10.0f },
                            { compMix, 0.5f } } },
            { "Limiter", { { compThreshold, -10.0f }, { compRatio, 20.0f }, { compAttack, 0.5f }, { compRelease, 80.0f },
                           { compMakeup, 6.0f } } } } },
        { "octaver", {
            { "Sub Octave", { { octSub, 0.7f }, { octDry, 0.8f } } },
            { "Sub Only", { { octSub, 1.0f }, { octDry, 0.0f } } },
            { "Organ", { { octSub, 0.6f }, { octDry, 0.6f }, { octUp, 0.5f } } },
            { "Octave Up", { { octSub, 0.0f }, { octDry, 0.7f }, { octUp, 0.7f } } } } },
        { "synth", {
            { "Square Lead", { { synthWave, square }, { synthCutoff, 600.0f }, { synthResonance, 0.5f }, { synthEnvelope, 0.6f },
                               { synthMix, 0.7f } } },
            { "Saw Bass", { { synthWave, saw }, { synthCutoff, 400.0f }, { synthResonance, 0.4f }, { synthEnvelope, 0.7f },
                            { synthMix, 0.8f } } },
            { "Squelch", { { synthWave, square }, { synthCutoff, 250.0f }, { synthResonance, 0.85f }, { synthEnvelope, 0.9f },
                           { synthMix, 0.8f } } },
            { "Subtle", { { synthWave, saw }, { synthCutoff, 1200.0f }, { synthResonance, 0.3f }, { synthEnvelope, 0.3f },
                          { synthMix, 0.35f } } } } },
        { "pitch", {
            { "Octave Up", { { pitchShift, 12.0f }, { pitchMix, 0.5f } } },
            { "Octave Down", { { pitchShift, -12.0f }, { pitchMix, 0.6f } } },
            { "Fifth", { { pitchShift, 7.0f }, { pitchMix, 0.4f } } },
            { "Detune", { { pitchShift, 0.0f }, { pitchDetune, 12.0f }, { pitchMix, 0.5f } } } } },
        { "filter", {
            { "Low Pass", { { filterType, lp24 }, { filterCutoff, 800.0f }, { filterResonance, 0.4f } } },
            { "Slow Sweep", { { filterType, lp24 }, { filterCutoff, 500.0f }, { filterResonance, 0.6f }, { filterRate, 0.2f },
                              { filterDepth, 0.7f } } },
            { "Wobble", { { filterType, lp12 }, { filterCutoff, 400.0f }, { filterResonance, 0.7f }, { filterRate, 4.0f },
                          { filterDepth, 0.8f } } },
            { "Band Pass", { { filterType, bp24 }, { filterCutoff, 900.0f }, { filterResonance, 0.5f } } },
            { "Thin", { { filterType, hp24 }, { filterCutoff, 300.0f }, { filterResonance, 0.2f } } } } },
        { "envelope", {
            { "Funk", { { envSensitivity, 0.6f }, { envResonance, 4.0f }, { envType, envLp } } },
            { "Quack", { { envSensitivity, 0.7f }, { envResonance, 6.0f }, { envType, envBp }, { envMix, 0.9f } } },
            { "Down Sweep", { { envSensitivity, 0.6f }, { envResonance, 3.0f }, { envType, envLp }, { envDirection, sweepDown } } },
            { "Subtle", { { envSensitivity, 0.4f }, { envResonance, 2.0f }, { envMix, 0.5f } } } } },
        { "wah", {
            { "Cocked", { { wahPedal, 0.6f }, { wahResonance, 5.0f }, { wahBlend, 0.25f } } },
            { "Vocal", { { wahPedal, 0.45f }, { wahResonance, 8.0f }, { wahBlend, 0.1f } } },
            { "Dark", { { wahPedal, 0.25f }, { wahResonance, 4.0f }, { wahBlend, 0.4f } } } } },
        { "fuzz", {
            { "Vintage", { { fuzzAmount, 0.5f }, { fuzzTone, 0.4f }, { fuzzBlend, 0.6f } } },
            { "Wall", { { fuzzAmount, 1.0f }, { fuzzTone, 0.6f }, { fuzzBlend, 0.9f } } },
            { "Bassy", { { fuzzAmount, 0.7f }, { fuzzTone, 0.3f }, { fuzzBlend, 0.4f } } },
            { "Bright Lead", { { fuzzAmount, 0.8f }, { fuzzTone, 0.8f }, { fuzzBlend, 0.8f } } } } },
        { "drive", {
            { "Light Crunch", { { distDrive, 0.3f }, { distTone, 2500.0f }, { distBlend, 0.4f } } },
            { "Rock", { { distDrive, 0.6f }, { distTone, 2200.0f }, { distBlend, 0.6f } } },
            { "Grind", { { distDrive, 0.9f }, { distTone, 3500.0f }, { distBlend, 0.7f } } },
            { "Warm", { { distDrive, 0.8f }, { distTone, 1200.0f }, { distBlend, 0.5f } } } } },
        { "phaser", {
            { "Slow Swirl", { { phaserRate, 0.2f }, { phaserDepth, 0.7f }, { phaserFeedback, 0.3f } } },
            { "Fast Vibe", { { phaserRate, 3.0f }, { phaserDepth, 0.5f }, { phaserFeedback, 0.2f } } },
            { "Jet", { { phaserRate, 0.4f }, { phaserDepth, 0.9f }, { phaserFeedback, 0.8f } } },
            { "Subtle", { { phaserRate, 0.5f }, { phaserDepth, 0.35f }, { phaserFeedback, 0.1f } } } } },
        { "flanger", {
            { "Jet", { { flangerRate, 0.2f }, { flangerDepth, 0.9f }, { flangerFeedback, 0.8f }, { flangerMix, 0.5f } } },
            { "Metallic", { { flangerRate, 1.5f }, { flangerDepth, 0.6f }, { flangerFeedback, 0.85f }, { flangerMix, 0.6f } } },
            { "Slow Wash", { { flangerRate, 0.1f }, { flangerDepth, 0.8f }, { flangerFeedback, 0.4f }, { flangerMix, 0.4f } } },
            { "Subtle", { { flangerRate, 0.4f }, { flangerDepth, 0.4f }, { flangerFeedback, 0.2f }, { flangerMix, 0.3f } } } } },
        { "chorus", {
            { "Subtle", { { chorusRate, 0.6f }, { chorusDepth, 0.3f }, { chorusMix, 0.3f } } },
            { "Fretless", { { chorusRate, 0.8f }, { chorusDepth, 0.5f }, { chorusMix, 0.5f } } },
            { "Wide", { { chorusRate, 1.5f }, { chorusDepth, 0.7f }, { chorusMix, 0.6f } } },
            { "Vibrato", { { chorusRate, 4.5f }, { chorusDepth, 0.4f }, { chorusMix, 1.0f } } } } },
        { "tremolo", {
            { "Gentle", { { tremoloRate, 4.0f }, { tremoloDepth, 0.4f } } },
            { "Surf", { { tremoloRate, 6.0f }, { tremoloDepth, 0.7f }, { tremoloShape, 0.2f } } },
            { "Chop", { { tremoloRate, 8.0f }, { tremoloDepth, 1.0f }, { tremoloShape, 1.0f } } },
            { "Slow Pulse", { { tremoloRate, 1.5f }, { tremoloDepth, 0.6f }, { tremoloShape, 0.4f } } } } },
        { "delay", {
            { "Slapback", { { delayTime, 110.0f }, { delayFeedback, 0.1f }, { delayMix, 0.3f } } },
            { "Doubler", { { delayTime, 30.0f }, { delayFeedback, 0.0f }, { delayMix, 0.4f } } },
            { "Dub", { { delayTime, 375.0f }, { delayFeedback, 0.65f }, { delayMix, 0.4f } } },
            { "Ambient", { { delayTime, 700.0f }, { delayFeedback, 0.55f }, { delayMix, 0.35f } } } } },
        { "reverb", {
            { "Room", { { reverbSize, 0.35f }, { reverbDamping, 0.5f }, { reverbMix, 0.2f } } },
            { "Plate", { { reverbSize, 0.6f }, { reverbDamping, 0.2f }, { reverbMix, 0.3f } } },
            { "Hall", { { reverbSize, 0.75f }, { reverbDamping, 0.4f }, { reverbMix, 0.3f } } },
            { "Ambient", { { reverbSize, 0.95f }, { reverbDamping, 0.3f }, { reverbMix, 0.5f } } } } },
    };

    struct Unit
    {
        juce::StringArray prefixes;
        juce::String toggleId;
    };

    Unit unitFor (const juce::String& module)
    {
        if (module == "amp")
            return { { "amp_" } };

        if (module == "eq")
            return { { "eq_", "geq_", "peq_" }, eqOn };

        for (const auto& info : pedalInfos)
            if (module == info.key)
                return { { juce::String (info.toggleId).dropLastCharacters (2) }, info.toggleId };

        jassertfalse;
        return {};
    }

    template <typename Values, typename Function>
    void forEachTarget (const juce::AudioProcessorValueTreeState& state, const juce::String& module, const Values& values,
                        Function&& function)
    {
        const auto unit = unitFor (module);

        for (auto* parameter : state.processor.getParameters())
        {
            auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (parameter);

            if (ranged == nullptr || ranged->getParameterID() == unit.toggleId
                || ! std::any_of (unit.prefixes.begin(), unit.prefixes.end(),
                                  [ranged] (const juce::String& prefix) { return ranged->getParameterID().startsWith (prefix); }))
                continue;

            auto target = ranged->getDefaultValue();

            for (const auto& [paramId, value] : values)
                if (ranged->getParameterID() == paramId)
                    target = ranged->convertTo0to1 (value);

            function (*ranged, target);
        }
    }
}

const std::vector<FactoryPreset>& getModulePresets (const juce::String& module)
{
    static const PresetList none;

    if (module == "amp")
        return ampPresets;

    if (module == "eq")
        return eqPresets;

    const auto found = pedalPresets.find (module);
    return found != pedalPresets.end() ? found->second : none;
}

void applyModulePreset (juce::AudioProcessorValueTreeState& state, const juce::String& module, const FactoryPreset& preset)
{
    forEachTarget (state, module, preset.values, [] (juce::RangedAudioParameter& parameter, float target)
                   { parameter.setValueNotifyingHost (target); });

    if (auto* toggle = state.getParameter (unitFor (module).toggleId))
        toggle->setValueNotifyingHost (1.0f);
}

bool isModulePresetActive (const juce::AudioProcessorValueTreeState& state, const juce::String& module, const FactoryPreset& preset)
{
    auto active = true;
    forEachTarget (state, module, preset.values, [&active] (juce::RangedAudioParameter& parameter, float target)
                   { active = active && std::abs (parameter.getValue() - target) < 0.002f; });
    return active;
}

namespace
{
    constexpr const char* moduleExtension = ".xml";
    const juce::Identifier moduleTag { "MODULE_PRESET" }, paramTag { "PARAM" }, idAttribute { "id" }, valueAttribute { "value" };

    std::array<juce::Identifier, 2> filesOf (const juce::String& module)
    {
        if (module == "amp")
            return { PresetManager::impulseResponseProperty, PresetManager::captureProperty };

        return {};
    }

    struct UserValues
    {
        std::vector<std::pair<juce::String, float>> values;
        juce::NamedValueSet files;
    };

    std::optional<UserValues> readUserPreset (const juce::File& file)
    {
        const auto xml = juce::XmlDocument::parse (file);

        if (xml == nullptr || ! xml->hasTagName (moduleTag.toString()))
            return std::nullopt;

        UserValues preset;

        for (const auto* param : xml->getChildWithTagNameIterator (paramTag.toString()))
            preset.values.emplace_back (param->getStringAttribute (idAttribute), (float) param->getDoubleAttribute (valueAttribute));

        for (int i = 0; i < xml->getNumAttributes(); ++i)
            preset.files.set (xml->getAttributeName (i), xml->getAttributeValue (i));

        return preset;
    }
}

juce::File getDefaultModulePresetDirectory()
{
    return PresetManager::getDefaultUserDirectory().getChildFile ("Modules");
}

juce::Array<juce::File> getUserModulePresets (const juce::String& module, const juce::File& root)
{
    auto files = root.getChildFile (module).findChildFiles (juce::File::findFiles, false, juce::String ("*") + moduleExtension);
    files.sort();
    return files;
}

juce::File getUserModulePresetFile (const juce::String& module, const juce::String& name, const juce::File& root)
{
    return root.getChildFile (module).getChildFile (juce::File::createLegalFileName (name.trim()) + moduleExtension);
}

juce::File saveUserModulePreset (const juce::AudioProcessorValueTreeState& state, const juce::String& module, const juce::String& name,
                                 const juce::File& root)
{
    const auto file = getUserModulePresetFile (module, name, root);

    if (juce::File::createLegalFileName (name.trim()).isEmpty() || ! file.getParentDirectory().createDirectory())
        return {};

    juce::XmlElement xml (moduleTag);
    const std::vector<std::pair<juce::String, float>> none;

    forEachTarget (state, module, none, [&xml] (juce::RangedAudioParameter& parameter, float)
    {
        auto* param = xml.createNewChildElement (paramTag.toString());
        param->setAttribute (idAttribute, parameter.getParameterID());
        param->setAttribute (valueAttribute, parameter.convertFrom0to1 (parameter.getValue()));
    });

    for (const auto& property : filesOf (module))
        if (property.isValid())
            xml.setAttribute (property, state.state.getProperty (property).toString());

    return xml.writeTo (file) ? file : juce::File();
}

bool loadUserModulePreset (juce::AudioProcessorValueTreeState& state, const juce::String& module, const juce::File& file)
{
    const auto preset = readUserPreset (file);

    if (! preset.has_value())
        return false;

    forEachTarget (state, module, preset->values, [] (juce::RangedAudioParameter& parameter, float target)
                   { parameter.setValueNotifyingHost (target); });

    for (const auto& property : filesOf (module))
        if (property.isValid() && preset->files.contains (property))
            state.state.setProperty (property, preset->files[property], nullptr);

    if (auto* toggle = state.getParameter (unitFor (module).toggleId))
        toggle->setValueNotifyingHost (1.0f);

    return true;
}

bool isUserModulePresetActive (const juce::AudioProcessorValueTreeState& state, const juce::String& module, const juce::File& file)
{
    const auto preset = readUserPreset (file);

    if (! preset.has_value())
        return false;

    auto active = true;
    forEachTarget (state, module, preset->values, [&active] (juce::RangedAudioParameter& parameter, float target)
                   { active = active && std::abs (parameter.getValue() - target) < 0.002f; });

    for (const auto& property : filesOf (module))
        if (property.isValid())
            active = active && state.state.getProperty (property).toString() == preset->files[property].toString();

    return active;
}
