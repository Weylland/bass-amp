#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

BassAmpProcessor::BassAmpProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::mono(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    gate.bind (parameters);
    compressor.bind (parameters);
    octaver.bind (parameters);
    envelopeFilter.bind (parameters);
    fuzz.bind (parameters);
    distortion.bind (parameters);
    amp.bind (parameters);
    chorus.bind (parameters);
    delay.bind (parameters);
    reverb.bind (parameters);

    presets.loadFactoryPreset (0);
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
    const juce::dsp::ProcessSpec monoSpec { sampleRate, maxBlockSize, 1 };
    const juce::dsp::ProcessSpec outputSpec { sampleRate, maxBlockSize, (juce::uint32) juce::jmax (1, getTotalNumOutputChannels()) };

    tuner.prepare (sampleRate);
    gate.prepare (sampleRate);
    compressor.prepare (sampleRate);
    octaver.prepare (monoSpec);
    envelopeFilter.prepare (monoSpec);
    fuzz.prepare (sampleRate, samplesPerBlock);
    distortion.prepare (sampleRate, samplesPerBlock);
    amp.prepare (sampleRate, samplesPerBlock);
    chorus.prepare (outputSpec);
    delay.prepare (outputSpec);
    reverb.prepare (outputSpec);
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

    inputMeter.push (buffer.getReadPointer (0), numSamples);
    tuner.push (buffer.getReadPointer (0), numSamples);

    juce::dsp::AudioBlock<float> block (buffer);
    auto mono = block.getSingleChannelBlock (0);

    gate.process (mono);
    compressor.process (mono);
    octaver.process (mono);
    envelopeFilter.process (mono);
    fuzz.process (mono);
    distortion.process (mono);
    amp.process (mono);

    for (int channel = 1; channel < numOutputs; ++channel)
        buffer.copyFrom (channel, 0, buffer, 0, 0, numSamples);

    auto outputBlock = block.getSubsetChannelBlock (0, (size_t) numOutputs);
    const juce::dsp::ProcessContextReplacing<float> context (outputBlock);
    chorus.process (context);
    delay.process (context);
    reverb.process (context);

    for (int channel = 0; channel < numOutputs; ++channel)
        outputMeter.push (buffer.getReadPointer (channel), numSamples);

    if (muteOutput.load())
        outputBlock.clear();
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
