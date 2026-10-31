#include "AmpHead.h"
#include "AmpLookAndFeel.h"
#include "AmpSkins.h"
#include "Help.h"
#include "ModulePresetMenu.h"
#include "../dsp/AmpModels.h"
#include "../i18n/Language.h"

static_assert (std::tuple_size_v<decltype (ampSkins)> == std::tuple_size_v<decltype (ampModels)>, "one skin per amp model");

namespace
{
    constexpr float frameCorner = 14.0f;
    constexpr int border = 12;
    constexpr int topHeight = 84;
    constexpr int labelHeight = 14;
    constexpr int knobSize = 58;
    constexpr int cutKnobSize = 36;
    constexpr int panelKnobs = 8;
    constexpr int midFreqKnob = 3;

    const auto& skinFor (int model) { return ampSkins[(size_t) juce::jlimit (0, (int) ampSkins.size() - 1, model)]; }

    juce::Colour coveringInk (const AmpSkin& skin)
    {
        return skin.cabinet.getPerceivedBrightness() > 0.5f ? juce::Colour (0xff2a1c10) : juce::Colours::white.withAlpha (0.7f);
    }
}

AmpHead::AmpHead (BassAmpProcessor& processorToUse)
    : processor (processorToUse)
{
    auto& state = processor.parameters;
    modelParam = state.getRawParameterValue (ParamIds::ampModel);
    cabParam = state.getRawParameterValue (ParamIds::ampCab);

    const std::pair<const char*, const char*> knobSpecs[] {
        { ParamIds::ampGain, "GAIN" }, { ParamIds::ampBass, "BASS" }, { ParamIds::ampMid, "MIDDLE" },
        { ParamIds::ampMidFreq, "FREQ" }, { ParamIds::ampTreble, "TREBLE" }, { ParamIds::ampContour, "CONTOUR" },
        { ParamIds::ampBlend, "DI BLEND" }, { ParamIds::ampMaster, "MASTER" },
        { ParamIds::ampLowCut, "LO CUT" }, { ParamIds::ampHighCut, "HI CUT" },
    };

    for (const auto& [paramId, label] : knobSpecs)
    {
        Knob knob;
        knob.slider = std::make_unique<juce::Slider> (juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox);
        knob.slider->setPopupDisplayEnabled (true, true, nullptr);
        knob.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, paramId, *knob.slider);
        knob.slider->setComponentID (paramId);

        if (auto* parameter = state.getParameter (paramId))
            knob.slider->setDoubleClickReturnValue (true, parameter->convertFrom0to1 (parameter->getDefaultValue()));

        knob.label = label;
        addAndMakeVisible (*knob.slider);
        knobs.push_back (std::move (knob));
    }

    for (const auto& [paramId, label] : { std::pair { ParamIds::ampDeep, "DEEP" }, std::pair { ParamIds::ampBright, "BRIGHT" } })
    {
        Switch toggle;
        toggle.button = std::make_unique<ToggleSwitch>();
        toggle.button->setComponentID (paramId);
        toggle.button->getProperties().set (ToggleStyle::property, ToggleStyle::batSwitch);
        toggle.attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (state, paramId, *toggle.button);
        toggle.label = label;
        addAndMakeVisible (*toggle.button);
        switches.push_back (std::move (toggle));
    }

    using ChoiceBox = std::pair<juce::ComboBox*, const char*>;

    for (const auto& [box, paramId] : { ChoiceBox { &modelBox, ParamIds::ampModel }, ChoiceBox { &cabBox, ParamIds::ampCab } })
        if (auto* parameter = dynamic_cast<juce::AudioParameterChoice*> (state.getParameter (paramId)))
            box->addItemList (parameter->choices, 1);

    modelBox.getProperties().set ("comboStyle", "badge");
    modelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (state, ParamIds::ampModel, modelBox);
    cabAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (state, ParamIds::ampCab, cabBox);
    addAndMakeVisible (modelBox);
    addAndMakeVisible (cabBox);

    irButton.onClick = [this] { chooseImpulseResponse(); };
    irButton.setTooltip (Help::get ("ui.loadIr"));
    addAndMakeVisible (irButton);

    presetsButton.onClick = [this] { showModulePresetMenu (processor.parameters, "amp", presetsButton); };
    presetsButton.setTooltip (Help::get ("ui.ampPresets"));
    addAndMakeVisible (presetsButton);

    captureButton.onClick = [this] { chooseCapture(); };
    captureButton.setTooltip (Help::get ("ui.loadNam"));
    addChildComponent (captureButton);

    applySkin();
    startTimerHz (30);
}

AmpHead::~AmpHead()
{
    stopTimer();
}

void AmpHead::applySkin()
{
    currentModel = juce::roundToInt (modelParam->load());
    const auto& skin = skinFor (currentModel);

    for (auto& knob : knobs)
    {
        knob.slider->getProperties().set (KnobStyle::property, skin.knobStyle);
        knob.slider->setColour (juce::Slider::thumbColourId, skin.knob);
        knob.slider->setColour (juce::Slider::rotarySliderFillColourId, skin.pointer);
    }

    const auto passive = ampModels[(size_t) juce::jlimit (0, (int) ampModels.size() - 1, currentModel)].passiveToneStack.has_value();
    knobs[midFreqKnob].slider->setEnabled (! passive);
    knobs[midFreqKnob].slider->setAlpha (passive ? 0.3f : 1.0f);

    captureButton.setVisible (showsCapture());
    modelBox.setTooltip (Help::get (juce::String ("model.") + ampModels[(size_t) juce::jlimit (0, (int) ampModels.size() - 1, currentModel)].name));
    shownCapture = captureText();
    modelBox.setColour (juce::ComboBox::textColourId, skin.accent.brighter (0.2f));
    modelBox.setColour (juce::ComboBox::arrowColourId, skin.accent);

    staticLayer = {};
    repaint();
}

void AmpHead::timerCallback()
{
    if (juce::roundToInt (modelParam->load()) != currentModel)
        applySkin();

    const auto cab = juce::roundToInt (cabParam->load());
    const auto irText = impulseResponseText();

    if (cab != currentCab || irText != shownImpulseResponse)
    {
        currentCab = cab;
        shownImpulseResponse = irText;
        cabBox.setTooltip (Help::get (juce::String ("cab.") + cabModels[(size_t) juce::jlimit (0, (int) cabModels.size() - 1, cab)].name));
        repaint (cabArea);
    }

    if (const auto text = captureText(); text != shownCapture)
    {
        shownCapture = text;
        repaint (ventArea);
    }

    const auto target = juce::jlimit (0.0f, 1.0f, processor.ampMeter.takePeak() * 1.8f);
    const auto previous = glow;
    glow += (target > glow ? 0.5f : 0.06f) * (target - glow);

    if (std::abs (glow - previous) > 0.003f)
        repaint (ventArea);
}

juce::String AmpHead::impulseResponseText() const
{
    if (! cabModels[(size_t) juce::jlimit (0, (int) cabModels.size() - 1, currentCab)].usesImpulseResponse)
        return {};

    const auto file = processor.getCabinetImpulseResponse();
    return file.existsAsFile() ? file.getFileNameWithoutExtension() : Language::tr ("no file loaded");
}

bool AmpHead::showsCapture() const
{
    return ampModels[(size_t) juce::jlimit (0, (int) ampModels.size() - 1, currentModel)].neuralCapture;
}

juce::String AmpHead::captureText() const
{
    if (! showsCapture())
        return {};

    const auto file = processor.getCapture();

    if (! file.existsAsFile())
        return Language::tr ("No capture loaded");

    const auto capturedRate = processor.getCaptureSampleRate();

    if (juce::exactlyEqual (capturedRate, 0.0))
        return file.getFileNameWithoutExtension() + "\n" + Language::tr ("Can't read this file");

    const auto playingRate = processor.getSampleRate();

    if (capturedRate > 0.0 && playingRate > 0.0 && std::abs (capturedRate - playingRate) > 1.0)
        return file.getFileNameWithoutExtension() + "\n" + Language::tr ("Set audio to") + " " + juce::String (capturedRate / 1000.0, 1) + " kHz";

    return file.getFileNameWithoutExtension();
}

void AmpHead::chooseCapture()
{
    const auto current = processor.getCapture();
    fileChooser = std::make_unique<juce::FileChooser> (Language::tr ("Load a Neural Amp Modeler capture"),
                                                       current.existsAsFile() ? current.getParentDirectory() : juce::File(),
                                                       "*.nam");

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [safeThis = SafePointer<AmpHead> (this)] (const juce::FileChooser& chooser)
    {
        if (safeThis != nullptr && chooser.getResult().existsAsFile())
            safeThis->processor.setCapture (chooser.getResult());
    });
}

void AmpHead::chooseImpulseResponse()
{
    const auto current = processor.getCabinetImpulseResponse();
    fileChooser = std::make_unique<juce::FileChooser> (Language::tr ("Load a cabinet impulse response"),
                                                       current.existsAsFile() ? current.getParentDirectory() : juce::File(),
                                                       "*.wav;*.aif;*.aiff;*.flac");

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [safeThis = SafePointer<AmpHead> (this)] (const juce::FileChooser& chooser)
    {
        const auto file = chooser.getResult();

        if (safeThis == nullptr || ! file.existsAsFile())
            return;

        safeThis->processor.setCabinetImpulseResponse (file);

        if (auto* cab = safeThis->processor.parameters.getParameter (ParamIds::ampCab))
            cab->setValueNotifyingHost (cab->convertTo0to1 ((float) cabModels.size() - 1.0f));
    });
}

void AmpHead::resized()
{
    frameArea = getLocalBounds().reduced (2, 2).withTrimmedBottom (4);
    auto inner = frameArea.reduced (border);

    topArea = inner.removeFromTop (topHeight);
    auto top = topArea;
    badgeArea = top.removeFromLeft (250);
    top.removeFromLeft (14);
    cabArea = top.removeFromRight (200);
    top.removeFromRight (14);
    auto cuts = top.removeFromRight (110);
    top.removeFromRight (10);

    for (auto knob = (size_t) panelKnobs; knob < knobs.size(); ++knob)
    {
        auto cell = cuts.removeFromLeft (55);
        cell.removeFromTop (labelHeight + 2);
        knobs[knob].slider->setBounds (cell.withSizeKeepingCentre (cutKnobSize + 10, cutKnobSize + 10));
    }

    ventArea = top.reduced (0, 4);
    captureButton.setBounds (ventArea.reduced (8, 6).removeFromBottom (22).removeFromRight (72));

    modelBox.setBounds (badgeArea.withTrimmedTop (24).reduced (10, 6));

    auto cab = cabArea.reduced (0, 4);
    cab.removeFromTop (labelHeight);
    auto cabRow = cab.removeFromTop (28);
    irButton.setBounds (cabRow.removeFromRight (70));
    cabRow.removeFromRight (6);
    cabBox.setBounds (cabRow);
    cab.removeFromTop (6);
    presetsButton.setBounds (cab.removeFromTop (24));

    inner.removeFromTop (10);
    panelArea = inner;

    auto controls = panelArea.reduced (14, 8);
    pilotArea = controls.removeFromRight (56);
    controls.removeFromRight (6);

    // Gain | Bass Middle Freq Treble | Contour | Deep Bright | Blend Master
    const auto groupGap = 14;
    const auto switchWidth = 100;
    const auto cellWidth = (controls.getWidth() - switchWidth - 4 * groupGap) / panelKnobs;

    auto placeKnob = [&controls, cellWidth] (Knob& knob)
    {
        auto cell = controls.removeFromLeft (cellWidth);
        cell.removeFromTop (labelHeight + 2);
        knob.slider->setBounds (cell.withSizeKeepingCentre (knobSize + 10, knobSize + 10));
    };

    placeKnob (knobs[0]);
    controls.removeFromLeft (groupGap);

    for (size_t i = 1; i <= 4; ++i)
        placeKnob (knobs[i]);

    controls.removeFromLeft (groupGap);
    placeKnob (knobs[5]);
    controls.removeFromLeft (groupGap);

    switchArea = controls.removeFromLeft (switchWidth);
    auto switchCells = switchArea;
    switchCells.removeFromTop (labelHeight + 2);

    for (auto& toggle : switches)
    {
        auto cell = switchCells.removeFromLeft (switchWidth / 2);
        toggle.button->setBounds (cell.withSizeKeepingCentre (30, 40));
    }

    controls.removeFromLeft (groupGap);
    placeKnob (knobs[6]);
    placeKnob (knobs[7]);

    staticLayer = {};
}

void AmpHead::paint (juce::Graphics& g)
{
    const auto scale = g.getInternalContext().getPhysicalPixelScaleFactor();

    if (staticLayer.isNull() || ! juce::approximatelyEqual (scale, staticLayerScale))
    {
        staticLayerScale = scale;
        staticLayer = juce::Image (juce::Image::ARGB, juce::jmax (1, juce::roundToInt ((float) getWidth() * scale)),
                                   juce::jmax (1, juce::roundToInt ((float) getHeight() * scale)), true);
        juce::Graphics layer (staticLayer);
        layer.addTransform (juce::AffineTransform::scale (scale));
        paintStatic (layer);
    }

    g.drawImage (staticLayer, getLocalBounds().toFloat());
    paintVent (g);

    if (shownImpulseResponse.isNotEmpty())
    {
        const auto& skin = skinFor (currentModel);
        g.setColour (coveringInk (skin));
        g.setFont (Fonts::display (12.0f, false));
        g.drawFittedText ("IR: " + shownImpulseResponse, cabArea.withTop (cabBox.getBottom() + 4).withHeight (18),
                          juce::Justification::centredLeft, 1, 0.8f);
    }
}

void AmpHead::paintStatic (juce::Graphics& g)
{
    const auto& skin = skinFor (currentModel);
    const auto frame = frameArea.toFloat();

    juce::Path outline;
    outline.addRoundedRectangle (frame, frameCorner);
    juce::DropShadow (juce::Colours::black.withAlpha (0.8f), 22, { 0, 8 }).drawForPath (g, outline);

    paintCovering (g, frame);

    // Chrome corner protectors.
    if (skin.covering != AmpSkin::Covering::metal)
    {
        for (const auto corner : { frame.getTopLeft(), frame.getTopRight(), frame.getBottomLeft(), frame.getBottomRight() })
        {
            const auto c = corner.translated (corner.x < frame.getCentreX() ? 9.0f : -9.0f, corner.y < frame.getCentreY() ? 9.0f : -9.0f);
            g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff1f2f4), c.x - 8.0f, c.y - 8.0f,
                                                     juce::Colour (0xff5d6068), c.x + 8.0f, c.y + 8.0f, false));
            g.fillEllipse (c.x - 9.0f, c.y - 9.0f, 18.0f, 18.0f);
            g.setColour (juce::Colours::black.withAlpha (0.5f));
            g.drawEllipse (c.x - 9.0f, c.y - 9.0f, 18.0f, 18.0f, 1.0f);
        }
    }

    // Badge plate holding the model selector.
    const auto badge = badgeArea.toFloat().reduced (2.0f, 4.0f);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillRoundedRectangle (badge.translated (0.0f, 2.0f), 8.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff1d1e22), 0.0f, badge.getY(), juce::Colour (0xff08080a), 0.0f, badge.getBottom(), false));
    g.fillRoundedRectangle (badge, 8.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffe9ebee), badge.getX(), badge.getY(),
                                             juce::Colour (0xff6f737b), badge.getX(), badge.getBottom(), false));
    g.drawRoundedRectangle (badge, 8.0f, 2.0f);
    g.setColour (juce::Colours::white.withAlpha (0.55f));
    g.setFont (Fonts::label (11.0f).withKerningFactor (0.35f));
    g.drawText ("BASS  AMP", badge.withHeight (26.0f).translated (0.0f, 4.0f), juce::Justification::centred);

    // Vent recess.
    const auto vent = ventArea.toFloat();
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillRoundedRectangle (vent.expanded (3.0f), 7.0f);
    g.setColour (juce::Colours::white.withAlpha (0.12f));
    g.drawRoundedRectangle (vent.expanded (3.0f), 7.0f, 1.0f);

    g.setColour (coveringInk (skin));
    g.setFont (Fonts::label (11.0f));
    g.drawText ("CABINET", cabArea.withTrimmedTop (4).withHeight (labelHeight), juce::Justification::centredLeft);

    paintPanel (g);
}

void AmpHead::paintCovering (juce::Graphics& g, juce::Rectangle<float> frame)
{
    const auto& skin = skinFor (currentModel);
    juce::Path outline;
    outline.addRoundedRectangle (frame, frameCorner);

    g.saveState();
    g.reduceClipRegion (outline);
    g.setGradientFill (juce::ColourGradient (skin.cabinet.brighter (0.15f), 0.0f, frame.getY(),
                                             skin.cabinet.darker (0.3f), 0.0f, frame.getBottom(), false));
    g.fillRect (frame);

    juce::Random random (11);

    switch (skin.covering)
    {
        case AmpSkin::Covering::tolex:
            for (int i = 0; i < 6000; ++i)
            {
                g.setColour ((random.nextBool() ? juce::Colours::white : juce::Colours::black).withAlpha (0.06f));
                g.fillRect (frame.getX() + random.nextFloat() * frame.getWidth(), frame.getY() + random.nextFloat() * frame.getHeight(), 1.2f, 1.2f);
            }
            break;

        case AmpSkin::Covering::tweed:
            for (auto x = frame.getX() - frame.getHeight(); x < frame.getRight(); x += 3.0f)
            {
                const auto dark = juce::roundToInt (x) % 2 == 0;
                g.setColour (dark ? juce::Colour (0xff4a3215).withAlpha (0.28f) : juce::Colours::white.withAlpha (0.1f));
                g.drawLine (x, frame.getY(), x + frame.getHeight(), frame.getBottom(), 1.3f);
            }

            for (auto x = frame.getX(); x < frame.getRight() + frame.getHeight(); x += 6.0f)
            {
                g.setColour (juce::Colour (0xff4a3215).withAlpha (0.08f));
                g.drawLine (x, frame.getY(), x - frame.getHeight(), frame.getBottom(), 1.0f);
            }

            g.setGradientFill (juce::ColourGradient (juce::Colour (0x30ffb040), 0.0f, frame.getY(),
                                                     juce::Colour (0x50321e08), 0.0f, frame.getBottom(), false));
            g.fillRect (frame);
            break;

        case AmpSkin::Covering::check:
            for (auto y = frame.getY(); y < frame.getBottom(); y += 8.0f)
            {
                for (auto x = frame.getX() + (juce::roundToInt ((y - frame.getY()) / 8.0f) % 2) * 4.0f; x < frame.getRight(); x += 8.0f)
                {
                    juce::Path diamond;
                    diamond.addQuadrilateral (x, y - 3.0f, x + 3.0f, y, x, y + 3.0f, x - 3.0f, y);
                    g.setColour (juce::Colour (0xff9cc4ff).withAlpha (0.16f));
                    g.fillPath (diamond);
                }
            }
            break;

        case AmpSkin::Covering::metal:
            for (int i = 0; i < 260; ++i)
            {
                const auto radius = 2.0f + random.nextFloat() * 6.0f;
                g.setColour ((random.nextBool() ? juce::Colours::white : juce::Colours::black).withAlpha (0.035f));
                g.fillEllipse (frame.getX() + random.nextFloat() * frame.getWidth(), frame.getY() + random.nextFloat() * frame.getHeight(),
                               radius, radius);
            }
            break;
    }

    g.setGradientFill (juce::ColourGradient (juce::Colours::white.withAlpha (0.1f), 0.0f, frame.getY(),
                                             juce::Colours::transparentWhite, 0.0f, frame.getY() + 30.0f, false));
    g.fillRect (frame);
    g.restoreState();

    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.strokePath (outline, juce::PathStrokeType (1.5f));
}

void AmpHead::paintPanel (juce::Graphics& g)
{
    const auto& skin = skinFor (currentModel);
    const auto panel = panelArea.toFloat();

    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillRoundedRectangle (panel.expanded (2.0f).translated (0.0f, 1.0f), 7.0f);

    if (skin.brushed)
    {
        Draw::brushedMetal (g, panel, skin.panel, 6.0f);
    }
    else
    {
        g.setGradientFill (juce::ColourGradient (skin.panel.brighter (0.1f), 0.0f, panel.getY(), skin.panel.darker (0.2f), 0.0f, panel.getBottom(), false));
        g.fillRoundedRectangle (panel, 6.0f);
    }

    g.setColour (juce::Colours::white.withAlpha (0.18f));
    g.drawLine (panel.getX() + 6.0f, panel.getY() + 0.5f, panel.getRight() - 6.0f, panel.getY() + 0.5f, 1.0f);

    for (const auto corner : { panel.getTopLeft().translated (8.0f, 8.0f), panel.getTopRight().translated (-8.0f, 8.0f),
                               panel.getBottomLeft().translated (8.0f, -8.0f), panel.getBottomRight().translated (-8.0f, -8.0f) })
        Draw::screw (g, corner, 3.5f);

    g.setFont (Fonts::label (11.5f));

    for (const auto& knob : knobs)
    {
        const auto bounds = knob.slider->getBounds().toFloat();
        const auto alpha = knob.slider->isEnabled() ? 1.0f : 0.35f;
        const auto ink = bounds.getY() < (float) panelArea.getY() ? coveringInk (skin) : skin.ink;
        g.setColour (ink.withMultipliedAlpha (alpha));
        g.drawText (knob.label, bounds.withY (bounds.getY() - labelHeight - 1.0f).withHeight ((float) labelHeight).expanded (12.0f, 0.0f),
                    juce::Justification::centred);

        if (juce::String (skin.knobStyle) == KnobStyle::modern)
            continue;

        const auto rotary = knob.slider->getRotaryParameters();
        const auto centre = bounds.getCentre();
        const auto radius = bounds.getWidth() * 0.5f - 1.0f;

        for (int tick = 0; tick <= 10; ++tick)
        {
            const auto angle = rotary.startAngleRadians + (rotary.endAngleRadians - rotary.startAngleRadians) * (float) tick / 10.0f;
            const auto dot = centre.getPointOnCircumference (radius, angle);
            const auto size = (tick == 0 || tick == 5 || tick == 10) ? 2.6f : 1.6f;
            g.setColour (ink.withMultipliedAlpha (0.75f * alpha));
            g.fillEllipse (dot.x - size / 2.0f, dot.y - size / 2.0f, size, size);
        }
    }

    for (const auto& toggle : switches)
    {
        const auto bounds = toggle.button->getBounds().toFloat();
        g.setColour (skin.ink);
        g.drawText (toggle.label, bounds.withY (bounds.getY() - labelHeight - 1.0f).withHeight ((float) labelHeight).expanded (10.0f, 0.0f),
                    juce::Justification::centred);
    }

    const auto pilot = pilotArea.toFloat();
    const auto lamp = juce::Point<float> (pilot.getCentreX(), pilot.getCentreY() + 2.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xffe9ebee), lamp.x - 12.0f, lamp.y - 12.0f,
                                             juce::Colour (0xff4d5057), lamp.x + 12.0f, lamp.y + 12.0f, false));
    g.fillEllipse (lamp.x - 12.0f, lamp.y - 12.0f, 24.0f, 24.0f);
    Draw::led (g, lamp, 8.0f, skin.accent, true);
    g.setColour (skin.ink);
    g.drawText ("POWER", pilot.withY (lamp.y - 36.0f).withHeight ((float) labelHeight), juce::Justification::centred);
}

void AmpHead::paintVent (juce::Graphics& g)
{
    const auto& skin = skinFor (currentModel);
    const auto vent = ventArea.toFloat();

    g.saveState();
    juce::Path clip;
    clip.addRoundedRectangle (vent, 5.0f);
    g.reduceClipRegion (clip);

    g.setColour (juce::Colour (0xff050506));
    g.fillRect (vent);

    if (showsCapture())
    {
        g.setColour (skin.accent.withAlpha (0.06f));
        g.fillRect (vent);

        auto text = vent.reduced (10.0f, 6.0f);
        g.setColour (Palette::textDim);
        g.setFont (Fonts::label (10.0f).withKerningFactor (0.2f));
        g.drawText ("CAPTURE", text.removeFromTop (14.0f), juce::Justification::centredLeft);

        const auto lines = juce::StringArray::fromLines (shownCapture);
        g.setColour (skin.accent);
        g.setFont (Fonts::display (15.0f));
        g.drawFittedText (lines[0], text.removeFromTop (22.0f).toNearestInt(), juce::Justification::centredLeft, 1, 0.7f);

        g.setColour (Palette::ledYellow);
        g.setFont (Fonts::display (10.0f, false));
        g.drawFittedText (lines[1], text.withRight ((float) captureButton.getX() - 6.0f).toNearestInt(),
                          juce::Justification::centredLeft, 2, 0.8f);
    }
    else if (skin.tubes)
    {
        paintTubes (g, vent);
    }
    else
    {
        paintGrille (g, vent);
    }

    g.setGradientFill (juce::ColourGradient (juce::Colours::black.withAlpha (0.8f), 0.0f, vent.getY(),
                                             juce::Colours::transparentBlack, 0.0f, vent.getY() + 14.0f, false));
    g.fillRect (vent);
    g.restoreState();
}

void AmpHead::paintTubes (juce::Graphics& g, juce::Rectangle<float> vent)
{
    const juce::Colour filament { 0xffff7a1e };
    const auto intensity = 0.35f + 0.65f * glow;

    g.setGradientFill (juce::ColourGradient (filament.withAlpha (0.18f + 0.3f * glow), vent.getCentreX(), vent.getBottom(),
                                             juce::Colours::transparentBlack, vent.getCentreX(), vent.getY() - vent.getHeight() * 0.3f, true));
    g.fillRect (vent);

    constexpr int numTubes = 6;
    const auto spacing = vent.getWidth() / (float) numTubes;
    const auto tubeWidth = juce::jmin (spacing * 0.62f, 30.0f);

    for (int i = 0; i < numTubes; ++i)
    {
        const auto centreX = vent.getX() + spacing * ((float) i + 0.5f);
        const auto glass = juce::Rectangle<float> (tubeWidth, vent.getHeight() * 0.9f).withCentre ({ centreX, vent.getCentreY() + vent.getHeight() * 0.08f });
        const auto tubeGlow = juce::jlimit (0.0f, 1.0f, intensity * (0.85f + 0.15f * std::sin ((float) i * 1.7f)));

        juce::Path envelope;
        envelope.addRoundedRectangle (glass.getX(), glass.getY(), glass.getWidth(), glass.getHeight(), tubeWidth * 0.48f, tubeWidth * 0.48f,
                                      true, true, false, false);

        const auto plate = glass.withSizeKeepingCentre (tubeWidth * 0.55f, glass.getHeight() * 0.42f).translated (0.0f, -glass.getHeight() * 0.04f);
        g.setGradientFill (juce::ColourGradient (filament.withAlpha (0.9f * tubeGlow), plate.getCentreX(), plate.getCentreY(),
                                                 filament.withAlpha (0.0f), plate.getCentreX() + tubeWidth * 1.3f, plate.getCentreY(), true));
        g.fillEllipse (plate.expanded (tubeWidth * 0.9f, tubeWidth * 0.5f));

        g.setColour (juce::Colour (0xff2b2b2e));
        g.fillRect (plate);
        g.setColour (juce::Colour (0xff4a4a50));
        g.drawRect (plate, 1.0f);

        g.setColour (juce::Colour (0xffffd08a).withAlpha (tubeGlow));
        g.fillRect (juce::Rectangle<float> (2.0f, plate.getHeight() * 0.8f).withCentre (plate.getCentre()));

        g.setColour (juce::Colour (0xffc8ccd4).withAlpha (0.45f));
        g.fillEllipse (glass.getCentreX() - tubeWidth * 0.25f, glass.getY() + 3.0f, tubeWidth * 0.5f, tubeWidth * 0.22f);

        g.setGradientFill (juce::ColourGradient (juce::Colours::white.withAlpha (0.16f), glass.getX(), 0.0f,
                                                 juce::Colours::white.withAlpha (0.02f), glass.getRight(), 0.0f, false));
        g.fillPath (envelope);
        g.setColour (juce::Colours::white.withAlpha (0.22f));
        g.strokePath (envelope, juce::PathStrokeType (1.0f));
        g.setColour (juce::Colours::white.withAlpha (0.3f));
        g.fillRect (glass.getX() + tubeWidth * 0.18f, glass.getY() + tubeWidth * 0.5f, 1.5f, glass.getHeight() * 0.55f);
    }

    g.setColour (juce::Colour (0xff9a9ea6).withAlpha (0.35f));

    for (auto x = vent.getX() + 6.0f; x < vent.getRight(); x += 12.0f)
        g.fillRect (x, vent.getY(), 1.5f, vent.getHeight());
}

void AmpHead::paintGrille (juce::Graphics& g, juce::Rectangle<float> vent)
{
    const auto& skin = skinFor (currentModel);
    const auto light = 0.25f + 0.75f * glow;

    g.setGradientFill (juce::ColourGradient (skin.accent.withAlpha (0.55f * light), vent.getCentreX(), vent.getCentreY(),
                                             juce::Colours::transparentBlack, vent.getX(), vent.getCentreY(), true));
    g.fillRect (vent);

    g.setColour (skin.accent.withAlpha (0.9f * light));
    g.fillRect (vent.withSizeKeepingCentre (vent.getWidth() * 0.8f, 2.0f));

    g.setColour (juce::Colour (0xff17181b));

    for (auto y = vent.getY(); y < vent.getBottom(); y += 6.0f)
    {
        g.fillRect (vent.getX(), y, vent.getWidth(), 2.0f);

        for (auto x = vent.getX() + (std::fmod (y, 12.0f) < 6.0f ? 0.0f : 3.0f); x < vent.getRight(); x += 6.0f)
            g.fillRect (x, y + 2.0f, 2.0f, 4.0f);
    }
}
