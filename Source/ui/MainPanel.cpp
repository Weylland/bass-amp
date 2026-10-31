#include "MainPanel.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "../ParameterIds.h"

namespace
{
    constexpr int margin = 12;
    constexpr int headerHeight = 56;
    constexpr int practiceHeight = 84;

    juce::Component* findShowingChild (const juce::Component& parent, const juce::String& componentId)
    {
        for (auto* child : parent.getChildren())
        {
            if (! child->isShowing())
                continue;

            if (child->getComponentID() == componentId)
                return child;

            if (auto* found = findShowingChild (*child, componentId))
                return found;
        }

        return nullptr;
    }

    // Every control bound to a parameter carries its ID: that is also its help key.
    void addParameterHelp (juce::Component& parent)
    {
        for (auto* child : parent.getChildren())
        {
            if (auto* client = dynamic_cast<juce::SettableTooltipClient*> (child))
                if (const auto text = Help::get (child->getComponentID()); text.isNotEmpty())
                    client->setTooltip (text);

            addParameterHelp (*child);
        }
    }
}

MainPanel::MainPanel (BassAmpProcessor& processorToUse)
    : processor (processorToUse),
      presetBar (processorToUse.presets),
      recorderBar (processorToUse.recorder),
      preBoard (processorToUse, Board::pre),
      tuner (processorToUse),
      ampHead (processorToUse),
      meters (processorToUse),
      equalizer (processorToUse),
      postBoard (processorToUse, Board::post),
      player (processorToUse),
      looperUnit (processorToUse),
      metronomeUnit (processorToUse)
{
    for (auto* component : std::initializer_list<juce::Component*> {
             &presetBar, &recorderBar, &preBoard, &tuner, &ampHead, &meters, &equalizer, &postBoard,
             &player, &looperUnit, &metronomeUnit })
        addAndMakeVisible (component);

    addParameterHelp (*this);
    addMouseListener (this, true);
    shownDeviceInfo = processor.audioDeviceInfo;
    setSize (designWidth, designHeight);
    startTimerHz (15);
}

MainPanel::~MainPanel()
{
    stopTimer();
}

void MainPanel::timerCallback()
{
    if (processor.audioDeviceInfo != shownDeviceInfo)
    {
        shownDeviceInfo = processor.audioDeviceInfo;
        repaint (headerArea);
    }

    const auto learning = processor.midiLearn.getLearningParameter();

    if (learning.isNotEmpty() || learning != shownLearning)
    {
        repaint (learningArea());
        shownLearning = learning;
        repaint (learningArea());
    }
}

juce::Rectangle<int> MainPanel::learningArea() const
{
    if (auto* control = findShowingChild (*this, shownLearning))
        return getLocalArea (control, control->getLocalBounds()).expanded (48, 8).withTrimmedBottom (-24);

    return {};
}

void MainPanel::mouseDown (const juce::MouseEvent& event)
{
    if (! event.mods.isPopupMenu())
        return;

    for (auto* component = event.eventComponent; component != nullptr && component != this; component = component->getParentComponent())
    {
        if (processor.parameters.getParameter (component->getComponentID()) != nullptr)
        {
            showMidiMenu (component->getComponentID());
            return;
        }
    }
}

void MainPanel::showMidiMenu (const juce::String& paramId)
{
    auto& midi = processor.midiLearn;
    const auto mapping = midi.describe (paramId);

    juce::PopupMenu menu;
    menu.addSectionHeader (processor.parameters.getParameter (paramId)->getName (64) + (mapping.isNotEmpty() ? "  (" + mapping + ")" : ""));

    if (midi.getLearningParameter() == paramId)
        menu.addItem ("Cancel MIDI learn", [&midi] { midi.cancelLearning(); });
    else
        menu.addItem ("MIDI learn", [&midi, paramId] { midi.learn (paramId); });

    if (mapping.isNotEmpty())
        menu.addItem ("Forget " + mapping, [&midi, paramId] { midi.forget (paramId); });

    menu.showMenuAsync (juce::PopupMenu::Options());
}

void MainPanel::paintOverChildren (juce::Graphics& g)
{
    if (shownLearning.isEmpty())
        return;

    auto* control = findShowingChild (*this, shownLearning);

    if (control == nullptr)
        return;

    const auto area = getLocalArea (control, control->getLocalBounds()).toFloat().expanded (4.0f);
    const auto pulse = 0.55f + 0.45f * std::sin ((float) juce::Time::getMillisecondCounter() * 0.008f);

    g.setColour (Palette::gold.withAlpha (0.25f * pulse));
    g.fillRoundedRectangle (area, 6.0f);
    g.setColour (Palette::gold.withAlpha (pulse));
    g.drawRoundedRectangle (area, 6.0f, 2.0f);

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (11.0f));
    g.drawText ("MOVE A MIDI CONTROL", area.withY (area.getBottom() + 2.0f).withHeight (16.0f).expanded (40.0f, 0.0f),
                juce::Justification::centredTop);
}

void MainPanel::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff23252b), bounds.getCentreX(), bounds.getY() + bounds.getHeight() * 0.35f,
                                             juce::Colour (0xff08080a), bounds.getX(), bounds.getBottom(), true));
    g.fillRect (bounds);

    auto header = headerArea.reduced (margin + 8, 0);
    const auto logo = header.removeFromLeft (220).toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff7dc9c), 0.0f, logo.getY() + 10.0f, juce::Colour (0xffb7832f), 0.0f, logo.getBottom() - 10.0f, false));
    g.setFont (Fonts::display (34.0f).withKerningFactor (0.1f));
    g.drawText ("BASS AMP", logo, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (Fonts::display (12.0f, false));
    g.drawFittedText (shownDeviceInfo, header.withLeft (recorderBar.getRight() + margin), juce::Justification::centredRight, 1, 0.8f);
}

void MainPanel::resized()
{
    auto area = getLocalBounds().reduced (margin, 0);
    headerArea = area.removeFromTop (headerHeight);

    presetBar.setBounds (juce::Rectangle<int> (440, 32).withPosition (250, (headerHeight - 32) / 2 + 2));
    recorderBar.setBounds (juce::Rectangle<int> (200, 30).withPosition (presetBar.getRight() + 28, (headerHeight - 30) / 2 + 2));

    area.removeFromTop (6);
    preBoard.setBounds (area.removeFromTop (236));

    area.removeFromTop (margin);
    auto middle = area.removeFromTop (240);
    tuner.setBounds (middle.removeFromLeft (232));
    middle.removeFromLeft (margin);
    meters.setBounds (middle.removeFromRight (232));
    middle.removeFromRight (margin);
    ampHead.setBounds (middle);

    area.removeFromTop (margin);
    auto practice = area.removeFromBottom (practiceHeight + margin).withTrimmedBottom (margin);
    player.setBounds (practice.removeFromLeft (556));
    practice.removeFromLeft (margin);
    metronomeUnit.setBounds (practice.removeFromRight (376));
    practice.removeFromRight (margin);
    looperUnit.setBounds (practice);

    auto bottom = area.withTrimmedBottom (margin);
    equalizer.setBounds (bottom.removeFromLeft (532));
    bottom.removeFromLeft (margin);
    postBoard.setBounds (bottom);
}
