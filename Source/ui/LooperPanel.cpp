#include "LooperPanel.h"
#include "AmpLookAndFeel.h"
#include "Help.h"
#include "../i18n/Language.h"

namespace
{
    constexpr int padding = 20;
    constexpr int headerHeight = 32;
    constexpr int gap = 8;
}

LooperPanel::TrackColumn::TrackColumn (Looper& looper, int track)
    : pad (looper, track)
{
    auto& controls = looper.controls (track);

    stopButton.onClick = [&looper, track] { looper.stopTrack (track); };
    clearButton.onClick = [&looper, track] { looper.clearTrack (track); };
    reverseSwitch.setToggleState (controls.reverse.load(), juce::dontSendNotification);
    reverseSwitch.onClick = [this, &controls] { controls.reverse.store (reverseSwitch.getToggleState()); };
    halfSwitch.setToggleState (controls.halfSpeed.load(), juce::dontSendNotification);
    halfSwitch.onClick = [this, &controls] { controls.halfSpeed.store (halfSwitch.getToggleState()); };

    level.setRange (0.0, 1.0);
    level.setValue (controls.level.load(), juce::dontSendNotification);
    level.setDoubleClickReturnValue (true, 1.0);
    level.getProperties().set (KnobStyle::property, KnobStyle::pedal);
    level.onValueChange = [this, &controls] { controls.level.store ((float) level.getValue()); };

    stopButton.setTooltip (Help::get ("ui.looperTrackStop"));
    clearButton.setTooltip (Help::get ("ui.looperTrackClear"));
    trimButton.setTooltip (Help::get ("ui.looperTrim"));
    trimButton.setEnabled (false);
    reverseSwitch.setTooltip (Help::get ("ui.looperReverse"));
    halfSwitch.setTooltip (Help::get ("ui.looperHalf"));
    level.setTooltip (Help::get ("ui.looperLevel"));
}

LooperPanel::LooperPanel (BassAmpProcessor& processor)
    : looper (processor.looper)
{
    for (int track = 0; track < Looper::numTracks; ++track)
    {
        auto* column = columns.add (new TrackColumn (looper, track));
        column->trimButton.onClick = [this, track] { trimView.show (track); };

        for (auto* component : std::initializer_list<juce::Component*> { &column->pad, &column->stopButton, &column->clearButton, &column->trimButton,
                                                                         &column->reverseSwitch, &column->halfSwitch, &column->level })
            addAndMakeVisible (component);
    }

    syncSwitch.setToggleState (looper.sync.load(), juce::dontSendNotification);
    syncSwitch.onClick = [this] { looper.sync.store (syncSwitch.getToggleState()); };
    countInSwitch.setToggleState (looper.countIn.load(), juce::dontSendNotification);
    countInSwitch.onClick = [this] { looper.countIn.store (countInSwitch.getToggleState()); };
    autoSwitch.setToggleState (looper.autoStart.load(), juce::dontSendNotification);
    autoSwitch.onClick = [this] { looper.autoStart.store (autoSwitch.getToggleState()); };
    undoButton.onClick = [this] { looper.undo(); };
    redoButton.onClick = [this] { looper.redo(); };
    stopAllButton.onClick = [this] { looper.stopOrPlayAll(); };
    clearAllButton.onClick = [this] { looper.clearAll(); };
    exportButton.onClick = [this] { exportMix(); };
    closeButton.onClick = [this] { if (onClose != nullptr) onClose(); };

    undoButton.setComponentID ("looper_undo");
    redoButton.setComponentID ("looper_redo");
    stopAllButton.setComponentID ("looper_stop_all");
    clearAllButton.setComponentID ("looper_clear_all");

    syncSwitch.setTooltip (Help::get ("ui.looperSync"));
    countInSwitch.setTooltip (Help::get ("ui.looperCountIn"));
    autoSwitch.setTooltip (Help::get ("ui.looperAuto"));
    undoButton.setTooltip (Help::get ("ui.looperUndo"));
    redoButton.setTooltip (Help::get ("ui.looperRedo"));
    stopAllButton.setTooltip (Help::get ("ui.looperStopAll"));
    clearAllButton.setTooltip (Help::get ("ui.looperClearAll"));
    exportButton.setTooltip (Help::get ("ui.looperExport"));
    closeButton.setTooltip (Help::get ("ui.looperClose"));

    for (auto* component : std::initializer_list<juce::Component*> { &syncSwitch, &countInSwitch, &autoSwitch, &undoButton, &redoButton, &stopAllButton,
                                                                     &clearAllButton, &exportButton, &closeButton })
        addAndMakeVisible (component);

    addChildComponent (trimView);

    startTimerHz (30);
}

LooperPanel::~LooperPanel()
{
    stopTimer();
}

void LooperPanel::timerCallback()
{
    if (! isShowing())
        return;

    undoButton.setEnabled (looper.canUndo());
    redoButton.setEnabled (looper.canRedo());
    exportButton.setEnabled (looper.hasLoops());

    for (int track = 0; track < columns.size(); ++track)
    {
        columns[track]->trimButton.setEnabled (looper.canTrim (track));
        columns[track]->pad.repaint();
    }

    repaint (statusArea);

    for (auto* column : columns)
        repaint (column->labelArea);
}

void LooperPanel::exportMix()
{
    const auto file = Recorder::getDefaultDirectory()
                          .getChildFile ("Loop " + juce::Time::getCurrentTime().formatted ("%Y-%m-%d %H-%M-%S") + ".wav");

    if (file.getParentDirectory().createDirectory() && looper.exportMix (file))
    {
        exportMessage = Language::tr ("Saved") + " " + file.getFileName();
        file.revealToUser();
    }
    else
    {
        exportMessage = Language::tr ("Export failed");
    }
}

void LooperPanel::resized()
{
    auto area = getLocalBounds().reduced (padding, padding - 6);
    auto header = area.removeFromTop (headerHeight);

    closeButton.setBounds (header.removeFromRight (72).withSizeKeepingCentre (72, 26));
    header.removeFromRight (16);
    exportButton.setBounds (header.removeFromRight (104).withSizeKeepingCentre (104, 26));
    header.removeFromRight (16);
    clearAllButton.setBounds (header.removeFromRight (88).withSizeKeepingCentre (88, 26));
    header.removeFromRight (gap);
    stopAllButton.setBounds (header.removeFromRight (84).withSizeKeepingCentre (84, 26));
    header.removeFromRight (16);
    redoButton.setBounds (header.removeFromRight (64).withSizeKeepingCentre (64, 26));
    header.removeFromRight (gap);
    undoButton.setBounds (header.removeFromRight (64).withSizeKeepingCentre (64, 26));
    header.removeFromRight (16);
    syncSwitch.setBounds (header.removeFromRight (72).withSizeKeepingCentre (72, 24));
    header.removeFromRight (gap);
    countInSwitch.setBounds (header.removeFromRight (104).withSizeKeepingCentre (104, 24));
    header.removeFromRight (gap);
    autoSwitch.setBounds (header.removeFromRight (72).withSizeKeepingCentre (72, 24));
    titleArea = header;

    area.removeFromTop (4);
    statusArea = area.removeFromTop (20);
    area.removeFromTop (gap);

    trimView.setBounds (area);
    const auto columnWidth = area.getWidth() / columns.size();

    for (auto* column : columns)
    {
        auto cell = area.removeFromLeft (columnWidth).reduced (gap, 0);
        auto controls = cell.removeFromBottom (92);
        column->labelArea = cell.removeFromTop (22);

        const auto padSize = juce::jmin (cell.getWidth() - 20, cell.getHeight() - 8);
        column->pad.setBounds (cell.withSizeKeepingCentre (padSize, padSize));

        auto knob = controls.removeFromRight (64);
        column->levelLabelArea = knob.removeFromTop (14);
        column->level.setBounds (knob.withSizeKeepingCentre (52, 52));
        controls.removeFromRight (gap);

        auto top = controls.removeFromTop (controls.getHeight() / 2).withSizeKeepingCentre (controls.getWidth(), 28);
        auto bottom = controls.withSizeKeepingCentre (controls.getWidth(), 28);
        const auto half = (top.getWidth() - gap) / 2;
        const auto third = (top.getWidth() - 2 * gap) / 3;
        column->stopButton.setBounds (top.removeFromLeft (third));
        column->trimButton.setBounds (top.removeFromRight (third));
        column->clearButton.setBounds (top.withSizeKeepingCentre (third, top.getHeight()));
        column->reverseSwitch.setBounds (bottom.removeFromLeft (half).withSizeKeepingCentre (half, 24));
        column->halfSwitch.setBounds (bottom.removeFromRight (half).withSizeKeepingCentre (half, 24));
    }
}

void LooperPanel::paint (juce::Graphics& g)
{
    Draw::unitChassis (g, getLocalBounds().toFloat());

    g.setColour (Palette::gold);
    g.setFont (Fonts::display (22.0f).withKerningFactor (0.12f));
    g.drawText ("LOOPER", titleArea, juce::Justification::centredLeft);

    const auto status = looper.isSynced()
                            ? (looper.countIn.load() ? Language::tr ("Synced to the metronome: a bar of count-in, then loops start and end on a bar.")
                                                     : Language::tr ("Synced to the metronome: loops start and end on a bar."))
                            : (looper.autoStart.load() ? Language::tr ("Free: recording starts on your first note; the first loop sets the length the others follow.")
                                                       : Language::tr ("Free: the first loop sets the length the others follow."));
    g.setColour (Palette::textDim);
    g.setFont (Fonts::label (12.0f));
    g.drawText (status, statusArea, juce::Justification::centredLeft);
    g.setColour (Palette::gold);
    g.drawText (exportMessage, statusArea, juce::Justification::centredRight);

    for (int track = 0; track < columns.size(); ++track)
    {
        const auto* column = columns[track];
        const auto seconds = looper.getLengthSeconds (track);

        g.setColour (Palette::text);
        g.setFont (Fonts::display (14.0f));
        g.drawText ("TRACK " + juce::String (track + 1), column->labelArea, juce::Justification::centredLeft);
        g.setColour (Palette::textDim);
        g.drawText (seconds > 0.0 ? juce::String (seconds, 1) + " s" : juce::String(), column->labelArea, juce::Justification::centredRight);

        g.setFont (Fonts::label (10.0f));
        g.drawText ("LEVEL", column->levelLabelArea, juce::Justification::centredBottom);
    }
}
