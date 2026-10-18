#pragma once

#include <juce_audio_formats/juce_audio_formats.h>

// Records the processed output to a 24-bit WAV file. The audio thread only copies samples
// into a FIFO; a background thread writes them to disk, so the disk never blocks the audio.
class Recorder
{
public:
    Recorder();
    ~Recorder();

    static juce::File getDefaultDirectory();

    void prepare (double sampleRate, int numChannels);
    void push (const juce::AudioBuffer<float>& buffer, int numChannels, int numSamples);

    bool start (const juce::File& file);
    void stop();

    bool isRecording() const { return activeWriter.load() != nullptr; }
    double getRecordedSeconds() const;
    juce::int64 getDroppedSamples() const { return droppedSamples.load(); }
    juce::File getLastFile() const { return lastFile; }

private:
    static constexpr double bufferSeconds = 2.0;

    juce::TimeSliceThread writerThread { "Bass Amp recorder" };
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
    juce::SpinLock writerLock;

    std::atomic<double> sampleRate { 44100.0 };
    std::atomic<int> numChannels { 2 };
    std::atomic<juce::int64> recordedSamples { 0 }, droppedSamples { 0 };
    juce::File lastFile;
};
