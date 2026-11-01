#pragma once

#include <juce_audio_formats/juce_audio_formats.h>

// Records the processed output to a 24-bit WAV file. With separateTracks, it records instead the
// rig (bass, pedals and loops) and the player to two 32-bit float files side by side, "... - Bass"
// and "... - Player", to mix them later. The audio thread only copies samples into FIFOs; a
// background thread writes them to disk, so the disk never blocks the audio.
class Recorder
{
public:
    Recorder();
    ~Recorder();

    static juce::File getDefaultDirectory();

    std::atomic<bool> separateTracks { false };

    void prepare (double sampleRate, int numChannels);

    // Audio thread: the mix, and the rig and player apart, which are only read when recording
    // separate tracks.
    void push (const juce::AudioBuffer<float>& mix, const juce::AudioBuffer<float>& rig, const juce::AudioBuffer<float>& player,
               int numChannels, int numSamples);

    // The file of the mix; separate tracks are written next to it, with " - Bass" and " - Player".
    bool start (const juce::File& file);
    void stop();

    bool isRecording() const { return recording.load(); }
    double getRecordedSeconds() const;
    juce::int64 getDroppedSamples() const { return droppedSamples.load(); }
    juce::File getLastFile() const { return lastFiles.isEmpty() ? juce::File() : lastFiles.getFirst(); }
    juce::Array<juce::File> getLastFiles() const { return lastFiles; }

private:
    static constexpr double bufferSeconds = 2.0;

    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> createWriter (const juce::File&, int bitsPerSample);

    juce::TimeSliceThread writerThread { "Bass Amp recorder" };
    std::vector<std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter>> writers;
    std::atomic<bool> recording { false };
    juce::SpinLock writerLock;

    std::atomic<double> sampleRate { 44100.0 };
    std::atomic<int> numChannels { 2 };
    std::atomic<juce::int64> recordedSamples { 0 }, droppedSamples { 0 };
    juce::Array<juce::File> lastFiles;
};
