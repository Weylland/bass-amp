#include "Looper.h"

namespace
{
    juce::int64 wrap (juce::int64 value, juce::int64 length)
    {
        return ((value % length) + length) % length;
    }
}

void Looper::prepare (double sampleRate, int newNumChannels)
{
    rate = sampleRate;
    numChannels = juce::jlimit (1, 2, newNumChannels);
    maxSamples = (int) (sampleRate * maxSeconds);
    const auto numChunks = (size_t) ((maxSamples + chunkSize - 1) >> chunkBits);

    for (auto& track : tracks)
    {
        for (auto& take : track.takes)
        {
            take.setSize (numChannels, maxSamples);
            take.clear();
        }

        track.current.assign (numChunks, 0);
        track.touched.assign (numChunks, -1);
        track.generation = 0;
        track.state = State::empty;
        track.recorded = 0;
        track.length.store (0);
        track.start.store (0);
        track.recordedLength.store (0);
        track.shownState.store (State::empty);
        track.progress.store (0.0f);
        track.shownSamples.store (0);
    }

    preRoll.setSize (numChannels, juce::jmax (1, (int) (sampleRate * preRollSeconds)));
    preRoll.clear();
    preRollPosition = 0;

    transport = 0;
    masterLength = 0;
    undoTrack = -1;
    undoKind = Undo::none;
    undone = false;
}

void Looper::trigger (int midiAction)
{
    if (juce::isPositiveAndBelow (midiAction, numTracks))
        pressTrack (midiAction);
    else if (midiAction == numTracks)
        undo();
    else if (midiAction == numTracks + 1)
        redo();
    else if (midiAction == numTracks + 2)
        stopOrPlayAll();
    else if (midiAction == numTracks + 3)
        clearAll();
}

void Looper::trimTrack (int track, int startSample, int endSample)
{
    auto& chosen = tracks[(size_t) track];
    chosen.requestedStart.store (startSample);
    chosen.requestedEnd.store (endSample);
    chosen.trimRequest.store (true);
}

bool Looper::canTrim (int track) const
{
    const auto state = tracks[(size_t) track].shownState.load();

    if (state != State::playing && state != State::overdubbing && state != State::stopped)
        return false;

    for (int other = 0; other < numTracks; ++other)
        if (other != track && tracks[(size_t) other].shownState.load() != State::empty)
            return false;

    return tracks[(size_t) track].recordedLength.load() > 2 * (int) (rate * minTrimSeconds);
}

Looper::Trim Looper::getTrim (int track) const
{
    const auto& chosen = tracks[(size_t) track];
    const auto start = chosen.start.load();
    return { start, start + chosen.length.load(), chosen.recordedLength.load() };
}

std::vector<float> Looper::getOverview (int track, int points) const
{
    const auto& chosen = tracks[(size_t) track];
    const auto recorded = chosen.recordedLength.load();
    std::vector<float> peaks ((size_t) juce::jmax (0, points), 0.0f);

    for (int point = 0; point < points && recorded > 0; ++point)
    {
        const auto from = (int) ((juce::int64) recorded * point / points);
        const auto to = juce::jmax (from + 1, (int) ((juce::int64) recorded * (point + 1) / points));

        for (int i = from; i < to; ++i)
            for (int channel = 0; channel < numChannels; ++channel)
                peaks[(size_t) point] = juce::jmax (peaks[(size_t) point], std::abs (chosen.sample (channel, i)));
    }

    return peaks;
}

double Looper::getLengthSeconds (int track) const
{
    return (double) tracks[(size_t) track].shownSamples.load() / rate;
}

bool Looper::hasLoops() const
{
    return std::any_of (tracks.begin(), tracks.end(), [] (const Track& track)
    {
        const auto state = track.shownState.load();
        return state == State::playing || state == State::overdubbing || state == State::stopped;
    });
}

float Looper::Track::read (int channel, double position, int loopLength) const
{
    const auto first = (int) position;
    const auto second = first + 1 == loopLength ? 0 : first + 1;
    const auto fraction = (float) (position - first);
    const auto offset = start.load();
    const auto a = sample (channel, offset + first);
    const auto b = sample (channel, offset + second);
    return a + fraction * (b - a);
}

double Looper::Track::playPosition (juce::int64 now, int loopLength) const
{
    const auto elapsed = now - origin.load();
    const auto phase = controls.halfSpeed.load() ? (double) wrap (elapsed, 2 * (juce::int64) loopLength) * 0.5
                                                 : (double) wrap (elapsed, loopLength);

    if (! controls.reverse.load())
        return phase;

    const auto reversed = (double) loopLength - 1.0 - phase;
    return reversed < 0.0 ? reversed + loopLength : reversed;
}

void Looper::Track::prepareChunkForWrite (int index)
{
    const auto chunk = (size_t) (index >> chunkBits);

    if (touched[chunk] == generation)
        return;

    const auto from = current[chunk];
    const auto to = (juce::uint8) (1 - from);
    const auto first = (int) chunk << chunkBits;
    const auto count = juce::jmin (chunkSize, takes[0].getNumSamples() - first);

    for (int channel = 0; channel < takes[0].getNumChannels(); ++channel)
        takes[to].copyFrom (channel, first, takes[from], channel, first, count);

    current[chunk] = to;
    touched[chunk] = generation;
}

void Looper::Track::flipTouchedChunks()
{
    const auto numChunks = (recordedLength.load() + chunkSize - 1) >> chunkBits;

    for (size_t chunk = 0; chunk < (size_t) numChunks; ++chunk)
        if (touched[chunk] == generation)
            current[chunk] ^= 1;
}

// The pre-roll samples are the ones just before now, so the loop starts that much earlier.
void Looper::startRecording (Track& track, int fromPreRoll)
{
    track.origin.store (transport - fromPreRoll);
    track.recorded = 0;
    track.state = State::recording;

    const auto size = preRoll.getNumSamples();

    for (int k = 0; k < fromPreRoll; ++k, ++track.recorded)
    {
        auto& take = track.takes[track.current[(size_t) (track.recorded >> chunkBits)]];
        const auto source = (preRollPosition - fromPreRoll + k + size) % size;

        for (int channel = 0; channel < numChannels; ++channel)
            take.setSample (channel, track.recorded, preRoll.getSample (channel, source));
    }
}

void Looper::finish (int index)
{
    auto& track = tracks[(size_t) index];
    track.length.store (juce::jmax (1, track.recorded));
    track.start.store (0);
    track.recordedLength.store (track.length.load());
    track.state = track.stopWhenFinished ? State::stopped : State::playing;
    undoTrack = index;
    undoKind = Undo::recording;
    undone = false;
    updateMaster();
}

bool Looper::isOnlyTrack (int index) const
{
    for (int other = 0; other < numTracks; ++other)
        if (other != index && tracks[(size_t) other].state != State::empty)
            return false;

    return tracks[(size_t) index].hasLoop();
}

// The loop keeps its origin, so a trimmed loop goes on from where its new length puts it.
void Looper::trim (int index)
{
    auto& track = tracks[(size_t) index];
    const auto recorded = track.recordedLength.load();
    const auto minimum = (int) (rate * minTrimSeconds);

    if (! isOnlyTrack (index) || recorded <= 2 * minimum)
        return;

    const auto start = juce::jlimit (0, recorded - minimum, track.requestedStart.load());
    const auto end = juce::jlimit (start + minimum, recorded, track.requestedEnd.load());
    track.start.store (start);
    track.length.store (end - start);
    masterLength = end - start;
}

void Looper::updateMaster()
{
    const auto withLoop = std::find_if (tracks.begin(), tracks.end(), [] (const Track& track) { return track.hasLoop(); });

    if (withLoop == tracks.end())
        masterLength = 0;
    else if (masterLength == 0)
    {
        masterLength = withLoop->length.load();
        masterOrigin = withLoop->origin.load();
    }
}

void Looper::press (int index, bool hasGrid)
{
    auto& track = tracks[(size_t) index];

    switch (track.state)
    {
        case State::empty:
            if (undoTrack == index)
                undoKind = Undo::none;

            ++track.generation;
            track.stopWhenFinished = false;
            track.recorded = 0;
            track.armedAt = transport;

            if (hasGrid || autoStart.load())
                track.state = State::armed;
            else
                startRecording (track);
            break;

        case State::armed:       track.state = State::empty; break;
        case State::recording:   if (hasGrid) track.state = State::finishing; else finish (index); break;
        case State::finishing:   break;
        case State::overdubbing: track.state = State::playing; break;
        case State::stopped:     track.state = State::playing; break;

        case State::playing:
            ++track.generation;
            track.lastWritten = -1;
            track.state = State::overdubbing;
            undoTrack = index;
            undoKind = Undo::overdub;
            undone = false;
            break;
    }
}

void Looper::stop (int index, bool hasGrid)
{
    auto& track = tracks[(size_t) index];

    if (track.state == State::armed)
        track.state = State::empty;
    else if (track.state == State::recording || track.state == State::finishing)
    {
        track.stopWhenFinished = true;

        if (hasGrid)
            track.state = State::finishing;
        else
            finish (index);
    }
    else if (track.state == State::playing || track.state == State::overdubbing)
        track.state = State::stopped;
}

void Looper::clear (int index)
{
    tracks[(size_t) index].state = State::empty;
    tracks[(size_t) index].length.store (0);

    if (undoTrack == index)
        undoKind = Undo::none;

    updateMaster();
}

void Looper::applyUndo (bool redoing)
{
    if (undoKind == Undo::none || undone != redoing)
        return;

    auto& track = tracks[(size_t) undoTrack];

    if (track.state == State::overdubbing)
        track.state = State::playing;

    if (undoKind == Undo::recording)
        track.state = redoing ? State::playing : State::empty;
    else
        track.flipTouchedChunks();

    undone = ! redoing;
    updateMaster();
}

void Looper::applyRequests (bool hasGrid)
{
    if (clearAllRequest.exchange (false))
        for (int index = 0; index < numTracks; ++index)
            clear (index);

    for (int index = 0; index < numTracks; ++index)
    {
        auto& track = tracks[(size_t) index];

        if (track.clearRequest.exchange (false))
            clear (index);

        if (track.stopRequest.exchange (false))
            stop (index, hasGrid);

        if (track.trimRequest.exchange (false))
            trim (index);

        for (auto presses = track.presses.exchange (0); presses > 0; --presses)
            press (index, hasGrid);
    }

    if (stopAllRequest.exchange (false))
    {
        const auto anyPlaying = std::any_of (tracks.begin(), tracks.end(), [] (const Track& track)
                                             { return track.state == State::playing || track.state == State::overdubbing; });

        for (auto& track : tracks)
        {
            if (anyPlaying && (track.state == State::playing || track.state == State::overdubbing))
                track.state = State::stopped;
            else if (! anyPlaying && track.state == State::stopped)
                track.state = State::playing;
        }
    }

    if (undoRequest.exchange (false))
        applyUndo (false);

    if (redoRequest.exchange (false))
        applyUndo (true);
}

void Looper::process (juce::dsp::AudioBlock<float>& block, const Metronome& clock)
{
    const auto barSync = sync.load() && clock.enabled.load();
    synced.store (barSync);
    applyRequests (barSync || masterLength > 0);

    const auto channels = juce::jmin ((int) block.getNumChannels(), numChannels);
    const auto barLength = barSync ? clock.getBarLength() : 0.0;
    const auto countInLength = countIn.load() ? barLength - countInGraceSeconds * rate : 0.0;
    auto nextBar = barSync ? clock.getSamplesToNextBar() : 0.0;

    for (int i = 0; i < (int) block.getNumSamples(); ++i)
    {
        auto boundary = true;

        if (barSync)
        {
            boundary = i >= (int) std::ceil (nextBar);

            if (boundary)
                nextBar += barLength;
        }
        else if (masterLength > 0)
        {
            boundary = wrap (transport - masterOrigin, masterLength) == 0;
        }

        std::array<float, 2> input {}, added {};
        auto peak = 0.0f;

        for (int channel = 0; channel < channels; ++channel)
        {
            input[(size_t) channel] = block.getSample (channel, i);
            peak = juce::jmax (peak, std::abs (input[(size_t) channel]));
        }

        for (int index = 0; index < numTracks; ++index)
        {
            auto& track = tracks[(size_t) index];

            if (boundary && track.state == State::armed)
            {
                // The first loop waits for a full bar of count-in, or for the first note.
                if (barSync)
                {
                    if (masterLength > 0 || (double) (transport - track.armedAt) >= countInLength)
                        startRecording (track);
                }
                else if (masterLength > 0)
                {
                    startRecording (track);
                }
                else if (peak >= startLevel)
                {
                    startRecording (track, (int) juce::jmin ((juce::int64) preRoll.getNumSamples(), transport - track.armedAt));
                }
            }
            else if (boundary && track.state == State::finishing)
                finish (index);

            if (track.state == State::recording || track.state == State::finishing)
            {
                auto& take = track.takes[track.current[(size_t) (track.recorded >> chunkBits)]];

                for (int channel = 0; channel < channels; ++channel)
                    take.setSample (channel, track.recorded, input[(size_t) channel]);

                if (++track.recorded == maxSamples)
                    finish (index);
            }
            else if (track.state == State::playing || track.state == State::overdubbing)
            {
                const auto length = track.length.load();
                const auto position = track.playPosition (transport, length);
                const auto level = track.controls.level.load();

                for (int channel = 0; channel < channels; ++channel)
                    added[(size_t) channel] += track.read (channel, position, length) * level;

                if (const auto written = (int) position; track.state == State::overdubbing && written != track.lastWritten)
                {
                    const auto inTake = track.start.load() + written;
                    track.prepareChunkForWrite (inTake);
                    auto& take = track.takes[track.current[(size_t) (inTake >> chunkBits)]];

                    for (int channel = 0; channel < channels; ++channel)
                        take.addSample (channel, inTake, input[(size_t) channel]);

                    track.lastWritten = written;
                }
            }
        }

        for (int channel = 0; channel < channels; ++channel)
        {
            block.setSample (channel, i, input[(size_t) channel] + added[(size_t) channel]);
            preRoll.setSample (channel, preRollPosition, input[(size_t) channel]);
        }

        preRollPosition = (preRollPosition + 1) % preRoll.getNumSamples();
        ++transport;
    }

    for (auto& track : tracks)
    {
        const auto recording = track.state == State::recording || track.state == State::finishing;
        const auto length = track.length.load();
        track.shownState.store (track.state);
        track.shownSamples.store (recording ? track.recorded : (track.hasLoop() ? length : 0));
        track.progress.store (track.state == State::playing || track.state == State::overdubbing
                                  ? (float) (track.playPosition (transport, length) / length) : 0.0f);
    }

    undoable.store (undoKind != Undo::none && ! undone);
    redoable.store (undoKind != Undo::none && undone);
}

bool Looper::exportMix (const juce::File& file) const
{
    auto playing = [] (const Track& track)
    {
        const auto state = track.shownState.load();
        return state == State::playing || state == State::overdubbing;
    };

    const auto anyPlaying = std::any_of (tracks.begin(), tracks.end(), playing);
    std::vector<const Track*> chosen;

    for (const auto& track : tracks)
        if (anyPlaying ? playing (track) : track.shownState.load() == State::stopped)
            chosen.push_back (&track);

    if (chosen.empty())
        return false;

    auto period = [] (const Track* track) { return track->length.load() * (track->controls.halfSpeed.load() ? 2 : 1); };
    const auto* longest = *std::max_element (chosen.begin(), chosen.end(),
                                             [&period] (const Track* a, const Track* b) { return period (a) < period (b); });
    const auto start = longest->origin.load();

    juce::AudioBuffer<float> mix (numChannels, period (longest));
    mix.clear();

    for (const auto* track : chosen)
    {
        const auto length = track->length.load();
        const auto level = track->controls.level.load();

        for (int i = 0; i < mix.getNumSamples(); ++i)
        {
            const auto position = track->playPosition (start + i, length);

            for (int channel = 0; channel < numChannels; ++channel)
                mix.addSample (channel, i, track->read (channel, position, length) * level);
        }
    }

    file.deleteFile();
    auto stream = file.createOutputStream();

    if (stream == nullptr)
        return false;

    std::unique_ptr<juce::AudioFormatWriter> writer (
        juce::WavAudioFormat().createWriterFor (stream.get(), rate, (unsigned int) numChannels, 24, {}, 0));

    if (writer == nullptr)
        return false;

    stream.release();
    return writer->writeFromAudioSampleBuffer (mix, 0, mix.getNumSamples());
}
