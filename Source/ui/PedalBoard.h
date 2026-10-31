#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "Pedal.h"

// One pedalboard, before or after the amp: the shown pedals in chain order (scrolling when
// they don't fit), an ADD slot that brings in a hidden pedal, an × on each pedal to take it
// off, and drag-and-drop to reorder.
class PedalBoard : public juce::Component, private juce::ChangeListener, private juce::Timer
{
public:
    PedalBoard (BassAmpProcessor&, Board);
    ~PedalBoard() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    class Content : public juce::Component
    {
    public:
        explicit Content (PedalBoard& boardToUse) : board (boardToUse) {}
        void paintOverChildren (juce::Graphics&) override;

    private:
        PedalBoard& board;
    };

    class AddSlot : public juce::Button
    {
    public:
        AddSlot() : juce::Button ("Add pedal") { setTooltip (Help::get ("ui.addPedal")); }
        void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    };

    void changeListenerCallback (juce::ChangeBroadcaster*) override;
    void timerCallback() override;
    void refresh();
    void layoutPedals();
    void showAddMenu();
    Pedal* pedalFor (PedalId) const;
    int dropIndexFor (float contentX) const;
    void paintDropIndicator (juce::Graphics&) const;

    BassAmpProcessor& processor;
    PedalChain& chain;
    Board board;

    std::vector<std::unique_ptr<Pedal>> pedals;
    std::vector<PedalId> shown;

    juce::Viewport viewport;
    Content content { *this };
    AddSlot addSlot;

    Pedal* dragged = nullptr;
    int dragOffset = 0;
    int dropIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalBoard)
};
