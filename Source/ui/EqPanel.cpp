#include "EqPanel.h"
#include "AmpLookAndFeel.h"
#include "Help.h"

namespace
{
    constexpr int padding = 12;
    constexpr int headerHeight = 28;
}

EqPanel::EqPanel (BassAmpProcessor& processorToUse)
    : processor (processorToUse),
      display (processorToUse.parameters, processorToUse.spectrum)
{
    onParam = processor.parameters.getRawParameterValue (ParamIds::eqOn);
    modeParam = processor.parameters.getRawParameterValue (ParamIds::eqMode);

    onSwitch.setColour (juce::ToggleButton::tickColourId, Palette::gold);
    onAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.parameters, ParamIds::eqOn, onSwitch);
    onSwitch.setComponentID (ParamIds::eqOn);

    basicButton.onClick = [this] { setMode (Equalizer::basicMode); };
    advancedButton.onClick = [this] { setMode (Equalizer::advancedMode); };
    basicButton.setTooltip (Help::get ("ui.eqBasic"));
    advancedButton.setTooltip (Help::get ("ui.eqAdvanced"));

    for (auto* component : std::initializer_list<juce::Component*> { &onSwitch, &basicButton, &advancedButton, &display })
        addAndMakeVisible (component);

    timerCallback();
    startTimerHz (30);
}

EqPanel::~EqPanel()
{
    stopTimer();
}

void EqPanel::setMode (int mode)
{
    if (auto* parameter = processor.parameters.getParameter (ParamIds::eqMode))
        parameter->setValueNotifyingHost (parameter->convertTo0to1 ((float) mode));

    timerCallback();
}

void EqPanel::timerCallback()
{
    const auto advanced = juce::roundToInt (modeParam->load()) == Equalizer::advancedMode;
    basicButton.setToggleState (! advanced, juce::dontSendNotification);
    advancedButton.setToggleState (advanced, juce::dontSendNotification);
    display.setMode (advanced, onParam->load() > 0.5f);
    display.updateSpectrum();
}

void EqPanel::paint (juce::Graphics& g)
{
    Draw::unitChassis (g, getLocalBounds().toFloat());

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (20.0f).withKerningFactor (0.12f));
    g.drawText ("EQUALIZER", titleArea, juce::Justification::centredLeft);
}

void EqPanel::resized()
{
    auto area = getLocalBounds().reduced (padding + 8, padding).withTrimmedBottom (4);
    auto header = area.removeFromTop (headerHeight);

    onSwitch.setBounds (header.removeFromRight (64).withSizeKeepingCentre (64, 24));
    header.removeFromRight (12);
    advancedButton.setBounds (header.removeFromRight (96).withSizeKeepingCentre (96, 24));
    header.removeFromRight (4);
    basicButton.setBounds (header.removeFromRight (70).withSizeKeepingCentre (70, 24));
    titleArea = header;

    area.removeFromTop (8);
    display.setBounds (area);
}
