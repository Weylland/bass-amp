#include "PedalBoard.h"
#include "AmpLookAndFeel.h"

namespace
{
    constexpr int padding = 12;
    constexpr int pedalGap = 10;
    constexpr int pedalChrome = 26;      // enclosure padding around the knob row
    constexpr int minKnobCell = 40;
    constexpr int maxKnobCell = 64;
    constexpr int addSlotWidth = 46;
    constexpr int scrollBarThickness = 8;
    constexpr int dragThreshold = 6;

    struct PedalSpec
    {
        PedalId id;
        const char* name;
        const char* subtitle;
        juce::Colour colour;
        std::vector<KnobSpec> knobs;
    };

    std::vector<PedalSpec> pedalSpecs()
    {
        using namespace ParamIds;

        return {
            { PedalId::gate, "GATE", "Noise gate", Palette::gate, { { gateThreshold, "Thresh" }, { gateRelease, "Release" } } },
            { PedalId::compressor, "COMP", "Studio compressor", Palette::compressor,
              { { compThreshold, "Thresh" }, { compRatio, "Ratio" }, { compAttack, "Attack" }, { compRelease, "Release" },
                { compMakeup, "Makeup" }, { compMix, "Mix" } } },
            { PedalId::octaver, "OCTAVER", "Sub and up octave", Palette::octaver, { { octSub, "Sub" }, { octUp, "Up" }, { octDry, "Dry" } } },
            { PedalId::synth, "SYNTH", "Analog bass synth", Palette::synth,
              { { synthWave, "Wave" }, { synthCutoff, "Cutoff" }, { synthResonance, "Reso" }, { synthEnvelope, "Env" }, { synthMix, "Mix" } } },
            { PedalId::pitch, "PITCH", "Pitch shifter", Palette::pitch, { { pitchShift, "Shift" }, { pitchDetune, "Detune" }, { pitchMix, "Mix" } } },
            { PedalId::filter, "FILTER", "Resonant filter", Palette::filter,
              { { filterType, "Type" }, { filterCutoff, "Cutoff" }, { filterResonance, "Reso" }, { filterRate, "Rate" }, { filterDepth, "Depth" } } },
            { PedalId::envelope, "ENVELOPE", "Auto-wah filter", Palette::envFilter,
              { { envType, "Type" }, { envDirection, "Sweep" }, { envSensitivity, "Sens" }, { envResonance, "Reso" }, { envMix, "Mix" } } },
            { PedalId::wah, "WAH", "Bass wah", Palette::wah, { { wahPedal, "Pedal" }, { wahResonance, "Reso" }, { wahBlend, "Clean" } } },
            { PedalId::fuzz, "FUZZ", "Silicon fuzz", Palette::fuzz, { { fuzzAmount, "Fuzz" }, { fuzzTone, "Tone" }, { fuzzBlend, "Blend" } } },
            { PedalId::drive, "DRIVE", "Bass overdrive", Palette::distortion, { { distDrive, "Drive" }, { distTone, "Tone" }, { distBlend, "Blend" } } },
            { PedalId::phaser, "PHASER", "Four-stage phase", Palette::phaser,
              { { phaserRate, "Rate" }, { phaserDepth, "Depth" }, { phaserFeedback, "Regen" } } },
            { PedalId::flanger, "FLANGER", "Jet flanger", Palette::flanger,
              { { flangerRate, "Rate" }, { flangerDepth, "Depth" }, { flangerFeedback, "Regen" }, { flangerMix, "Mix" } } },
            { PedalId::chorus, "CHORUS", "Bass chorus", Palette::chorus, { { chorusRate, "Rate" }, { chorusDepth, "Depth" }, { chorusMix, "Mix" } } },
            { PedalId::tremolo, "TREMOLO", "Amp tremolo", Palette::tremolo,
              { { tremoloRate, "Rate" }, { tremoloDepth, "Depth" }, { tremoloShape, "Shape" } } },
            { PedalId::delay, "DELAY", "Analog echo", Palette::delay, { { delayTime, "Time" }, { delayFeedback, "Repeat" }, { delayMix, "Mix" } } },
            { PedalId::reverb, "REVERB", "Room reverb", Palette::reverb, { { reverbSize, "Size" }, { reverbDamping, "Damp" }, { reverbMix, "Mix" } } },
        };
    }

    int knobCount (const Pedal& pedal)
    {
        return juce::jmax (2, pedal.getNumKnobs());
    }
}

PedalBoard::PedalBoard (BassAmpProcessor& processorToUse, Board boardToShow)
    : processor (processorToUse), chain (processorToUse.pedalChain), board (boardToShow)
{
    for (const auto& spec : pedalSpecs())
    {
        if (pedalInfos[(size_t) spec.id].board != board)
            continue;

        auto pedal = std::make_unique<Pedal> (processor.parameters, spec.id, spec.name, spec.subtitle, spec.colour, spec.knobs,
                                              pedalInfos[(size_t) spec.id].toggleId);

        if (spec.id == PedalId::delay)
            pedal->addTapTempo (ParamIds::delayTime);

        pedal->onRemove = [this, id = spec.id] { chain.hide (id); };
        pedal->addMouseListener (this, false);
        content.addChildComponent (*pedal);
        pedals.push_back (std::move (pedal));
    }

    viewport.setViewedComponent (&content, false);
    viewport.setScrollBarsShown (false, true);
    viewport.setScrollBarThickness (scrollBarThickness);
    viewport.getHorizontalScrollBar().setColour (juce::ScrollBar::thumbColourId, Palette::gold.withAlpha (0.35f));
    addAndMakeVisible (viewport);

    addSlot.onClick = [this] { showAddMenu(); };
    addAndMakeVisible (addSlot);

    chain.addChangeListener (this);
    refresh();
    startTimerHz (5);
}

PedalBoard::~PedalBoard()
{
    stopTimer();
    chain.removeChangeListener (this);
}

void PedalBoard::changeListenerCallback (juce::ChangeBroadcaster*)
{
    refresh();
}

void PedalBoard::timerCallback()
{
    if (dragged == nullptr && chain.getShown (board) != shown)
        refresh();
}

void PedalBoard::refresh()
{
    shown = chain.getShown (board);

    for (auto& pedal : pedals)
        pedal->setVisible (std::find (shown.begin(), shown.end(), pedal->getId()) != shown.end());

    addSlot.setEnabled (! chain.getHidden (board).empty());
    layoutPedals();
}

Pedal* PedalBoard::pedalFor (PedalId id) const
{
    for (const auto& pedal : pedals)
        if (pedal->getId() == id)
            return pedal.get();

    return nullptr;
}

void PedalBoard::resized()
{
    auto area = getLocalBounds().reduced (padding);
    addSlot.setBounds (area.removeFromRight (addSlotWidth).withTrimmedBottom (scrollBarThickness));
    area.removeFromRight (pedalGap);
    viewport.setBounds (area);
    layoutPedals();
}

// Knob cells shrink to fit the board, down to a minimum; past that the board scrolls.
void PedalBoard::layoutPedals()
{
    const auto available = viewport.getWidth();
    const auto height = viewport.getHeight() - scrollBarThickness;
    auto knobs = 0;

    for (const auto id : shown)
        knobs += knobCount (*pedalFor (id));

    const auto count = (int) shown.size();
    const auto fixed = count * pedalChrome + juce::jmax (0, count - 1) * pedalGap;
    const auto cell = knobs > 0 ? juce::jlimit (minKnobCell, maxKnobCell, (available - fixed) / knobs) : maxKnobCell;

    auto x = 0;

    for (const auto id : shown)
    {
        auto* pedal = pedalFor (id);
        const auto width = knobCount (*pedal) * cell + pedalChrome;

        if (pedal != dragged)
            pedal->setBounds (x, 0, width, height);

        x += width + pedalGap;
    }

    content.setSize (juce::jmax (available, x - pedalGap), height);
}

void PedalBoard::showAddMenu()
{
    juce::PopupMenu menu;
    const auto specs = pedalSpecs();

    for (const auto id : chain.getHidden (board))
        for (const auto& spec : specs)
            if (spec.id == id)
                menu.addItem (juce::String (spec.name) + "  -  " + spec.subtitle, [this, id]
                {
                    chain.show (id);

                    if (auto* pedal = pedalFor (id))
                        viewport.setViewPosition (juce::jmax (0, pedal->getX() - padding), 0);
                });

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (addSlot));
}

void PedalBoard::mouseDown (const juce::MouseEvent& event)
{
    dragged = nullptr;
    dropIndex = -1;

    if (auto* pedal = dynamic_cast<Pedal*> (event.eventComponent))
        dragOffset = event.getEventRelativeTo (pedal).getMouseDownX();
}

void PedalBoard::mouseDrag (const juce::MouseEvent& event)
{
    auto* pedal = dynamic_cast<Pedal*> (event.eventComponent);

    if (pedal == nullptr || (dragged == nullptr && event.getDistanceFromDragStart() < dragThreshold))
        return;

    if (dragged == nullptr)
    {
        dragged = pedal;
        dragged->toFront (false);
        setMouseCursor (juce::MouseCursor::DraggingHandCursor);
    }

    const auto x = event.getEventRelativeTo (&content).position.x;
    dragged->setTopLeftPosition (juce::jlimit (0, content.getWidth() - dragged->getWidth(), juce::roundToInt (x) - dragOffset), 0);
    dropIndex = dropIndexFor (x);
    content.repaint();
}

void PedalBoard::mouseUp (const juce::MouseEvent&)
{
    if (dragged == nullptr)
        return;

    const auto id = dragged->getId();
    const auto from = (int) (std::find (shown.begin(), shown.end(), id) - shown.begin());
    const auto to = dropIndex > from ? dropIndex - 1 : dropIndex;
    dragged = nullptr;
    dropIndex = -1;
    setMouseCursor (juce::MouseCursor::NormalCursor);

    if (to != from)
        chain.move (id, to);

    refresh();
    content.repaint();
}

// The gap the dragged pedal would land in, counted among the shown pedals.
int PedalBoard::dropIndexFor (float contentX) const
{
    auto index = 0;

    for (const auto id : shown)
    {
        const auto* pedal = pedalFor (id);

        if (pedal == dragged)
        {
            ++index;
            continue;
        }

        if (contentX < (float) pedal->getBounds().getCentreX())
            return index;

        ++index;
    }

    return (int) shown.size();
}

void PedalBoard::paintDropIndicator (juce::Graphics& g) const
{
    if (dragged == nullptr || dropIndex < 0)
        return;

    const auto from = (int) (std::find (shown.begin(), shown.end(), dragged->getId()) - shown.begin());

    if (dropIndex == from || dropIndex == from + 1)
        return;

    const auto x = dropIndex < (int) shown.size() ? pedalFor (shown[(size_t) dropIndex])->getX() - pedalGap / 2
                                                  : pedalFor (shown.back())->getRight() + pedalGap / 2;

    const auto bar = juce::Rectangle<float> ((float) x - 2.0f, 6.0f, 4.0f, (float) content.getHeight() - 12.0f);
    g.setColour (Palette::gold.withAlpha (0.3f));
    g.fillRoundedRectangle (bar.expanded (3.0f, 0.0f), 4.0f);
    g.setColour (Palette::gold);
    g.fillRoundedRectangle (bar, 2.0f);
}

void PedalBoard::Content::paintOverChildren (juce::Graphics& g)
{
    board.paintDropIndicator (g);
}

void PedalBoard::paint (juce::Graphics& g)
{
    const auto area = getLocalBounds().toFloat();
    juce::Path outline;
    outline.addRoundedRectangle (area, 12.0f);
    juce::DropShadow (juce::Colours::black.withAlpha (0.6f), 18, { 0, 6 }).drawForPath (g, outline);

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff1e1f23), 0.0f, area.getY(), juce::Colour (0xff111215), 0.0f, area.getBottom(), false));
    g.fillPath (outline);

    g.saveState();
    g.reduceClipRegion (outline);

    for (int rail = 1; rail <= 3; ++rail)
    {
        const auto y = area.getY() + area.getHeight() * (float) rail / 4.0f;
        g.setColour (juce::Colours::black.withAlpha (0.55f));
        g.fillRect (area.getX(), y - 4.0f, area.getWidth(), 8.0f);
        g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.fillRect (area.getX(), y + 4.0f, area.getWidth(), 1.0f);
    }

    g.restoreState();
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawRoundedRectangle (area.reduced (0.5f), 12.0f, 1.0f);
}

void PedalBoard::AddSlot::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const auto bounds = getLocalBounds().toFloat().reduced (3.0f, 2.0f).withTrimmedBottom (4.0f);
    const auto alpha = ! isEnabled() ? 0.15f : shouldDrawButtonAsDown ? 0.9f : shouldDrawButtonAsHighlighted ? 0.75f : 0.45f;
    const auto ink = Palette::gold.withAlpha (alpha);

    juce::Path outline;
    outline.addRoundedRectangle (bounds, 10.0f);
    juce::Path dashed;
    const float dashes[] { 6.0f, 5.0f };
    juce::PathStrokeType (1.5f).createDashedStroke (dashed, outline, dashes, 2);

    g.setColour (juce::Colours::white.withAlpha (shouldDrawButtonAsHighlighted && isEnabled() ? 0.05f : 0.02f));
    g.fillPath (outline);
    g.setColour (ink);
    g.fillPath (dashed);

    const auto centre = bounds.getCentre().translated (0.0f, -14.0f);
    g.fillRoundedRectangle (juce::Rectangle<float> (18.0f, 3.0f).withCentre (centre), 1.5f);
    g.fillRoundedRectangle (juce::Rectangle<float> (3.0f, 18.0f).withCentre (centre), 1.5f);

    g.setFont (Fonts::label (10.5f));
    g.drawText ("ADD", bounds.withTop (centre.y + 14.0f).withHeight (14.0f), juce::Justification::centred);
}
