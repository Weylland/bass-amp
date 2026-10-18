#include "Recorder.h"

Recorder::Recorder()
{
    writerThread.startThread();
}

Recorder::~Recorder()
{
    stop();
    writerThread.stopThread (2000);
}

juce::File Recorder::getDefaultDirectory()
{
    return juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("Bass Amp").getChildFile ("Recordings");
}

void Recorder::prepare (double newSampleRate, int newNumChannels)
{
    sampleRate.store (newSampleRate);
    numChannels.store (juce::jmax (1, newNumChannels));
}

void Recorder::push (const juce::AudioBuffer<float>& buffer, int channelsToWrite, int numSamples)
{
    const juce::SpinLock::ScopedTryLockType lock (writerLock);

    if (! lock.isLocked())
        return;

    if (auto* writer = activeWriter.load())
    {
        if (channelsToWrite >= numChannels.load() && writer->write (buffer.getArrayOfReadPointers(), numSamples))
            recordedSamples += numSamples;
        else
            droppedSamples += numSamples;
    }
}

bool Recorder::start (const juce::File& file)
{
    stop();

    if (! file.getParentDirectory().createDirectory())
        return false;

    file.deleteFile();
    auto stream = file.createOutputStream();

    if (stream == nullptr)
        return false;

    const auto rate = sampleRate.load();
    auto* writer = juce::WavAudioFormat().createWriterFor (stream.get(), rate, (unsigned int) numChannels.load(), 24, {}, 0);

    if (writer == nullptr)
        return false;

    stream.release();
    threadedWriter = std::make_unique<juce::AudioFormatWriter::ThreadedWriter> (writer, writerThread, (int) (rate * bufferSeconds));
    recordedSamples = 0;
    droppedSamples = 0;
    lastFile = file;

    const juce::SpinLock::ScopedLockType lock (writerLock);
    activeWriter = threadedWriter.get();
    return true;
}

void Recorder::stop()
{
    {
        const juce::SpinLock::ScopedLockType lock (writerLock);
        activeWriter = nullptr;
    }

    threadedWriter.reset();
}

double Recorder::getRecordedSeconds() const
{
    return (double) recordedSamples.load() / sampleRate.load();
}
