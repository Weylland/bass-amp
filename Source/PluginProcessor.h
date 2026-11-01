#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "MidiLearn.h"
#include "dsp/AmpStage.h"
#include "dsp/BackingTrack.h"
#include "dsp/ChorusEffect.h"
#include "dsp/Compressor.h"
#include "dsp/Delay.h"
#include "dsp/Distortion.h"
#include "dsp/EnvelopeFilter.h"
#include "dsp/Equalizer.h"
#include "dsp/QuickCuts.h"
#include "dsp/FilterEffect.h"
#include "dsp/FlangerEffect.h"
#include "dsp/Fuzz.h"
#include "dsp/Looper.h"
#include "dsp/Metronome.h"
#include "dsp/NoiseGate.h"
#include "dsp/Octaver.h"
#include "dsp/OutputLimiter.h"
#include "dsp/PeakMeter.h"
#include "dsp/PhaserEffect.h"
#include "dsp/PitchShifter.h"
#include "dsp/Recorder.h"
#include "dsp/ReverbEffect.h"
#include "dsp/SpectrumTap.h"
#include "dsp/SynthEffect.h"
#include "dsp/TremoloEffect.h"
#include "dsp/Tuner.h"
#include "dsp/WahEffect.h"
#include "PedalChain.h"
#include "UndoHistory.h"
#include "presets/PresetManager.h"

class BassAmpProcessor : public juce::AudioProcessor, private juce::ValueTree::Listener
{
public:
    BassAmpProcessor();
    ~BassAmpProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 10.0; }

    int getNumPrograms() override { return presets.getNumFactoryPresets(); }
    int getCurrentProgram() override { return juce::jmax (0, presets.getCurrentFactoryIndex()); }
    void setCurrentProgram (int index) override { presets.loadFactoryPreset (index); }
    const juce::String getProgramName (int index) override { return presets.getFactoryPresetName (index); }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getCompressorGainReductionDb() const { return compressor.getGainReductionDb(); }

    // The impulse response file played by the "IR File" cabinet, saved with the state and presets.
    void setCabinetImpulseResponse (const juce::File&);
    juce::File getCabinetImpulseResponse() const;

    // The .nam file played by the "NAM Capture" amp model, saved with the state and presets.
    void setCapture (const juce::File&);
    juce::File getCapture() const;
    double getCaptureSampleRate() const { return amp.getCaptureSampleRate(); }

    // Set by the standalone app; read and written on the message thread only.
    juce::String audioDeviceInfo;
    int deviceInputs = 0;

    // The interface input the bass is plugged into: 0 for input 1, 1 for input 2.
    std::atomic<int> inputChannel { 0 };
    int getNumSelectableInputs() const { return juce::jmin (2, deviceInputs > 0 ? deviceInputs : getTotalNumInputChannels()); }

    // The editor's last width, so it reopens at the same size. Message thread.
    int editorWidth = 0;

    juce::AudioProcessorValueTreeState parameters;
    PresetManager presets { parameters };
    PedalChain pedalChain { parameters };
    MidiLearn midiLearn { parameters, MidiLearn::getDefaultFile(), { Looper::midiActions.begin(), Looper::midiActions.end() } };
    UndoHistory undoHistory { parameters, [this] { return midiLearn.getMappedIds(); } };
    Tuner tuner;
    PeakMeter inputMeter, outputMeter, ampMeter;
    SpectrumTap spectrum;
    Recorder recorder;
    BackingTrack backingTrack;
    Looper looper;
    Metronome metronome;
    std::atomic<bool> muteOutput { false };

private:
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeRedirected (juce::ValueTree&) override;
    void reloadImpulseResponse();
    void processPedal (PedalId, juce::dsp::AudioBlock<float>& mono, const juce::dsp::ProcessContextReplacing<float>& output);
    void followHostTempo();

    NoiseGate gate;
    Compressor compressor;
    Octaver octaver;
    SynthEffect synth;
    PitchShifter pitchShifter;
    FilterEffect filter;
    EnvelopeFilter envelopeFilter;
    WahEffect wah;
    Fuzz fuzz;
    Distortion distortion;
    AmpStage amp;
    Equalizer equalizer;
    QuickCuts quickCuts;
    PhaserEffect phaser;
    FlangerEffect flanger;
    ChorusEffect chorus;
    TremoloEffect tremolo;
    Delay delay;
    ReverbEffect reverb;
    OutputLimiter limiter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassAmpProcessor)
};
