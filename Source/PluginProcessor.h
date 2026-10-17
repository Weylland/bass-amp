#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "dsp/AmpStage.h"
#include "dsp/Distortion.h"

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
    double getTailLengthSeconds() const override { return 3.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateDsp();

    struct RawValues
    {
        std::atomic<float>* ampGain;
        std::atomic<float>* ampBass;
        std::atomic<float>* ampMid;
        std::atomic<float>* ampTreble;
        std::atomic<float>* ampMaster;
        std::atomic<float>* distOn;
        std::atomic<float>* distDrive;
        std::atomic<float>* distTone;
        std::atomic<float>* distBlend;
        std::atomic<float>* chorusOn;
        std::atomic<float>* chorusRate;
        std::atomic<float>* chorusDepth;
        std::atomic<float>* chorusMix;
        std::atomic<float>* reverbOn;
        std::atomic<float>* reverbSize;
        std::atomic<float>* reverbDamping;
        std::atomic<float>* reverbMix;
    };

    RawValues raw;

    Distortion distortion;
    AmpStage amp;
    juce::dsp::Chorus<float> chorus;
    juce::dsp::Reverb reverb;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassAmpProcessor)
};
