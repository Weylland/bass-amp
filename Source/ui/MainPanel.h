#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"
#include "AmpHead.h"
#include "EqPanel.h"
#include "LevelMeters.h"
#include "LooperPanel.h"
#include "LooperUnit.h"
#include "MetronomeUnit.h"
#include "PedalBoard.h"
#include "PlayerUnit.h"
#include "PresetBar.h"
#include "RecorderBar.h"
#include "TunerDisplay.h"

// The whole rig at its design size: header, pre-amp pedalboard, tuner, amp head and
// meters, the EQ and the post-amp pedalboard, then the practice row (player, looper,
// metronome). The full looper opens over the pedalboards. The editor scales it to the window.
// Right-clicking a control opens its MIDI learn menu; a gold dot marks the controls a MIDI
// control drives. Hovering a control for a second explains it.
class MainPanel : public juce::Component, private juce::Timer
{
public:
    static constexpr int designWidth = 1320;
    static constexpr int designHeight = 896;

    explicit MainPanel (BassAmpProcessor&);
    ~MainPanel() override;

    // Shows the full screen button in the header, which calls onToggleFullScreen.
    void setFullScreenAvailable (bool);
    std::function<void()> onToggleFullScreen;

    // Called once the language button saved a new interface language.
    std::function<void()> onLanguageChanged;

    void paint (juce::Graphics&) override;
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent&) override;

private:
    class FullScreenButton : public juce::Button
    {
    public:
        FullScreenButton() : juce::Button ("Full Screen") {}
        void paintButton (juce::Graphics&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    };

    void timerCallback() override;
    void showMidiMenu (const juce::String& paramId);
    void showLanguageMenu();
    juce::Rectangle<int> learningArea() const;

    BassAmpProcessor& processor;
    juce::String shownDeviceInfo;
    juce::String shownLearning;
    juce::StringArray shownMapped;

    PresetBar presetBar;
    RecorderBar recorderBar;

    PedalBoard preBoard, postBoard;
    TunerDisplay tuner;
    AmpHead ampHead;
    LevelMeters meters;
    EqPanel equalizer;
    PlayerUnit player;
    LooperUnit looperUnit;
    LooperPanel looperPanel;
    MetronomeUnit metronomeUnit;
    FullScreenButton fullScreenButton;
    juce::TextButton languageButton;
    juce::TooltipWindow tooltipWindow { this, 1000 };

    juce::Rectangle<int> headerArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainPanel)
};
