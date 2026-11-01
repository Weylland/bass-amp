#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

namespace
{
    // Saved with the session but not with presets: they belong to the setup, not to the sound.
    constexpr auto inputChannelAttribute = "inputChannel";
    constexpr auto editorWidthAttribute = "editorWidth";
}

BassAmpProcessor::BassAmpProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    gate.bind (parameters);
    compressor.bind (parameters);
    octaver.bind (parameters);
    synth.bind (parameters);
    pitchShifter.bind (parameters);
    filter.bind (parameters);
    envelopeFilter.bind (parameters);
    wah.bind (parameters);
    fuzz.bind (parameters);
    distortion.bind (parameters);
    amp.bind (parameters);
    equalizer.bind (parameters);
    quickCuts.bind (parameters);
    phaser.bind (parameters);
    flanger.bind (parameters);
    chorus.bind (parameters);
    tremolo.bind (parameters);
    delay.bind (parameters);
    reverb.bind (parameters);
    limiter.bind (parameters);

    midiLearn.onProgramChange = [this] (int program)
    {
        if (program < getNumPrograms())
            setCurrentProgram (program);
    };
    midiLearn.onAction = [this] (int action) { looper.trigger (action); };

    presets.loadFactoryPreset (0);
    undoHistory.clear();
    parameters.state.addListener (this);
}

BassAmpProcessor::~BassAmpProcessor()
{
    parameters.state.removeListener (this);
}

void BassAmpProcessor::setCabinetImpulseResponse (const juce::File& file)
{
    parameters.state.setProperty (PresetManager::impulseResponseProperty, file.getFullPathName(), nullptr);
}

juce::File BassAmpProcessor::getCabinetImpulseResponse() const
{
    const auto path = parameters.state.getProperty (PresetManager::impulseResponseProperty).toString();
    return juce::File::isAbsolutePath (path) ? juce::File (path) : juce::File();
}

void BassAmpProcessor::setCapture (const juce::File& file)
{
    parameters.state.setProperty (PresetManager::captureProperty, file.getFullPathName(), nullptr);
}

juce::File BassAmpProcessor::getCapture() const
{
    const auto path = parameters.state.getProperty (PresetManager::captureProperty).toString();
    return juce::File::isAbsolutePath (path) ? juce::File (path) : juce::File();
}

void BassAmpProcessor::valueTreePropertyChanged (juce::ValueTree& tree, const juce::Identifier& property)
{
    if (tree == parameters.state && property == PresetManager::impulseResponseProperty)
        reloadImpulseResponse();

    if (tree == parameters.state && property == PresetManager::captureProperty)
        amp.loadCapture (getCapture());
}

void BassAmpProcessor::valueTreeRedirected (juce::ValueTree&)
{
    reloadImpulseResponse();
    amp.loadCapture (getCapture());
}

void BassAmpProcessor::reloadImpulseResponse()
{
    amp.loadImpulseResponse (getCabinetImpulseResponse());
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
    spectrum.prepare (sampleRate);
    recorder.prepare (sampleRate, getTotalNumOutputChannels());
    backingTrack.prepare (sampleRate, samplesPerBlock);
    rigTrack.setSize (juce::jmax (1, getTotalNumOutputChannels()), samplesPerBlock);
    playerTrack.setSize (juce::jmax (1, getTotalNumOutputChannels()), samplesPerBlock);
    playerTrack.clear();
    looper.prepare (sampleRate, getTotalNumOutputChannels());
    metronome.prepare (sampleRate);
    gate.prepare (sampleRate);
    compressor.prepare (sampleRate);
    octaver.prepare (monoSpec);
    synth.prepare (monoSpec);
    pitchShifter.prepare (monoSpec);
    filter.prepare (monoSpec);
    envelopeFilter.prepare (monoSpec);
    wah.prepare (monoSpec);
    fuzz.prepare (sampleRate, samplesPerBlock);
    distortion.prepare (sampleRate, samplesPerBlock);
    amp.prepare (sampleRate, samplesPerBlock);
    equalizer.prepare (sampleRate);
    quickCuts.prepare (sampleRate);
    phaser.prepare (outputSpec);
    flanger.prepare (outputSpec);
    chorus.prepare (outputSpec);
    tremolo.prepare (outputSpec);
    delay.prepare (outputSpec);
    reverb.prepare (outputSpec);
    limiter.prepare (sampleRate);
}

void BassAmpProcessor::processPedal (PedalId pedal, juce::dsp::AudioBlock<float>& mono,
                                     const juce::dsp::ProcessContextReplacing<float>& output)
{
    switch (pedal)
    {
        case PedalId::gate:       gate.process (mono); break;
        case PedalId::compressor: compressor.process (mono); break;
        case PedalId::octaver:    octaver.process (mono); break;
        case PedalId::synth:      synth.process (mono); break;
        case PedalId::pitch:      pitchShifter.process (mono); break;
        case PedalId::filter:     filter.process (mono); break;
        case PedalId::envelope:   envelopeFilter.process (mono); break;
        case PedalId::wah:        wah.process (mono); break;
        case PedalId::fuzz:       fuzz.process (mono); break;
        case PedalId::drive:      distortion.process (mono); break;
        case PedalId::phaser:     phaser.process (output); break;
        case PedalId::flanger:    flanger.process (output); break;
        case PedalId::chorus:     chorus.process (output); break;
        case PedalId::tremolo:    tremolo.process (output); break;
        case PedalId::delay:      delay.process (output); break;
        case PedalId::reverb:     reverb.process (output); break;
    }
}

void BassAmpProcessor::followHostTempo()
{
    auto* host = wrapperType != wrapperType_Standalone ? getPlayHead() : nullptr;
    const auto position = host != nullptr ? host->getPosition() : juce::Optional<juce::AudioPlayHead::PositionInfo>();
    const auto hostBpm = position.hasValue() ? position->getBpm() : juce::Optional<double>();

    metronome.followsHost = hostBpm.hasValue();

    if (! hostBpm.hasValue())
        return;

    metronome.bpm = (float) *hostBpm;

    if (const auto ppq = position->getPpqPosition(); ppq.hasValue() && position->getIsPlaying())
        metronome.syncTo (*ppq, position->getPpqPositionOfLastBarStart().orFallback (0.0),
                          position->getTimeSignature().hasValue() ? position->getTimeSignature()->numerator : 4);
}

void BassAmpProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    midiLearn.process (midi);
    followHostTempo();

    const auto numInputs = getTotalNumInputChannels();
    const auto numOutputs = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    if (numInputs == 0 || numOutputs == 0)
    {
        buffer.clear();
        return;
    }

    if (inputChannel.load() == 1 && numInputs > 1)
        buffer.copyFrom (0, 0, buffer, 1, 0, numSamples);

    inputMeter.push (buffer.getReadPointer (0), numSamples);
    tuner.push (buffer.getReadPointer (0), numSamples);

    backingTrack.render (numSamples);
    const auto reamp = backingTrack.reamp.load();

    if (reamp)
        for (int i = 0; i < numSamples; ++i)
            buffer.addSample (0, i, backingTrack.getMonoSample (i));

    juce::dsp::AudioBlock<float> block (buffer);
    auto mono = block.getSingleChannelBlock (0);
    auto outputBlock = block.getSubsetChannelBlock (0, (size_t) numOutputs);
    const juce::dsp::ProcessContextReplacing<float> context (outputBlock);

    pedalChain.forEach (Board::pre, [&] (PedalId pedal) { processPedal (pedal, mono, context); });
    amp.process (mono);
    ampMeter.push (buffer.getReadPointer (0), numSamples);
    equalizer.process (mono);
    quickCuts.process (mono);

    for (int channel = 1; channel < numOutputs; ++channel)
        buffer.copyFrom (channel, 0, buffer, 0, 0, numSamples);

    pedalChain.forEach (Board::post, [&] (PedalId pedal) { processPedal (pedal, mono, context); });

    if (! reamp)
        for (int channel = 0; channel < numOutputs; ++channel)
            for (int i = 0; i < numSamples; ++i)
                buffer.addSample (channel, i, backingTrack.getSample (channel, i));

    looper.process (outputBlock, metronome);

    // The looper only adds its loops, so the rig with its loops is the output less the player.
    if (recorder.separateTracks.load() && numSamples <= rigTrack.getNumSamples())
    {
        for (int channel = 0; channel < numOutputs; ++channel)
        {
            rigTrack.copyFrom (channel, 0, buffer, channel, 0, numSamples);

            for (int i = 0; i < numSamples; ++i)
            {
                const auto player = reamp ? 0.0f : backingTrack.getSample (channel, i);
                playerTrack.setSample (channel, i, player);
                rigTrack.addSample (channel, i, -player);
            }
        }
    }

    limiter.process (outputBlock);

    for (int channel = 0; channel < numOutputs; ++channel)
        outputMeter.push (buffer.getReadPointer (channel), numSamples);

    spectrum.push (buffer.getReadPointer (0), numSamples);
    recorder.push (buffer, rigTrack, playerTrack, numOutputs, numSamples);
    metronome.process (outputBlock);

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
    {
        xml->setAttribute (inputChannelAttribute, inputChannel.load());
        xml->setAttribute (editorWidthAttribute, editorWidth);
        copyXmlToBinary (*xml, destData);
    }
}

void BassAmpProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
        {
            inputChannel = juce::jlimit (0, 1, xml->getIntAttribute (inputChannelAttribute));
            editorWidth = xml->getIntAttribute (editorWidthAttribute);
            xml->removeAttribute (inputChannelAttribute);
            xml->removeAttribute (editorWidthAttribute);

            presets.restoreState (juce::ValueTree::fromXml (*xml));
            presets.markUnmodified();
            undoHistory.clear();
        }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BassAmpProcessor();
}
