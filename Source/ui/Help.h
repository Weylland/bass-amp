#pragma once

#include <juce_core/juce_core.h>

// The hover help, in French: what each control does and how to use it. A key is a parameter
// ID, "pedal.<key>", "model.<name>", "cab.<name>" or "ui.<name>". The first line of a text is
// its title. Unknown keys return an empty string, which shows no tooltip.
namespace Help
{
    juce::String get (const juce::String& key);
}
