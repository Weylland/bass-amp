#include "MainPanel.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "../ParameterIds.h"
#include "../i18n/Language.h"

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
      looperPanel (processorToUse),
      metronomeUnit (processorToUse)
{
    for (auto* component : std::initializer_list<juce::Component*> {
             &presetBar, &recorderBar, &preBoard, &tuner, &ampHead, &meters, &equalizer, &postBoard,
             &player, &looperUnit, &metronomeUnit })
        addAndMakeVisible (component);

    addChildComponent (looperPanel);
    addChildComponent (fullScreenButton);
    fullScreenButton.setTooltip (Help::get ("ui.fullScreen"));
    fullScreenButton.onClick = [this] { if (onToggleFullScreen != nullptr) onToggleFullScreen(); };
    for (auto* button : std::initializer_list<juce::Button*> { &audioButton, &deviceInfo })
    {
        addChildComponent (button);
        button->setTooltip (Help::get ("ui.audioSettings"));
        button->onClick = [this] { if (processor.showAudioSettings != nullptr) processor.showAudioSettings(); };
    }

    addChildComponent (updateButton);
    updateButton.setColour (juce::TextButton::textColourOffId, Palette::gold);
    updateButton.setTooltip (Help::get ("ui.update"));
    updateButton.onClick = [this] { juce::URL (updateCheck->getNewerRelease().url).launchInDefaultBrowser(); };
    addAndMakeVisible (languageButton);
    languageButton.setButtonText (Language::getCurrent().toUpperCase());
    languageButton.setTooltip (Help::get ("ui.language"));
    languageButton.onClick = [this] { showLanguageMenu(); };
    looperUnit.onOpen = [this] { looperPanel.setVisible (! looperPanel.isVisible()); };
    looperPanel.onClose = [this] { looperPanel.setVisible (false); };

    addParameterHelp (*this);
    addMouseListener (this, true);
    setSize (designWidth, designHeight);
    timerCallback();
    startTimerHz (15);
}

MainPanel::~MainPanel()
{
    stopTimer();
}

void MainPanel::setFullScreenAvailable (bool available)
{
    fullScreenButton.setVisible (available);
    resized();
    repaint (headerArea);
}

void MainPanel::showLanguageMenu()
{
    juce::PopupMenu menu;
    menu.addSectionHeader (Language::tr ("Language"));

    for (const auto& info : Language::available)
    {
        const juce::String code (info.code);
        menu.addItem (juce::String::fromUTF8 (info.name), true, code == Language::getCurrent(), [this, code]
        {
            if (code == Language::getCurrent())
                return;

            Language::setCurrent (code);

            if (onLanguageChanged != nullptr)
                onLanguageChanged();
        });
    }

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (languageButton));
}

void MainPanel::FullScreenButton::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const auto bounds = getLocalBounds().toFloat().reduced (1.0f).translated (0.0f, shouldDrawButtonAsDown ? 1.0f : 0.0f);
    g.setColour (juce::Colours::white.withAlpha (shouldDrawButtonAsHighlighted ? 0.12f : 0.05f));
    g.fillRoundedRectangle (bounds, 5.0f);

    const auto icon = bounds.reduced (bounds.getWidth() * 0.27f);
    const auto arm = icon.getWidth() * 0.38f;
    juce::Path corners;

    for (const auto& [corner, dx, dy] : { std::tuple { icon.getTopLeft(), 1.0f, 1.0f }, std::tuple { icon.getTopRight(), -1.0f, 1.0f },
                                          std::tuple { icon.getBottomLeft(), 1.0f, -1.0f }, std::tuple { icon.getBottomRight(), -1.0f, -1.0f } })
    {
        corners.startNewSubPath (corner.translated (dx * arm, 0.0f));
        corners.lineTo (corner);
        corners.lineTo (corner.translated (0.0f, dy * arm));
    }

    g.setColour (shouldDrawButtonAsHighlighted ? Palette::gold : Palette::textDim);
    g.strokePath (corners, juce::PathStrokeType (1.6f, juce::PathStrokeType::mitered, juce::PathStrokeType::square));
}

void MainPanel::DeviceInfoButton::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool)
{
    g.setColour (shouldDrawButtonAsHighlighted ? Palette::gold : Palette::textDim);
    g.setFont (Fonts::display (12.0f, false));
    g.drawFittedText (getButtonText(), getLocalBounds(), juce::Justification::centredRight, 2, 0.8f);
}

void MainPanel::timerCallback()
{
    if (processor.audioDeviceInfo != deviceInfo.getButtonText())
        deviceInfo.setButtonText (processor.audioDeviceInfo);

    if (const auto available = processor.showAudioSettings != nullptr; available != audioButton.isVisible())
    {
        audioButton.setVisible (available);
        deviceInfo.setVisible (available);
        resized();
    }

    if (const auto release = updateCheck->getNewerRelease(); release.version != shownUpdate)
    {
        shownUpdate = release.version;
        updateButton.setButtonText ("v" + release.version + " " + Language::tr ("available"));
        updateButton.setVisible (true);
        resized();
        repaint (headerArea);
    }

    if (const auto mapped = processor.midiLearn.getMappedIds(); mapped != shownMapped)
    {
        shownMapped = mapped;
        repaint();
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
    {
        processor.undoHistory.checkpoint();
        return;
    }

    for (auto* component = event.eventComponent; component != nullptr && component != this; component = component->getParentComponent())
    {
        if (processor.midiLearn.isLearnable (component->getComponentID()))
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
    menu.addSectionHeader (midi.getName (paramId) + (mapping.isNotEmpty() ? "  (" + mapping + ")" : ""));

    if (midi.getLearningParameter() == paramId)
        menu.addItem (Language::tr ("Cancel MIDI learn"), [&midi] { midi.cancelLearning(); });
    else
        menu.addItem (Language::tr ("MIDI learn"), [&midi, paramId] { midi.learn (paramId); });

    if (mapping.isNotEmpty())
        menu.addItem (Language::tr ("Forget") + " " + mapping, [&midi, paramId] { midi.forget (paramId); });

    menu.showMenuAsync (juce::PopupMenu::Options());
}

void MainPanel::paintOverChildren (juce::Graphics& g)
{
    for (const auto& id : shownMapped)
    {
        if (auto* control = findShowingChild (*this, id))
        {
            const auto area = getLocalArea (control, control->getLocalBounds()).toFloat();
            const auto dot = juce::Rectangle<float> (7.0f, 7.0f).withCentre (area.getTopRight().translated (-4.0f, 4.0f));
            g.setColour (juce::Colours::black.withAlpha (0.6f));
            g.fillEllipse (dot.expanded (1.5f));
            g.setColour (Palette::gold);
            g.fillEllipse (dot);
        }
    }

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
    g.drawText (Language::tr ("MOVE A MIDI CONTROL"), area.withY (area.getBottom() + 2.0f).withHeight (16.0f).expanded (40.0f, 0.0f),
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
    g.setFont (Fonts::display (10.0f, false));
    const auto logoWidth = juce::GlyphArrangement::getStringWidth (Fonts::display (34.0f).withKerningFactor (0.1f), "BASS AMP");
    g.drawText (juce::String ("v") + JucePlugin_VersionString, logo.withWidth (logoWidth).withTrimmedTop (42.0f).withHeight (12.0f),
                juce::Justification::centredRight);
}

void MainPanel::resized()
{
    auto area = getLocalBounds().reduced (margin, 0);
    headerArea = area.removeFromTop (headerHeight);

    presetBar.setBounds (juce::Rectangle<int> (440, 32).withPosition (250, (headerHeight - 32) / 2 + 2));
    recorderBar.setBounds (juce::Rectangle<int> (200, 30).withPosition (presetBar.getRight() + 28, (headerHeight - 30) / 2 + 2));
    fullScreenButton.setBounds (juce::Rectangle<int> (30, 30).withPosition (headerArea.getRight() - 8 - 30, (headerHeight - 30) / 2 + 2));
    const auto languageRight = fullScreenButton.isVisible() ? fullScreenButton.getX() - 8 : fullScreenButton.getRight();
    languageButton.setBounds (juce::Rectangle<int> (40, 30).withPosition (languageRight - 40, (headerHeight - 30) / 2 + 2));
    audioButton.setBounds (juce::Rectangle<int> (66, 30).withPosition (languageButton.getX() - 8 - 66, (headerHeight - 30) / 2 + 2));
    const auto updateRight = audioButton.isVisible() ? audioButton.getX() : languageButton.getX();
    const auto updateWidth = juce::roundToInt (juce::GlyphArrangement::getStringWidth (Fonts::label (13.0f), updateButton.getButtonText())) + 24;
    updateButton.setBounds (juce::Rectangle<int> (updateWidth, 30).withPosition (updateRight - 8 - updateWidth, (headerHeight - 30) / 2 + 2));

    const auto infoRight = updateButton.isVisible() ? updateButton.getX() : updateRight;
    deviceInfo.setBounds (headerArea.withLeft (recorderBar.getRight() + margin).withRight (infoRight - margin).reduced (0, 8));

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
    player.setBounds (practice.removeFromLeft (520));
    practice.removeFromLeft (margin);
    metronomeUnit.setBounds (practice.removeFromRight (376));
    practice.removeFromRight (margin);
    looperUnit.setBounds (practice);

    auto bottom = area.withTrimmedBottom (margin);
    equalizer.setBounds (bottom.removeFromLeft (532));
    bottom.removeFromLeft (margin);
    postBoard.setBounds (bottom);

    looperPanel.setBounds (preBoard.getBounds().getUnion (postBoard.getBounds()).getUnion (equalizer.getBounds()));
}
