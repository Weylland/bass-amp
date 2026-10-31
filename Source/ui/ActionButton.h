#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// A text button that leaves right-clicks alone, so they open the MIDI learn menu of the
// action it triggers without triggering it.
class ActionButton : public juce::TextButton
{
public:
    using juce::TextButton::TextButton;

    void mouseDown (const juce::MouseEvent& event) override
    {
        if (! event.mods.isPopupMenu())
            juce::TextButton::mouseDown (event);
    }

    void mouseUp (const juce::MouseEvent& event) override
    {
        if (! event.mods.isPopupMenu())
            juce::TextButton::mouseUp (event);
    }
};
