#include "PedalChain.h"

namespace
{
    const PedalInfo& infoFor (PedalId id)
    {
        return pedalInfos[(size_t) id];
    }

    std::vector<PedalId> boardPedals (Board board)
    {
        std::vector<PedalId> pedals;

        for (const auto& info : pedalInfos)
            if (info.board == board)
                pedals.push_back (info.id);

        return pedals;
    }

    // Puts each pedal missing from the list right after the last listed pedal that comes before
    // it in canonical order.
    std::vector<PedalId> withCanonicalPlaces (std::vector<PedalId> listed, const std::vector<PedalId>& missing)
    {
        for (const auto pedal : missing)
        {
            auto position = listed.begin();

            for (auto it = listed.begin(); it != listed.end(); ++it)
                if (*it < pedal)
                    position = it + 1;

            listed.insert (position, pedal);
        }

        return listed;
    }
}

PedalChain::PedalChain (juce::AudioProcessorValueTreeState& stateToUse)
    : state (stateToUse)
{
    publish();
    state.state.addListener (this);
}

PedalChain::~PedalChain()
{
    state.state.removeListener (this);
}

bool PedalChain::isOn (PedalId id) const
{
    return state.getRawParameterValue (infoFor (id).toggleId)->load() > 0.5f;
}

// The stored list of the board, or the default pedals when the state has none.
std::vector<PedalId> PedalChain::getOrder (Board board) const
{
    std::vector<PedalId> listed, missing;

    if (state.state.hasProperty (property))
    {
        for (const auto& key : juce::StringArray::fromTokens (state.state.getProperty (property).toString(), ",", {}))
            for (const auto& info : pedalInfos)
                if (key == info.key && info.board == board
                    && std::find (listed.begin(), listed.end(), info.id) == listed.end())
                    listed.push_back (info.id);
    }
    else
    {
        for (const auto pedal : boardPedals (board))
            if (infoFor (pedal).shownByDefault)
                listed.push_back (pedal);
    }

    for (const auto pedal : boardPedals (board))
        if (std::find (listed.begin(), listed.end(), pedal) == listed.end())
            missing.push_back (pedal);

    return withCanonicalPlaces (listed, missing);
}

std::vector<PedalId> PedalChain::getShown (Board board) const
{
    const auto stored = state.state.getProperty (property).toString();
    const auto tokens = juce::StringArray::fromTokens (stored, ",", {});
    std::vector<PedalId> shown;

    for (const auto pedal : getOrder (board))
    {
        const auto& info = infoFor (pedal);
        const auto listed = state.state.hasProperty (property) ? tokens.contains (info.key) : info.shownByDefault;

        if (listed || isOn (pedal))
            shown.push_back (pedal);
    }

    return shown;
}

std::vector<PedalId> PedalChain::getHidden (Board board) const
{
    const auto shown = getShown (board);
    std::vector<PedalId> hidden;

    for (const auto pedal : boardPedals (board))
        if (std::find (shown.begin(), shown.end(), pedal) == shown.end())
            hidden.push_back (pedal);

    return hidden;
}

void PedalChain::store (const std::vector<PedalId>& shownPre, const std::vector<PedalId>& shownPost)
{
    juce::StringArray keys;

    for (const auto* list : { &shownPre, &shownPost })
        for (const auto pedal : *list)
            keys.add (infoFor (pedal).key);

    state.state.setProperty (property, keys.joinIntoString (","), nullptr);
}

void PedalChain::show (PedalId id)
{
    const auto board = infoFor (id).board;
    auto shown = withCanonicalPlaces (getShown (board), { id });
    const auto other = getShown (board == Board::pre ? Board::post : Board::pre);
    board == Board::pre ? store (shown, other) : store (other, shown);

    if (auto* toggle = state.getParameter (infoFor (id).toggleId))
        toggle->setValueNotifyingHost (1.0f);
}

void PedalChain::hide (PedalId id)
{
    if (auto* toggle = state.getParameter (infoFor (id).toggleId))
        toggle->setValueNotifyingHost (0.0f);

    const auto board = infoFor (id).board;
    auto shown = getShown (board);
    shown.erase (std::remove (shown.begin(), shown.end(), id), shown.end());
    const auto other = getShown (board == Board::pre ? Board::post : Board::pre);
    board == Board::pre ? store (shown, other) : store (other, shown);
}

void PedalChain::move (PedalId id, int newShownIndex)
{
    const auto board = infoFor (id).board;
    auto shown = getShown (board);
    shown.erase (std::remove (shown.begin(), shown.end(), id), shown.end());
    shown.insert (shown.begin() + juce::jlimit (0, (int) shown.size(), newShownIndex), id);
    const auto other = getShown (board == Board::pre ? Board::post : Board::pre);
    board == Board::pre ? store (shown, other) : store (other, shown);
}

void PedalChain::publish()
{
    for (const auto board : { Board::pre, Board::post })
    {
        const auto offset = board == Board::pre ? 0 : numPre;
        juce::uint64 packed = 0;
        auto shift = 0;

        for (const auto pedal : getOrder (board))
        {
            packed |= (juce::uint64) ((int) pedal - offset) << shift;
            shift += 4;
        }

        order[(size_t) board].store (packed, std::memory_order_release);
    }

    sendChangeMessage();
}

void PedalChain::valueTreePropertyChanged (juce::ValueTree& tree, const juce::Identifier& changed)
{
    if (tree == state.state && changed == property)
        publish();
}

void PedalChain::valueTreeRedirected (juce::ValueTree&)
{
    publish();
}
