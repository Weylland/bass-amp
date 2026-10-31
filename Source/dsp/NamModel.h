#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace nam { class DSP; }

// A Neural Amp Modeler capture (.nam file). It is loaded and warmed up on the message thread,
// then swapped in under a lock the audio thread only tries, so the audio never waits. The
// output is levelled from the loudness stored in the capture.
class NamModel
{
public:
    NamModel();
    ~NamModel();

    // Message thread. Keeps the previous capture when the file can't be read.
    bool load (const juce::File&);
    void unload();
    double getExpectedSampleRate() const { return expectedSampleRate.load(); }   // 0: none loaded, < 0: not in the file

    // Not real-time: from prepareToPlay.
    void prepare (double sampleRate, int maxBlockSize);

    // Audio thread, mono, in place. Returns false, leaving the samples as they are, when no
    // capture is ready.
    bool process (float* samples, int numSamples);

private:
    std::unique_ptr<nam::DSP> model;
    juce::SpinLock lock;
    std::vector<float> output;
    float outputGain = 1.0f;
    double sampleRate = 48000.0;
    int maxBlockSize = 512;
    std::atomic<double> expectedSampleRate { 0.0 };

    JUCE_DECLARE_NON_COPYABLE (NamModel)
};
