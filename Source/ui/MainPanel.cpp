#include "MainPanel.h"
#include "AmpLookAndFeel.h"
#include "../ParameterIds.h"

namespace
{
    constexpr int margin = 12;
    constexpr int headerHeight = 56;
    constexpr int boardPadding = 12;
    constexpr int pedalGap = 10;
}

MainPanel::MainPanel (BassAmpProcessor& processorToUse)
    : processor (processorToUse),
      presetBar (processorToUse.presets),
      recorderBar (processorToUse.recorder),
      gate (processorToUse.parameters, "GATE", "Noise gate", Palette::gate,
            { { ParamIds::gateThreshold, "Thresh" }, { ParamIds::gateRelease, "Release" } }, ParamIds::gateOn),
      compressor (processorToUse.parameters, "COMP", "Studio compressor", Palette::compressor,
                  { { ParamIds::compThreshold, "Thresh" }, { ParamIds::compRatio, "Ratio" }, { ParamIds::compAttack, "Attack" },
                    { ParamIds::compRelease, "Release" }, { ParamIds::compMakeup, "Makeup" }, { ParamIds::compMix, "Mix" } },
                  ParamIds::compOn),
      octaver (processorToUse.parameters, "OCTAVER", "Sub and up octave", Palette::octaver,
               { { ParamIds::octSub, "Sub" }, { ParamIds::octUp, "Up" }, { ParamIds::octDry, "Dry" } }, ParamIds::octOn),
      envFilter (processorToUse.parameters, "ENVELOPE", "Auto-wah filter", Palette::envFilter,
                 { { ParamIds::envSensitivity, "Sens" }, { ParamIds::envResonance, "Reso" }, { ParamIds::envMix, "Mix" } },
                 ParamIds::envOn),
      fuzz (processorToUse.parameters, "FUZZ", "Silicon fuzz", Palette::fuzz,
            { { ParamIds::fuzzAmount, "Fuzz" }, { ParamIds::fuzzTone, "Tone" }, { ParamIds::fuzzBlend, "Blend" } }, ParamIds::fuzzOn),
      distortion (processorToUse.parameters, "DRIVE", "Bass overdrive", Palette::distortion,
                  { { ParamIds::distDrive, "Drive" }, { ParamIds::distTone, "Tone" }, { ParamIds::distBlend, "Blend" } },
                  ParamIds::distOn),
      tuner (processorToUse),
      ampHead (processorToUse),
      meters (processorToUse),
      equalizer (processorToUse),
      phaser (processorToUse.parameters, "PHASER", "Four-stage phase", Palette::phaser,
              { { ParamIds::phaserRate, "Rate" }, { ParamIds::phaserDepth, "Depth" }, { ParamIds::phaserFeedback, "Regen" } },
              ParamIds::phaserOn),
      chorus (processorToUse.parameters, "CHORUS", "Bass chorus", Palette::chorus,
              { { ParamIds::chorusRate, "Rate" }, { ParamIds::chorusDepth, "Depth" }, { ParamIds::chorusMix, "Mix" } },
              ParamIds::chorusOn),
      delay (processorToUse.parameters, "DELAY", "Analog echo", Palette::delay,
             { { ParamIds::delayTime, "Time" }, { ParamIds::delayFeedback, "Repeat" }, { ParamIds::delayMix, "Mix" } },
             ParamIds::delayOn),
      reverb (processorToUse.parameters, "REVERB", "Room reverb", Palette::reverb,
              { { ParamIds::reverbSize, "Size" }, { ParamIds::reverbDamping, "Damp" }, { ParamIds::reverbMix, "Mix" } },
              ParamIds::reverbOn)
{
    for (auto* component : std::initializer_list<juce::Component*> {
             &presetBar, &recorderBar, &gate, &compressor, &octaver, &envFilter, &fuzz, &distortion,
             &tuner, &ampHead, &meters, &equalizer, &phaser, &chorus, &delay, &reverb })
        addAndMakeVisible (component);

    shownDeviceInfo = processor.audioDeviceInfo;
    setSize (designWidth, designHeight);
    startTimerHz (2);
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
}

void MainPanel::paintBoard (juce::Graphics& g, juce::Rectangle<float> board)
{
    juce::Path outline;
    outline.addRoundedRectangle (board, 12.0f);
    juce::DropShadow (juce::Colours::black.withAlpha (0.6f), 18, { 0, 6 }).drawForPath (g, outline);

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff1e1f23), 0.0f, board.getY(), juce::Colour (0xff111215), 0.0f, board.getBottom(), false));
    g.fillPath (outline);

    g.saveState();
    g.reduceClipRegion (outline);

    for (int rail = 1; rail <= 3; ++rail)
    {
        const auto y = board.getY() + board.getHeight() * (float) rail / 4.0f;
        g.setColour (juce::Colours::black.withAlpha (0.55f));
        g.fillRect (board.getX(), y - 4.0f, board.getWidth(), 8.0f);
        g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.fillRect (board.getX(), y + 4.0f, board.getWidth(), 1.0f);
    }

    g.restoreState();
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (board.reduced (0.5f), 12.0f, 1.0f);
}

void MainPanel::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff23252b), bounds.getCentreX(), bounds.getY() + bounds.getHeight() * 0.35f,
                                             juce::Colour (0xff08080a), bounds.getX(), bounds.getBottom(), true));
    g.fillRect (bounds);

    paintBoard (g, preBoard.toFloat());
    paintBoard (g, postBoard.toFloat());

    auto header = headerArea.reduced (margin + 8, 0);
    const auto logo = header.removeFromLeft (220).toFloat();
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xfff7dc9c), 0.0f, logo.getY() + 10.0f, juce::Colour (0xffb7832f), 0.0f, logo.getBottom() - 10.0f, false));
    g.setFont (Fonts::display (34.0f).withKerningFactor (0.1f));
    g.drawText ("BASS AMP", logo, juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (Fonts::display (12.0f, false));
    g.drawFittedText (shownDeviceInfo, header.withLeft (recorderBar.getRight() + margin), juce::Justification::centredRight, 1, 0.8f);
}

void MainPanel::layoutPedals (juce::Rectangle<int> area, std::initializer_list<Pedal*> pedals)
{
    auto totalWeight = 0;

    for (auto* pedal : pedals)
        totalWeight += juce::jmax (2, pedal->getNumKnobs()) + 1;

    auto remainingWidth = area.getWidth() - pedalGap * ((int) pedals.size() - 1);
    auto remainingWeight = totalWeight;

    for (auto* pedal : pedals)
    {
        const auto weight = juce::jmax (2, pedal->getNumKnobs()) + 1;
        const auto width = remainingWidth * weight / remainingWeight;
        pedal->setBounds (area.removeFromLeft (width));
        area.removeFromLeft (pedalGap);
        remainingWidth -= width;
        remainingWeight -= weight;
    }
}

void MainPanel::resized()
{
    auto area = getLocalBounds().reduced (margin, 0);
    headerArea = area.removeFromTop (headerHeight);

    presetBar.setBounds (juce::Rectangle<int> (440, 32).withPosition (250, (headerHeight - 32) / 2 + 2));
    recorderBar.setBounds (juce::Rectangle<int> (200, 30).withPosition (presetBar.getRight() + 28, (headerHeight - 30) / 2 + 2));

    area.removeFromTop (6);
    preBoard = area.removeFromTop (236);
    layoutPedals (preBoard.reduced (boardPadding), { &gate, &compressor, &octaver, &envFilter, &fuzz, &distortion });

    area.removeFromTop (margin);
    auto middle = area.removeFromTop (240);
    tuner.setBounds (middle.removeFromLeft (232));
    middle.removeFromLeft (margin);
    meters.setBounds (middle.removeFromRight (232));
    middle.removeFromRight (margin);
    ampHead.setBounds (middle);

    area.removeFromTop (margin);
    auto bottom = area.withTrimmedBottom (margin);
    equalizer.setBounds (bottom.removeFromLeft (532));
    bottom.removeFromLeft (margin);
    postBoard = bottom;
    layoutPedals (postBoard.reduced (boardPadding), { &phaser, &chorus, &delay, &reverb });
}
