#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "dsp/AmpStage.h"
#include "dsp/ChorusEffect.h"
#include "dsp/Compressor.h"
#include "dsp/Delay.h"
#include "dsp/Distortion.h"
#include "dsp/EnvelopeFilter.h"
#include "dsp/Fuzz.h"
#include "dsp/NoiseGate.h"
#include "dsp/Octaver.h"
#include "dsp/PeakMeter.h"
#include "dsp/ReverbEffect.h"
#include "dsp/Tuner.h"

class BassAmpProcessor : public juce::AudioProcessor
{
public:
    BassAmpProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getCompressorGainReductionDb() const { return compressor.getGainReductionDb(); }

    // Set by the standalone app; read and written on the message thread only.
    juce::String audioDeviceInfo;

    juce::AudioProcessorValueTreeState parameters;
    Tuner tuner;
    PeakMeter inputMeter, outputMeter;
    std::atomic<bool> muteOutput { false };

private:
    NoiseGate gate;
    Compressor compressor;
    Octaver octaver;
    EnvelopeFilter envelopeFilter;
    Fuzz fuzz;
    Distortion distortion;
    AmpStage amp;
    ChorusEffect chorus;
    Delay delay;
    ReverbEffect reverb;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassAmpProcessor)
};
