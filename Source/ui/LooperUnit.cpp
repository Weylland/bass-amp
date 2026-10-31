#include "LooperUnit.h"
#include "AmpLookAndFeel.h"
#include "Help.h"

namespace
{
    constexpr int sidePadding = 14;
    constexpr int titleHeight = 22;
    constexpr int gap = 6;

    struct StateLook
    {
        const char* name;
        const char* nextAction;
        juce::Colour colour;
    };

    StateLook lookFor (Looper::State state)
    {
        switch (state)
        {
            case Looper::State::recording:   return { "RECORDING", "PLAY", Palette::ledRed };
            case Looper::State::playing:     return { "PLAYING", "DUB", Palette::ledGreen };
            case Looper::State::overdubbing: return { "OVERDUB", "PLAY", Palette::ledYellow };
            case Looper::State::stopped:     return { "STOPPED", "PLAY", Palette::textDim };
            case Looper::State::empty:       break;
        }

        return { "EMPTY", "REC", Palette::textDim };
    }
}

LooperUnit::LooperUnit (BassAmpProcessor& processor)
    : looper (processor.looper)
{
    mainButton.onClick = [this] { looper.press (Looper::Command::main); };
    stopButton.onClick = [this] { looper.press (Looper::Command::stop); };
    clearButton.onClick = [this] { looper.press (Looper::Command::clear); };

    level.setRange (0.0, 1.0);
    level.setValue (looper.level.load(), juce::dontSendNotification);
    level.getProperties().set (KnobStyle::property, KnobStyle::pedal);
    level.onValueChange = [this] { looper.level.store ((float) level.getValue()); };

    setTooltip (Help::get ("ui.looper"));
    mainButton.setTooltip (Help::get ("ui.looperMain"));
    stopButton.setTooltip (Help::get ("ui.looperStop"));
    clearButton.setTooltip (Help::get ("ui.looperClear"));
    level.setTooltip (Help::get ("ui.looperLevel"));

    for (auto* component : std::initializer_list<juce::Component*> { &mainButton, &stopButton, &clearButton, &level })
        addAndMakeVisible (component);

    startTimerHz (15);
}

LooperUnit::~LooperUnit()
{
    stopTimer();
}

void LooperUnit::timerCallback()
{
    const auto state = looper.getState();

    if (state != shownState)
    {
        shownState = state;
        mainButton.setButtonText (lookFor (state).nextAction);
    }

    repaint (screenArea);
}

void LooperUnit::resized()
{
    auto area = getLocalBounds().reduced (sidePadding, 8).withTrimmedBottom (4);
    titleArea = area.removeFromTop (titleHeight);
    auto row = area.withSizeKeepingCentre (area.getWidth(), 30);

    level.setBounds (row.removeFromRight (40).withSizeKeepingCentre (40, 40));
    levelLabelArea = titleArea.withLeft (level.getX() - 10).withRight (level.getRight() + 10);
    row.removeFromRight (gap);

    mainButton.setBounds (row.removeFromLeft (52).reduced (0, 2));
    row.removeFromLeft (gap);
    stopButton.setBounds (row.removeFromLeft (44).reduced (0, 2));
    row.removeFromLeft (gap);
    clearButton.setBounds (row.removeFromLeft (56).reduced (0, 2));
    row.removeFromLeft (gap);
    screenArea = row;
}

void LooperUnit::paint (juce::Graphics& g)
{
    Draw::unitChassis (g, getLocalBounds().toFloat());

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (15.0f).withKerningFactor (0.12f));
    g.drawText ("LOOPER", titleArea, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (Fonts::label (10.0f));
    g.drawText ("LEVEL", levelLabelArea, juce::Justification::centredBottom);

    const auto look = lookFor (shownState);
    Draw::screen (g, screenArea.toFloat(), 5.0f);
    auto inner = screenArea.reduced (8, 3);
    auto bar = inner.removeFromBottom (5).toFloat();

    g.setFont (Fonts::display (11.0f));
    g.setColour (look.colour);
    g.drawText (look.name, inner, juce::Justification::centredLeft);

    const auto seconds = looper.getLengthSeconds();
    g.setColour (Palette::text.withAlpha (0.7f));
    g.drawText (seconds > 0.0 ? juce::String (seconds, 1) + " s" : juce::String(), inner, juce::Justification::centredRight);

    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.fillRoundedRectangle (bar, 2.0f);
    g.setColour (look.colour.withAlpha (0.85f));
    g.fillRoundedRectangle (bar.withWidth (bar.getWidth() * looper.getProgress()), 2.0f);
}
