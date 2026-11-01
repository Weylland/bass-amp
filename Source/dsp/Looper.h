#pragma once

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

#include "Metronome.h"

// Four-track looper on the rig output, made to be played live from a MIDI controller.
// Each track's button records, plays and overdubs in turn. Tracks start and end on a grid:
// the metronome's bars when SYNC is on and the metronome runs, otherwise the length of the
// first loop. Every track stays in phase with the others, so a stopped track comes back in
// time. The last recording or overdub can be undone and redone. A track can play reversed
// or at half speed (an octave down). Up to 30 seconds per track.
// The first loop can wait for the bass: with AUTO, recording starts on the first note played,
// keeping a few milliseconds before it for the attack. With SYNC, COUNT-IN gives a full bar of
// clicks before it.
// While it is the only loop, a track can be trimmed at both ends, for a late click on the pad.
// The trim only moves where playback starts and ends: the whole take is kept.
// Buttons and MIDI only post requests; the audio thread acts on them at the next block.
class Looper
{
public:
    static constexpr int numTracks = 4;

    enum class State { empty, armed, recording, finishing, playing, overdubbing, stopped };

    // MIDI-learnable actions: one per track, then the global ones. { id, name }
    static inline const std::array<std::pair<const char*, const char*>, numTracks + 4> midiActions {{
        { "looper_track1", "Looper Track 1" }, { "looper_track2", "Looper Track 2" },
        { "looper_track3", "Looper Track 3" }, { "looper_track4", "Looper Track 4" },
        { "looper_undo", "Looper Undo" }, { "looper_redo", "Looper Redo" },
        { "looper_stop_all", "Looper Stop / Play All" }, { "looper_clear_all", "Looper Clear All" },
    }};

    void prepare (double sampleRate, int numChannels);

    // Any thread.
    void pressTrack (int track)       { tracks[(size_t) track].presses.fetch_add (1); }
    void stopTrack (int track)        { tracks[(size_t) track].stopRequest.store (true); }
    void clearTrack (int track)       { tracks[(size_t) track].clearRequest.store (true); }
    void undo()                       { undoRequest.store (true); }
    void redo()                       { redoRequest.store (true); }
    void stopOrPlayAll()              { stopAllRequest.store (true); }
    void clearAll()                   { clearAllRequest.store (true); }
    void trimTrack (int track, int startSample, int endSample);
    void trigger (int midiAction);

    std::atomic<bool> sync { true }, autoStart { true }, countIn { true };

    struct TrackControls
    {
        std::atomic<float> level { 1.0f };
        std::atomic<bool> reverse { false }, halfSpeed { false };
    };

    TrackControls& controls (int track) { return tracks[(size_t) track].controls; }

    // Message thread.
    State getState (int track) const         { return tracks[(size_t) track].shownState.load(); }
    float getProgress (int track) const      { return tracks[(size_t) track].progress.load(); }
    double getLengthSeconds (int track) const;
    bool canUndo() const                     { return undoable.load(); }
    bool canRedo() const                     { return redoable.load(); }
    bool isSynced() const                    { return synced.load(); }
    bool hasLoops() const;
    bool canTrim (int track) const;
    double getSampleRate() const             { return rate; }

    struct Trim { int start = 0, end = 0, recorded = 0; };
    Trim getTrim (int track) const;

    // The peak of each of `points` slices of the whole take, for drawing it. Reads the take
    // while the audio thread may overdub it, so a slice can be a block late.
    std::vector<float> getOverview (int track, int points) const;

    // Writes every playing track (or every recorded one when none plays) to a 24-bit WAV,
    // over one turn of the longest loop.
    bool exportMix (const juce::File&) const;

    // Audio thread: records the block and adds the loops to it. The clock is read before it
    // processes the same block.
    void process (juce::dsp::AudioBlock<float>&, const Metronome& clock);

private:
    static constexpr double maxSeconds = 30.0;
    static constexpr double preRollSeconds = 0.02;
    static constexpr double countInGraceSeconds = 0.1;
    static constexpr float startLevel = 0.02f;
    static constexpr double minTrimSeconds = 0.1;
    static constexpr int chunkBits = 9, chunkSize = 1 << chunkBits;

    struct Track
    {
        // Two copies of the loop, chosen per chunk: the current take, and the take before the
        // last overdub for undo. An overdub copies a chunk the first time it writes to it.
        std::array<juce::AudioBuffer<float>, 2> takes;
        std::vector<juce::uint8> current;
        std::vector<int> touched;
        int generation = 0;

        State state = State::empty;
        int recorded = 0, lastWritten = -1;
        juce::int64 armedAt = 0;
        bool stopWhenFinished = false;
        std::atomic<int> length { 0 }, start { 0 }, recordedLength { 0 };
        std::atomic<juce::int64> origin { 0 };

        TrackControls controls;
        std::atomic<int> presses { 0 };
        std::atomic<bool> stopRequest { false }, clearRequest { false }, trimRequest { false };
        std::atomic<int> requestedStart { 0 }, requestedEnd { 0 };
        std::atomic<State> shownState { State::empty };
        std::atomic<float> progress { 0.0f };
        std::atomic<int> shownSamples { 0 };

        float sample (int channel, int index) const { return takes[current[(size_t) (index >> chunkBits)]].getSample (channel, index); }
        float read (int channel, double position, int loopLength) const;
        double playPosition (juce::int64 now, int loopLength) const;
        void prepareChunkForWrite (int index);
        void flipTouchedChunks();
        bool hasLoop() const { return state == State::playing || state == State::overdubbing || state == State::stopped; }
    };

    enum class Undo { none, recording, overdub };

    void applyRequests (bool hasGrid);
    void press (int track, bool hasGrid);
    void stop (int track, bool hasGrid);
    void clear (int track);
    void startRecording (Track&, int fromPreRoll = 0);
    void finish (int track);
    void trim (int track);
    bool isOnlyTrack (int track) const;
    void applyUndo (bool redoing);
    void updateMaster();

    std::array<Track, numTracks> tracks;
    int numChannels = 2, maxSamples = 0;
    double rate = 44100.0;
    juce::int64 transport = 0;
    int masterLength = 0;
    juce::int64 masterOrigin = 0;
    juce::AudioBuffer<float> preRoll;
    int preRollPosition = 0;

    int undoTrack = -1;
    Undo undoKind = Undo::none;
    bool undone = false;

    std::atomic<bool> undoRequest { false }, redoRequest { false }, stopAllRequest { false }, clearAllRequest { false };
    std::atomic<bool> undoable { false }, redoable { false }, synced { false };
};
