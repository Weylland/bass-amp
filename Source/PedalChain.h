#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "ParameterIds.h"

enum class Board { pre, post };

enum class PedalId
{
    gate, compressor, octaver, synth, pitch, filter, envelope, wah, fuzz, drive,
    phaser, flanger, chorus, tremolo, delay, reverb
};

struct PedalInfo
{
    PedalId id;
    const char* key;
    const char* toggleId;
    Board board;
    bool shownByDefault;
};

// In canonical order: a pedal added to a board goes back to this position among its neighbours.
inline constexpr std::array<PedalInfo, 16> pedalInfos {{
    { PedalId::gate,       "gate",     ParamIds::gateOn,    Board::pre,  true },
    { PedalId::compressor, "comp",     ParamIds::compOn,    Board::pre,  true },
    { PedalId::octaver,    "octaver",  ParamIds::octOn,     Board::pre,  true },
    { PedalId::synth,      "synth",    ParamIds::synthOn,   Board::pre,  false },
    { PedalId::pitch,      "pitch",    ParamIds::pitchOn,   Board::pre,  false },
    { PedalId::filter,     "filter",   ParamIds::filterOn,  Board::pre,  false },
    { PedalId::envelope,   "envelope", ParamIds::envOn,     Board::pre,  true },
    { PedalId::wah,        "wah",      ParamIds::wahOn,     Board::pre,  false },
    { PedalId::fuzz,       "fuzz",     ParamIds::fuzzOn,    Board::pre,  true },
    { PedalId::drive,      "drive",    ParamIds::distOn,    Board::pre,  true },
    { PedalId::phaser,     "phaser",   ParamIds::phaserOn,  Board::post, true },
    { PedalId::flanger,    "flanger",  ParamIds::flangerOn, Board::post, false },
    { PedalId::chorus,     "chorus",   ParamIds::chorusOn,  Board::post, true },
    { PedalId::tremolo,    "tremolo",  ParamIds::tremoloOn, Board::post, false },
    { PedalId::delay,      "delay",    ParamIds::delayOn,   Board::post, true },
    { PedalId::reverb,     "reverb",   ParamIds::reverbOn,  Board::post, true },
}};

// Which pedals sit on each board, and in what order. The order is stored in the state as a
// list of pedal keys, so presets and sessions keep it; pedals missing from the list keep
// their canonical place. A pedal is shown when it is in the list or switched on, and the
// audio thread reads each board's full order as a packed permutation.
class PedalChain : public juce::ChangeBroadcaster, private juce::ValueTree::Listener
{
public:
    static inline const juce::Identifier property { "pedalChain" };

    explicit PedalChain (juce::AudioProcessorValueTreeState&);
    ~PedalChain() override;

    // Message thread.
    std::vector<PedalId> getShown (Board) const;
    std::vector<PedalId> getHidden (Board) const;
    void show (PedalId);                         // inserted at its canonical place and switched on
    void hide (PedalId);                         // removed and switched off
    void move (PedalId, int newShownIndex);

    // Audio thread: every pedal of the board in processing order.
    template <typename Function>
    void forEach (Board board, Function&& function) const
    {
        const auto packed = order[(size_t) board].load (std::memory_order_acquire);
        const auto count = board == Board::pre ? numPre : numPost;
        const auto offset = board == Board::pre ? 0 : numPre;

        for (int i = 0; i < count; ++i)
            function ((PedalId) (offset + (int) ((packed >> (4 * i)) & 0xf)));
    }

private:
    static constexpr int numPre = 10, numPost = 6;

    std::vector<PedalId> getOrder (Board) const;
    bool isOn (PedalId) const;
    void store (const std::vector<PedalId>& shownPre, const std::vector<PedalId>& shownPost);
    void publish();

    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeRedirected (juce::ValueTree&) override;

    juce::AudioProcessorValueTreeState& state;
    std::array<std::atomic<juce::uint64>, 2> order {};
};
