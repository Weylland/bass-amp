#include "LoopPad.h"
#include "AmpLookAndFeel.h"
#include "Help.h"

LoopPad::LoopPad (Looper& looperToUse, int trackIndex)
    : juce::Button ("Track " + juce::String (trackIndex + 1)), looper (looperToUse), track (trackIndex)
{
    setComponentID (Looper::midiActions[(size_t) track].first);
    setTooltip (Help::get ("ui.loopPad"));
    onClick = [this] { looper.pressTrack (track); };
}

juce::Colour LoopPad::colourFor (Looper::State state)
{
    switch (state)
    {
        case Looper::State::armed:
        case Looper::State::finishing:   return Palette::ledRed.withAlpha (0.6f);
        case Looper::State::recording:   return Palette::ledRed;
        case Looper::State::playing:     return Palette::ledGreen;
        case Looper::State::overdubbing: return Palette::ledYellow;
        case Looper::State::stopped:     return Palette::text.withAlpha (0.55f);
        case Looper::State::empty:       break;
    }

    return Palette::textDim.withAlpha (0.5f);
}

juce::String LoopPad::nameFor (Looper::State state)
{
    switch (state)
    {
        case Looper::State::armed:       return "WAIT";
        case Looper::State::recording:   return "REC";
        case Looper::State::finishing:   return "REC >";
        case Looper::State::playing:     return "PLAY";
        case Looper::State::overdubbing: return "DUB";
        case Looper::State::stopped:     return "STOP";
        case Looper::State::empty:       break;
    }

    return "EMPTY";
}

void LoopPad::mouseDown (const juce::MouseEvent& event)
{
    if (! event.mods.isPopupMenu())
        juce::Button::mouseDown (event);
}

void LoopPad::mouseUp (const juce::MouseEvent& event)
{
    if (! event.mods.isPopupMenu())
        juce::Button::mouseUp (event);
}

void LoopPad::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const auto state = looper.getState (track);
    const auto colour = colourFor (state);
    const auto size = (float) juce::jmin (getWidth(), getHeight());
    const auto bounds = getLocalBounds().toFloat().withSizeKeepingCentre (size, size).reduced (2.0f);
    const auto ringWidth = juce::jmax (3.0f, size * 0.06f);
    const auto face = bounds.reduced (ringWidth + 2.0f).translated (0.0f, shouldDrawButtonAsDown ? 1.0f : 0.0f);

    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillEllipse (bounds.translated (0.0f, 2.0f));
    g.setColour (Palette::screen);
    g.fillEllipse (bounds);

    juce::Path ring;
    ring.addCentredArc (bounds.getCentreX(), bounds.getCentreY(), bounds.getWidth() / 2.0f - ringWidth / 2.0f,
                        bounds.getHeight() / 2.0f - ringWidth / 2.0f, 0.0f, 0.0f, juce::MathConstants<float>::twoPi, true);
    g.setColour (colour.withAlpha (0.18f));
    g.strokePath (ring, juce::PathStrokeType (ringWidth));

    if (const auto progress = looper.getProgress (track); progress > 0.0f)
    {
        juce::Path turned;
        turned.addCentredArc (bounds.getCentreX(), bounds.getCentreY(), bounds.getWidth() / 2.0f - ringWidth / 2.0f,
                              bounds.getHeight() / 2.0f - ringWidth / 2.0f, 0.0f, 0.0f,
                              juce::MathConstants<float>::twoPi * progress, true);
        g.setColour (colour);
        g.strokePath (turned, juce::PathStrokeType (ringWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    const auto lit = state != Looper::State::empty;
    g.setGradientFill (juce::ColourGradient (colour.withAlpha (lit ? 0.45f : 0.12f).brighter (0.2f), face.getCentreX(), face.getY(),
                                             colour.withAlpha (lit ? 0.18f : 0.05f), face.getCentreX(), face.getBottom(), false));
    g.fillEllipse (face);
    g.setColour (juce::Colours::white.withAlpha (shouldDrawButtonAsHighlighted ? 0.35f : 0.15f));
    g.drawEllipse (face, 1.0f);

    g.setColour (lit ? Palette::text : Palette::textDim);
    g.setFont (Fonts::display (size * 0.3f));
    g.drawText (juce::String (track + 1), face.withTrimmedBottom (size > 80.0f ? face.getHeight() * 0.3f : 0.0f),
                juce::Justification::centred);

    if (size > 80.0f)
    {
        g.setColour (colour);
        g.setFont (Fonts::display (size * 0.09f));
        g.drawText (nameFor (state), face.withTrimmedTop (face.getHeight() * 0.55f).withTrimmedBottom (face.getHeight() * 0.12f),
                    juce::Justification::centred);
    }
}
