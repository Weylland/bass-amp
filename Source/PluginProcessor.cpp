#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIds.h"

namespace
{
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
}

BassAmpProcessor::BassAmpProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::mono(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    auto get = [this] (const char* id) { return parameters.getRawParameterValue (id); };

    raw = { get (ParamIds::ampGain),   get (ParamIds::ampBass),     get (ParamIds::ampMid),
            get (ParamIds::ampTreble), get (ParamIds::ampMaster),
            get (ParamIds::distOn),    get (ParamIds::distDrive),   get (ParamIds::distTone),  get (ParamIds::distBlend),
            get (ParamIds::chorusOn),  get (ParamIds::chorusRate),  get (ParamIds::chorusDepth), get (ParamIds::chorusMix),
            get (ParamIds::reverbOn),  get (ParamIds::reverbSize),  get (ParamIds::reverbDamping), get (ParamIds::reverbMix) };
}

juce::AudioProcessorValueTreeState::ParameterLayout BassAmpProcessor::createParameterLayout()
{
    using juce::NormalisableRange;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto addFloat = [&layout] (const char* id, const char* name, NormalisableRange<float> range, float defaultValue,
                               juce::AudioParameterFloatAttributes attributes)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, defaultValue, attributes));
    };

    auto addToggle = [&layout] (const char* id, const char* name)
    {
        layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { id, 1 }, name, false));
    };

    NormalisableRange<float> toneRange { 400.0f, 8000.0f, 1.0f };
    toneRange.setSkewForCentre (2000.0f);

    NormalisableRange<float> rateRange { 0.1f, 5.0f, 0.01f };
    rateRange.setSkewForCentre (1.0f);

    addFloat (ParamIds::ampGain,   "Gain",   { 0.0f, 10.0f, 0.01f }, 3.0f, withUnit ("", 1));
    addFloat (ParamIds::ampBass,   "Bass",   { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampMid,    "Mid",    { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampTreble, "Treble", { -12.0f, 12.0f, 0.1f }, 0.0f, withUnit ("dB", 1));
    addFloat (ParamIds::ampMaster, "Master", { -48.0f, 6.0f, 0.1f }, -6.0f, withUnit ("dB", 1));

    addToggle (ParamIds::distOn, "Disto On");
    addFloat (ParamIds::distDrive, "Drive", { 0.0f, 1.0f, 0.001f }, 0.5f, asPercent());
    addFloat (ParamIds::distTone,  "Tone",  toneRange, 2500.0f, withUnit ("Hz", 0));
    addFloat (ParamIds::distBlend, "Blend", { 0.0f, 1.0f, 0.001f }, 0.5f, asPercent());

    addToggle (ParamIds::chorusOn, "Chorus On");
    addFloat (ParamIds::chorusRate,  "Rate",  rateRange, 0.8f, withUnit ("Hz", 2));
    addFloat (ParamIds::chorusDepth, "Depth", { 0.0f, 1.0f, 0.001f }, 0.4f, asPercent());
    addFloat (ParamIds::chorusMix,   "Mix",   { 0.0f, 1.0f, 0.001f }, 0.5f, asPercent());

    addToggle (ParamIds::reverbOn, "Reverb On");
    addFloat (ParamIds::reverbSize,    "Size",    { 0.0f, 1.0f, 0.001f }, 0.5f, asPercent());
    addFloat (ParamIds::reverbDamping, "Damping", { 0.0f, 1.0f, 0.001f }, 0.5f, asPercent());
    addFloat (ParamIds::reverbMix,     "Mix",     { 0.0f, 1.0f, 0.001f }, 0.25f, asPercent());

    return layout;
}

bool BassAmpProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    auto isMonoOrStereo = [] (const juce::AudioChannelSet& set)
    {
        return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
    };

    return isMonoOrStereo (layouts.getMainInputChannelSet()) && isMonoOrStereo (layouts.getMainOutputChannelSet());
}

void BassAmpProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    const auto maxBlockSize = (juce::uint32) samplesPerBlock;
    const auto numOutputs = (juce::uint32) juce::jmax (1, getTotalNumOutputChannels());

    distortion.prepare (sampleRate, samplesPerBlock);
    amp.prepare ({ sampleRate, maxBlockSize, 1 });

    chorus.prepare ({ sampleRate, maxBlockSize, numOutputs });
    chorus.setCentreDelay (7.0f);
    chorus.setFeedback (0.0f);

    reverb.prepare ({ sampleRate, maxBlockSize, numOutputs });

    updateDsp();
    chorus.reset();
    reverb.reset();
}

void BassAmpProcessor::updateDsp()
{
    distortion.setParameters (raw.distOn->load() > 0.5f, raw.distDrive->load(), raw.distTone->load(), raw.distBlend->load());

    amp.setParameters (raw.ampGain->load(), raw.ampBass->load(), raw.ampMid->load(), raw.ampTreble->load(), raw.ampMaster->load());

    chorus.setRate (raw.chorusRate->load());
    chorus.setDepth (raw.chorusDepth->load());
    chorus.setMix (raw.chorusOn->load() > 0.5f ? raw.chorusMix->load() : 0.0f);

    // juce::Reverb scales dry by 2 and wet by 3: dryLevel 0.5 is unity.
    const auto reverbMix = raw.reverbOn->load() > 0.5f ? raw.reverbMix->load() : 0.0f;
    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = raw.reverbSize->load();
    reverbParams.damping = raw.reverbDamping->load();
    reverbParams.wetLevel = reverbMix * 0.66f;
    reverbParams.dryLevel = 0.5f * (1.0f - 0.4f * reverbMix);
    reverbParams.width = 1.0f;
    reverb.setParameters (reverbParams);
}

void BassAmpProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numInputs = getTotalNumInputChannels();
    const auto numOutputs = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    if (numInputs == 0 || numOutputs == 0)
    {
        buffer.clear();
        return;
    }

    updateDsp();

    juce::dsp::AudioBlock<float> block (buffer);
    auto mono = block.getSingleChannelBlock (0);

    distortion.process (mono);
    amp.process (mono);

    for (int channel = 1; channel < numOutputs; ++channel)
        buffer.copyFrom (channel, 0, buffer, 0, 0, numSamples);

    auto outputBlock = block.getSubsetChannelBlock (0, (size_t) numOutputs);
    juce::dsp::ProcessContextReplacing<float> context (outputBlock);
    chorus.process (context);
    reverb.process (context);
}

juce::AudioProcessorEditor* BassAmpProcessor::createEditor()
{
    return new BassAmpEditor (*this);
}

void BassAmpProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = parameters.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void BassAmpProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BassAmpProcessor();
}
