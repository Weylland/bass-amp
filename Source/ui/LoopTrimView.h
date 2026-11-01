#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../dsp/Looper.h"

// Trims a looper track while it plays: the whole take as a waveform, with a handle at each end
// of the loop. Dragging a handle cuts the start or the end; the part left out is shaded and the
// loop plays the change at once. RESET plays the whole take again. It closes itself when the
// track can no longer be trimmed.
class LoopTrimView : public juce::Component, private juce::Timer
{
public:
    explicit LoopTrimView (Looper&);
    ~LoopTrimView() override;

    void show (int track);
    std::function<void()> onClose;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    void timerCallback() override;
    void close();
    void refreshOverview();
    float xFor (int sample) const;
    int sampleAt (float x) const;
    bool nearStart (float x) const;
    void send();

    Looper& looper;
    int track = 0, ticks = 0;
    bool dragging = false, draggingStart = true;
    Looper::Trim trim;
    std::vector<float> overview;
    juce::TextButton resetButton { "RESET" }, doneButton { "DONE" };
    juce::Rectangle<int> titleArea, hintArea, waveArea, infoArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoopTrimView)
};
