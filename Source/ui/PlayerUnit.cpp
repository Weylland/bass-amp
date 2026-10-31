#include "PlayerUnit.h"
#include "AmpLookAndFeel.h"

namespace
{
    constexpr int sidePadding = 14;
    constexpr int titleHeight = 22;
    constexpr int gap = 6;
    constexpr int timeWidth = 76;

    juce::String formatTime (double seconds)
    {
        const auto total = juce::roundToInt (std::floor (seconds));
        return juce::String (total / 60) + ":" + juce::String (total % 60).paddedLeft ('0', 2);
    }
}

PlayerUnit::PlayerUnit (BassAmpProcessor& processor)
    : track (processor.backingTrack)
{
    loadButton.onClick = [this] { chooseFile(); };
    playButton.onClick = [this] { track.isPlaying() ? track.pause() : track.play(); };
    stopButton.onClick = [this] { track.stop(); };

    loopSwitch.setToggleState (true, juce::dontSendNotification);
    loopSwitch.onClick = [this] { track.setLooping (loopSwitch.getToggleState()); };
    reampSwitch.setTooltip ("Feed the file into the rig, like a DI track, instead of mixing it into the output");
    reampSwitch.onClick = [this] { track.reamp.store (reampSwitch.getToggleState()); };

    position.setRange (0.0, 1.0);
    position.setColour (juce::Slider::trackColourId, Palette::gold);
    position.onDragEnd = [this] { track.setPositionSeconds (position.getValue() * track.getLengthSeconds()); };

    volume.setRange (0.0, 1.0);
    volume.setValue (track.volume.load(), juce::dontSendNotification);
    volume.getProperties().set (KnobStyle::property, KnobStyle::pedal);
    volume.onValueChange = [this] { track.volume.store ((float) volume.getValue()); };

    for (auto* component : std::initializer_list<juce::Component*> { &loadButton, &playButton, &stopButton, &loopSwitch,
                                                                     &reampSwitch, &position, &volume })
        addAndMakeVisible (component);

    startTimerHz (10);
}

PlayerUnit::~PlayerUnit()
{
    stopTimer();
}

void PlayerUnit::chooseFile()
{
    fileChooser = std::make_unique<juce::FileChooser> ("Choose a backing track",
                                                       juce::File::getSpecialLocation (juce::File::userMusicDirectory),
                                                       "*.wav;*.aif;*.aiff;*.flac;*.ogg;*.mp3");

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [safeThis = juce::Component::SafePointer<PlayerUnit> (this)] (const juce::FileChooser& chooser)
    {
        if (safeThis != nullptr && chooser.getResult().existsAsFile() && safeThis->track.load (chooser.getResult()))
        {
            safeThis->track.play();
            safeThis->repaint();
        }
    });
}

void PlayerUnit::timerCallback()
{
    playButton.setButtonText (track.isPlaying() ? "PAUSE" : "PLAY");
    const auto length = track.getLengthSeconds();

    if (! position.isMouseButtonDown())
        position.setValue (length > 0.0 ? track.getPositionSeconds() / length : 0.0, juce::dontSendNotification);

    repaint (screenArea);
}

void PlayerUnit::resized()
{
    auto area = getLocalBounds().reduced (sidePadding, 8).withTrimmedBottom (4);
    titleArea = area.removeFromTop (titleHeight);
    auto row = area.withSizeKeepingCentre (area.getWidth(), 30);

    volume.setBounds (row.removeFromRight (40).withSizeKeepingCentre (40, 40));
    volumeLabelArea = titleArea.withLeft (volume.getX() - 10).withRight (volume.getRight() + 10);
    row.removeFromRight (gap);

    for (auto* button : { &loadButton, &playButton, &stopButton })
    {
        button->setBounds (row.removeFromLeft (button == &stopButton ? 48 : 54).reduced (0, 2));
        row.removeFromLeft (gap);
    }

    reampSwitch.setBounds (row.removeFromRight (78).reduced (0, 3));
    row.removeFromRight (gap);
    loopSwitch.setBounds (row.removeFromRight (62).reduced (0, 3));
    row.removeFromRight (gap);

    screenArea = row;
    position.setBounds (screenArea.reduced (8, 0).removeFromBottom (14).withTrimmedRight (timeWidth));
}

void PlayerUnit::paint (juce::Graphics& g)
{
    Draw::unitChassis (g, getLocalBounds().toFloat());

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (15.0f).withKerningFactor (0.12f));
    g.drawText ("PLAYER", titleArea, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (Fonts::label (10.0f));
    g.drawText ("VOL", volumeLabelArea, juce::Justification::centredBottom);

    Draw::screen (g, screenArea.toFloat(), 5.0f);
    auto text = screenArea.reduced (8, 1);
    const auto timeArea = text.removeFromBottom (14).removeFromRight (timeWidth);
    const auto length = track.getLengthSeconds();

    g.setFont (Fonts::display (11.0f, false));
    g.setColour (Palette::text.withAlpha (0.8f));
    const auto time = length > 0.0 ? formatTime (track.getPositionSeconds()) + " / " + formatTime (length) : juce::String();
    g.drawText (time, timeArea, juce::Justification::centredRight);

    g.setFont (Fonts::display (12.0f, false));
    const auto file = track.getFile();
    g.setColour (file.existsAsFile() ? Palette::ledGreen : Palette::textDim);
    g.drawFittedText (file.existsAsFile() ? file.getFileNameWithoutExtension() : juce::String ("No track loaded"),
                      text, juce::Justification::centredLeft, 1, 0.8f);
}
