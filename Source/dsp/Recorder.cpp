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

void Recorder::push (const juce::AudioBuffer<float>& mix, const juce::AudioBuffer<float>& rig, const juce::AudioBuffer<float>& player,
                     int channelsToWrite, int numSamples)
{
    const juce::SpinLock::ScopedTryLockType lock (writerLock);

    if (! lock.isLocked() || ! recording.load())
        return;

    const auto separate = writers.size() == 2;
    auto written = channelsToWrite >= numChannels.load();

    if (separate)
        written = written && numSamples <= juce::jmin (rig.getNumSamples(), player.getNumSamples())
                  && writers[0]->write (rig.getArrayOfReadPointers(), numSamples)
                  && writers[1]->write (player.getArrayOfReadPointers(), numSamples);
    else
        written = written && writers[0]->write (mix.getArrayOfReadPointers(), numSamples);

    if (written)
        recordedSamples += numSamples;
    else
        droppedSamples += numSamples;
}

std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> Recorder::createWriter (const juce::File& file, int bitsPerSample)
{
    file.deleteFile();
    auto stream = file.createOutputStream();

    if (stream == nullptr)
        return {};

    const auto rate = sampleRate.load();
    auto* writer = juce::WavAudioFormat().createWriterFor (stream.get(), rate, (unsigned int) numChannels.load(), bitsPerSample, {}, 0);

    if (writer == nullptr)
        return {};

    stream.release();
    return std::make_unique<juce::AudioFormatWriter::ThreadedWriter> (writer, writerThread, (int) (rate * bufferSeconds));
}

bool Recorder::start (const juce::File& file)
{
    stop();

    if (! file.getParentDirectory().createDirectory())
        return false;

    juce::Array<juce::File> files;
    auto bitsPerSample = 24;

    if (separateTracks.load())
    {
        for (const auto* suffix : { " - Bass", " - Player" })
            files.add (file.getSiblingFile (file.getFileNameWithoutExtension() + suffix + file.getFileExtension()));

        bitsPerSample = 32;
    }
    else
    {
        files.add (file);
    }

    std::vector<std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter>> created;

    for (const auto& target : files)
    {
        created.push_back (createWriter (target, bitsPerSample));

        if (created.back() == nullptr)
            return false;
    }

    recordedSamples = 0;
    droppedSamples = 0;
    lastFiles = files;

    const juce::SpinLock::ScopedLockType lock (writerLock);
    writers = std::move (created);
    recording = true;
    return true;
}

void Recorder::stop()
{
    std::vector<std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter>> finished;

    {
        const juce::SpinLock::ScopedLockType lock (writerLock);
        recording = false;
        finished = std::move (writers);
        writers.clear();
    }
}

double Recorder::getRecordedSeconds() const
{
    return (double) recordedSamples.load() / sampleRate.load();
}
