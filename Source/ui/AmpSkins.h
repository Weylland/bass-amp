#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "AmpLookAndFeel.h"

// The look of the amp head for each model, in the same order as ampModels.
struct AmpSkin
{
    enum class Covering { tolex, tweed, check, metal };

    Covering covering;
    juce::Colour cabinet;    // covering colour
    juce::Colour panel;      // control panel
    bool brushed;            // brushed metal panel, or painted
    juce::Colour ink;        // panel silkscreen
    juce::Colour accent;     // badge, pilot light and value arcs
    const char* knobStyle;
    juce::Colour knob;       // knob body or cap
    juce::Colour pointer;
    bool tubes;              // glowing valves behind the vent, or a solid-state grille
};

inline const std::array<AmpSkin, 7> ampSkins {{
    // Hi-Fi: black and yellow, Italian solid-state.
    { AmpSkin::Covering::tolex, juce::Colour (0xff151515), juce::Colour (0xff1d1d1f), false,
      juce::Colour (0xfff2c230), juce::Colour (0xfff2c230), KnobStyle::modern, juce::Colour (0xff2a2a2d), juce::Colour (0xfff2c230), false },

    // Tube 300: silver panel, black knurled knobs, black tolex.
    { AmpSkin::Covering::tolex, juce::Colour (0xff17171a), juce::Colour (0xffc4c7cd), true,
      juce::Colour (0xff15161a), juce::Colour (0xff3d6fe0), KnobStyle::skirted, juce::Colour (0xffb8bcc4), juce::Colour (0xfff4f4f4), true },

    // Crossover: matte black, white print, red accent.
    { AmpSkin::Covering::metal, juce::Colour (0xff121214), juce::Colour (0xff222327), false,
      juce::Colour (0xffeeeeee), juce::Colour (0xffe8383d), KnobStyle::modern, juce::Colour (0xff36373c), juce::Colour (0xffe8383d), false },

    // Tweed: lacquered tweed, brown panel, ivory chicken-heads.
    { AmpSkin::Covering::tweed, juce::Colour (0xffcdae74), juce::Colour (0xff3d2a1a), false,
      juce::Colour (0xfff1e4c3), juce::Colour (0xffffa53d), KnobStyle::chicken, juce::Colour (0xfff0e6cf), juce::Colour (0xff1e140c), true },

    // Flip Top: blue check covering, dark metal panel, silver knobs.
    { AmpSkin::Covering::check, juce::Colour (0xff1d3b66), juce::Colour (0xff2c3038), true,
      juce::Colour (0xffdfe7f2), juce::Colour (0xff6aa8ff), KnobStyle::skirted, juce::Colour (0xffd4d8de), juce::Colour (0xfff4f4f4), true },

    // Solid Punch: black head, charcoal panel, red-orange print.
    { AmpSkin::Covering::tolex, juce::Colour (0xff111113), juce::Colour (0xff2b2c30), true,
      juce::Colour (0xffff6a3d), juce::Colour (0xffff5a2a), KnobStyle::skirted, juce::Colour (0xff4b4d53), juce::Colour (0xffff6a3d), false },

    // Tube DI: hammered grey enclosure, black knobs, green light.
    { AmpSkin::Covering::metal, juce::Colour (0xff474b52), juce::Colour (0xff5a5f67), false,
      juce::Colour (0xfff5f5f5), juce::Colour (0xff3ed17a), KnobStyle::pedal, juce::Colour (0xff1a1a1d), juce::Colour (0xfff5f5f5), false },
}};
