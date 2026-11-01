#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../i18n/Language.h"

// A short notice, in the interface language, about something that couldn't be done.
namespace Message
{
    inline void show (const char* title, const char* text, const juce::String& detail)
    {
        juce::AlertWindow::showAsync (juce::MessageBoxOptions()
                                          .withIconType (juce::MessageBoxIconType::NoIcon)
                                          .withTitle (Language::tr (title))
                                          .withMessage (Language::tr (text) + "\n\n" + detail)
                                          .withButton (Language::tr ("OK")),
                                      nullptr);
    }
}
