#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../dsp/Looper.h"

// One looper track as a round pad: a click records, plays and overdubs in turn. The ring
// shows the loop turning, the colour the track's state. Right-clicks are left to the MIDI
// learn menu. The owner repaints it on a timer.
class LoopPad : public juce::Button
{
public:
    LoopPad (Looper&, int track);

    static juce::Colour colourFor (Looper::State);
    static juce::String nameFor (Looper::State);

    void mouseDown (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    Looper& looper;
    int track;
};
