#include "RecorderBar.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "Message.h"

namespace
{
    const juce::Colour recordColour { 0xffe0453f };

    juce::String formatTime (double seconds)
    {
        const auto total = (int) seconds;
        return juce::String::formatted ("%02d:%02d", total / 60, total % 60);
    }
}

RecorderBar::RecorderBar (Recorder& recorderToUse)
    : recorder (recorderToUse)
{
    recordButton.setColour (juce::TextButton::buttonOnColourId, recordColour);
    recordButton.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    recordButton.onClick = [this] { toggleRecording(); };
    filesButton.onClick = [this] { showRecordings(); };
    splitSwitch.setToggleState (recorder.separateTracks.load(), juce::dontSendNotification);
    splitSwitch.onClick = [this] { recorder.separateTracks.store (splitSwitch.getToggleState()); };
    recordButton.setTooltip (Help::get ("ui.record"));
    filesButton.setTooltip (Help::get ("ui.recordings"));
    splitSwitch.setTooltip (Help::get ("ui.recordSplit"));

    addAndMakeVisible (recordButton);
    addAndMakeVisible (splitSwitch);
    addAndMakeVisible (filesButton);

    timerCallback();
    startTimerHz (10);
}

RecorderBar::~RecorderBar()
{
    stopTimer();
}

void RecorderBar::toggleRecording()
{
    if (recorder.isRecording())
    {
        recorder.stop();
    }
    else
    {
        const auto name = "Bass Amp " + juce::Time::getCurrentTime().formatted ("%Y-%m-%d %H-%M-%S") + ".wav";
        const auto file = Recorder::getDefaultDirectory().getChildFile (name);

        if (! recorder.start (file))
            Message::show ("Error whilst saving", "Couldn't write to the specified file!", file.getFullPathName());
    }

    timerCallback();
}

void RecorderBar::showRecordings()
{
    const auto lastFile = recorder.getLastFile();

    if (lastFile.existsAsFile())
    {
        lastFile.revealToUser();
        return;
    }

    const auto directory = Recorder::getDefaultDirectory();

    if (directory.createDirectory())
        directory.startAsProcess();
}

void RecorderBar::timerCallback()
{
    const auto recording = recorder.isRecording();
    recordButton.setToggleState (recording, juce::dontSendNotification);
    recordButton.setButtonText (recording ? "STOP" : "REC");
    splitSwitch.setEnabled (! recording);

    const auto text = recording ? formatTime (recorder.getRecordedSeconds()) : timeText;

    if (text != timeText || recording)
    {
        timeText = text;
        repaint (timeArea);
    }
}

void RecorderBar::paint (juce::Graphics& g)
{
    const auto recording = recorder.isRecording();
    const auto blinkOn = recording && (juce::Time::getMillisecondCounter() / 500) % 2 == 0;

    auto dotArea = timeArea.toFloat().removeFromLeft (14.0f);
    g.setColour (blinkOn ? recordColour : Palette::track);
    g.fillEllipse (dotArea.withSizeKeepingCentre (8.0f, 8.0f));

    g.setColour (recording ? Palette::text : Palette::textDim);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (timeText, timeArea.withTrimmedLeft (14), juce::Justification::centredLeft);
}

void RecorderBar::resized()
{
    auto area = getLocalBounds();
    recordButton.setBounds (area.removeFromLeft (58));
    area.removeFromLeft (8);
    timeArea = area.removeFromLeft (62);
    area.removeFromLeft (4);
    splitSwitch.setBounds (area.removeFromLeft (64).withSizeKeepingCentre (64, 24));
    area.removeFromLeft (8);
    filesButton.setBounds (area.removeFromLeft (58));
}
