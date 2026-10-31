#include "LooperUnit.h"
#include "AmpLookAndFeel.h"
#include "Help.h"

namespace
{
    constexpr int sidePadding = 14;
    constexpr int titleHeight = 22;
    constexpr int gap = 6;
}

LooperUnit::LooperUnit (BassAmpProcessor& processor)
    : looper (processor.looper)
{
    for (int track = 0; track < Looper::numTracks; ++track)
        addAndMakeVisible (pads.add (new LoopPad (looper, track)));

    stopAllButton.setComponentID ("looper_stop_all");
    stopAllButton.onClick = [this] { looper.stopOrPlayAll(); };
    openButton.onClick = [this] { if (onOpen != nullptr) onOpen(); };

    setTooltip (Help::get ("ui.looper"));
    stopAllButton.setTooltip (Help::get ("ui.looperStopAll"));
    openButton.setTooltip (Help::get ("ui.looperOpen"));

    addAndMakeVisible (stopAllButton);
    addAndMakeVisible (openButton);

    startTimerHz (30);
}

LooperUnit::~LooperUnit()
{
    stopTimer();
}

void LooperUnit::timerCallback()
{
    for (auto* pad : pads)
        pad->repaint();
}

void LooperUnit::resized()
{
    auto area = getLocalBounds().reduced (sidePadding, 8).withTrimmedBottom (4);
    titleArea = area.removeFromTop (titleHeight);
    auto row = area.withSizeKeepingCentre (area.getWidth(), juce::jmin (44, area.getHeight() + 4));

    openButton.setBounds (row.removeFromRight (58).withSizeKeepingCentre (58, 26));
    row.removeFromRight (gap);
    stopAllButton.setBounds (row.removeFromRight (72).withSizeKeepingCentre (72, 26));
    row.removeFromRight (gap + 2);

    const auto padSize = juce::jmin (row.getHeight(), (row.getWidth() - (pads.size() - 1) * gap) / pads.size());

    for (auto* pad : pads)
    {
        pad->setBounds (row.removeFromLeft (padSize).withSizeKeepingCentre (padSize, padSize));
        row.removeFromLeft (gap);
    }
}

void LooperUnit::paint (juce::Graphics& g)
{
    Draw::unitChassis (g, getLocalBounds().toFloat());

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (15.0f).withKerningFactor (0.12f));
    g.drawText ("LOOPER", titleArea, juce::Justification::centredLeft);
}
