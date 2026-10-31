#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// The Bass Amp artwork, drawn in code: the app icon and the splash screen.
namespace Logo
{
    // A speaker cone with a gold dust cap in a gold-trimmed metal square.
    void drawIcon (juce::Graphics&, juce::Rectangle<float> square);

    juce::Image createIcon (int size);
    juce::Image createSplash (const juce::String& version);
}
