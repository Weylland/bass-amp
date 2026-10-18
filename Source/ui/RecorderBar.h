#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../dsp/Recorder.h"

// REC/STOP button, elapsed time, and a button that shows the recordings in the file explorer.
class RecorderBar : public juce::Component, private juce::Timer
{
public:
    explicit RecorderBar (Recorder&);
    ~RecorderBar() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void toggleRecording();
    void showRecordings();

    Recorder& recorder;
    juce::TextButton recordButton { "REC" }, filesButton { "Files" };
    juce::Rectangle<int> timeArea;
    juce::String timeText { "00:00" };
};
