#include "EqPanel.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "ModulePresetMenu.h"

namespace
{
    constexpr int padding = 12;
    constexpr int headerHeight = 28;
    constexpr int cutsWidth = 88;
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
    presetsButton.onClick = [this] { showModulePresetMenu (processor.parameters, "eq", presetsButton); };
    presetsButton.setTooltip (Help::get ("ui.eqPresets"));

    for (auto* component : std::initializer_list<juce::Component*> { &onSwitch, &basicButton, &advancedButton, &presetsButton, &display })
        addAndMakeVisible (component);

    using Cut = std::pair<const char*, const char*>;

    for (const auto& [paramId, label] : { Cut { ParamIds::cutLow, "LOW CUT" }, Cut { ParamIds::killLow, "KILL LOW" },
                                          Cut { ParamIds::killMid, "KILL MID" }, Cut { ParamIds::killHigh, "KILL HIGH" },
                                          Cut { ParamIds::cutHigh, "HIGH CUT" } })
    {
        auto* toggle = cutSwitches.add (new ToggleSwitch (label));
        toggle->setComponentID (paramId);
        cutAttachments.add (new juce::AudioProcessorValueTreeState::ButtonAttachment (processor.parameters, paramId, *toggle));
        addAndMakeVisible (toggle);
    }

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
    header.removeFromRight (12);
    presetsButton.setBounds (header.removeFromRight (78).withSizeKeepingCentre (78, 24));
    titleArea = header;

    area.removeFromTop (8);
    auto cuts = area.removeFromRight (cutsWidth);
    area.removeFromRight (8);
    display.setBounds (area);

    const auto cutHeight = cuts.getHeight() / cutSwitches.size();

    for (auto* toggle : cutSwitches)
        toggle->setBounds (cuts.removeFromTop (cutHeight).withSizeKeepingCentre (cutsWidth, juce::jmin (24, cutHeight - 4)));
}
