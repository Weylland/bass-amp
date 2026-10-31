#include "BackingTrack.h"

namespace
{
    constexpr int readAheadSamples = 32768;
}

BackingTrack::BackingTrack()
{
    formats.registerBasicFormats();
    readAheadThread.startThread();
}

BackingTrack::~BackingTrack()
{
    transport.setSource (nullptr);
    readAheadThread.stopThread (1000);
}

bool BackingTrack::load (const juce::File& newFile)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formats.createReaderFor (newFile));

    if (reader == nullptr)
        return false;

    const auto sourceRate = reader->sampleRate;
    auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader.release(), true);
    newSource->setLooping (looping);

    transport.stop();
    transport.setSource (newSource.get(), readAheadSamples, &readAheadThread, sourceRate, 2);
    readerSource = std::move (newSource);
    file = newFile;
    return true;
}

void BackingTrack::play()
{
    if (readerSource != nullptr)
        transport.start();
}

void BackingTrack::pause()
{
    transport.stop();
}

void BackingTrack::stop()
{
    transport.stop();
    transport.setPosition (0.0);
}

bool BackingTrack::isPlaying() const
{
    return transport.isPlaying();
}

double BackingTrack::getPositionSeconds() const
{
    const auto length = transport.getLengthInSeconds();
    return length > 0.0 ? std::fmod (transport.getCurrentPosition(), length) : 0.0;
}

double BackingTrack::getLengthSeconds() const
{
    return transport.getLengthInSeconds();
}

void BackingTrack::setPositionSeconds (double seconds)
{
    transport.setPosition (seconds);
}

void BackingTrack::setLooping (bool shouldLoop)
{
    looping = shouldLoop;

    if (readerSource != nullptr)
        readerSource->setLooping (shouldLoop);
}

void BackingTrack::prepare (double sampleRate, int maxBlockSize)
{
    buffer.setSize (2, maxBlockSize);
    buffer.clear();
    transport.prepareToPlay (maxBlockSize, sampleRate);
}

void BackingTrack::render (int numSamples)
{
    const auto count = juce::jmin (numSamples, buffer.getNumSamples());
    const juce::AudioSourceChannelInfo info (&buffer, 0, count);
    transport.getNextAudioBlock (info);
    buffer.applyGain (0, count, volume.load());
}

float BackingTrack::getSample (int channel, int index) const
{
    return buffer.getSample (juce::jmin (channel, 1), index);
}

float BackingTrack::getMonoSample (int index) const
{
    return 0.5f * (buffer.getSample (0, index) + buffer.getSample (1, index));
}
