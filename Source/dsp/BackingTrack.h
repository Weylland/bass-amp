#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/juce_audio_devices.h>

// Plays an audio file along with the bass: mixed into the output as a backing track, or fed
// into the rig as the input (re-amp) to run a recorded DI through the pedals and the amp.
// The file is decoded ahead on a background thread; the audio thread only pulls samples.
class BackingTrack
{
public:
    BackingTrack();
    ~BackingTrack();

    // Message thread.
    bool load (const juce::File&);
    juce::File getFile() const { return file; }
    void play();
    void pause();
    void stop();
    bool isPlaying() const;
    double getPositionSeconds() const;
    double getLengthSeconds() const;
    void setPositionSeconds (double);
    void setLooping (bool);

    std::atomic<float> volume { 0.7f };
    std::atomic<bool> reamp { false };

    // Audio thread.
    void prepare (double sampleRate, int maxBlockSize);
    void render (int numSamples);                    // pulls the next block, silent when stopped
    float getSample (int channel, int index) const;   // from the last rendered block
    float getMonoSample (int index) const;

private:
    juce::AudioFormatManager formats;
    juce::TimeSliceThread readAheadThread { "Backing track reader" };
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transport;
    juce::AudioBuffer<float> buffer;
    juce::File file;
    bool looping = true;
};
