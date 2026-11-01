#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "ActionButton.h"
#include "LoopPad.h"
#include "LoopTrimView.h"
#include "ToggleSwitch.h"

// The full looper, over the pedalboards: a big pad per track with its STOP, CLEAR, TRIM, REV,
// HALF and level, and the global SYNC, UNDO, REDO, STOP ALL, CLEAR ALL and EXPORT. TRIM opens
// the trim view over the tracks.
class LooperPanel : public juce::Component, private juce::Timer
{
public:
    explicit LooperPanel (BassAmpProcessor&);
    ~LooperPanel() override;

    std::function<void()> onClose;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct TrackColumn
    {
        TrackColumn (Looper&, int track);

        LoopPad pad;
        juce::TextButton stopButton { "STOP" }, clearButton { "CLEAR" }, trimButton { "TRIM" };
        ToggleSwitch reverseSwitch { "REV" }, halfSwitch { "HALF" };
        juce::Slider level { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
        juce::Rectangle<int> labelArea, levelLabelArea;
    };

    void timerCallback() override;
    void exportMix();

    Looper& looper;
    juce::OwnedArray<TrackColumn> columns;
    ToggleSwitch syncSwitch { "SYNC" }, countInSwitch { "COUNT-IN" }, autoSwitch { "AUTO" };
    ActionButton undoButton { "UNDO" }, redoButton { "REDO" }, stopAllButton { "STOP ALL" }, clearAllButton { "CLEAR ALL" };
    juce::TextButton exportButton { "EXPORT WAV" }, closeButton { "CLOSE" };
    juce::String exportMessage;
    LoopTrimView trimView { looper };

    juce::Rectangle<int> titleArea, statusArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LooperPanel)
};
