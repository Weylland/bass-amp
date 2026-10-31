#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "../PluginProcessor.h"

// The amp head: model badge and selector, glowing valves (or a grille on solid-state
// models), cabinet and impulse response choice, and the control panel. Its look follows
// the selected model.
class AmpHead : public juce::Component, private juce::Timer
{
public:
    explicit AmpHead (BassAmpProcessor&);
    ~AmpHead() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
        juce::String label;
    };

    struct Switch
    {
        std::unique_ptr<juce::ToggleButton> button;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
        juce::String label;
    };

    void timerCallback() override;
    void applySkin();
    void chooseImpulseResponse();
    juce::String impulseResponseText() const;

    void paintStatic (juce::Graphics&);
    void paintCovering (juce::Graphics&, juce::Rectangle<float>);
    void paintPanel (juce::Graphics&);
    void paintVent (juce::Graphics&);
    void paintTubes (juce::Graphics&, juce::Rectangle<float>);
    void paintGrille (juce::Graphics&, juce::Rectangle<float>);

    BassAmpProcessor& processor;
    std::atomic<float>* modelParam = nullptr;
    std::atomic<float>* cabParam = nullptr;
    int currentModel = -1;
    int currentCab = -1;

    std::vector<Knob> knobs;
    std::vector<Switch> switches;
    juce::ComboBox modelBox, cabBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modelAttachment, cabAttachment;
    juce::TextButton irButton { "LOAD IR" };
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::Rectangle<int> frameArea, topArea, badgeArea, ventArea, cabArea, panelArea, pilotArea, switchArea;
    juce::Image staticLayer;
    float staticLayerScale = 0.0f;
    float glow = 0.0f;
    juce::String shownImpulseResponse;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpHead)
};
