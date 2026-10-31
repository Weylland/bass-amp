#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// A toggle button that leaves right-clicks alone, so they open the MIDI learn menu
// without flipping the switch.
class ToggleSwitch : public juce::ToggleButton
{
public:
    using juce::ToggleButton::ToggleButton;

    void mouseDown (const juce::MouseEvent& event) override
    {
        if (! event.mods.isPopupMenu())
            juce::ToggleButton::mouseDown (event);
    }

    void mouseUp (const juce::MouseEvent& event) override
    {
        if (! event.mods.isPopupMenu())
            juce::ToggleButton::mouseUp (event);
    }
};
