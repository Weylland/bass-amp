#include "EqDisplay.h"
#include "AmpLookAndFeel.h"
#include "Help.h"

namespace
{
    constexpr float minFrequency = 20.0f;
    constexpr float maxFrequency = 20000.0f;
    constexpr float basicRangeDb = 12.0f;
    constexpr float advancedRangeDb = 18.0f;
    constexpr float spectrumFloorDb = -96.0f;
    constexpr float spectrumDecayDb = 2.5f;
    constexpr float nodeRadius = 7.0f;
    constexpr int faderThumbRadius = 12;   // LookAndFeel_V4 insets a linear slider's travel by its thumb radius

    juce::String formatFrequency (float frequency)
    {
        return frequency >= 1000.0f ? juce::String (frequency / 1000.0f, frequency >= 10000.0f ? 1 : 2) + " kHz"
                                    : juce::String (juce::roundToInt (frequency)) + " Hz";
    }
}

EqDisplay::EqDisplay (juce::AudioProcessorValueTreeState& stateToUse, SpectrumTap& tapToUse)
    : state (stateToUse), tap (tapToUse), sources (stateToUse)
{
    using namespace ParamIds;

    nodes = {{
        { "LOW CUT",   peqLowCut,   nullptr,     nullptr,   juce::Colour (0xffb8bcc6) },
        { "LOW",       peqLowFreq,  peqLowGain,  nullptr,   juce::Colour (0xffff8a3d) },
        { "LO-MID",    peqMid1Freq, peqMid1Gain, peqMid1Q,  juce::Colour (0xffffd04a) },
        { "HI-MID",    peqMid2Freq, peqMid2Gain, peqMid2Q,  juce::Colour (0xff4ad8ff) },
        { "HIGH",      peqHighFreq, peqHighGain, nullptr,   juce::Colour (0xffb98aff) },
        { "HIGH CUT",  peqHighCut,  nullptr,     nullptr,   juce::Colour (0xffb8bcc6) },
    }};

    for (const auto* paramId : graphicEq)
    {
        auto* fader = faders.add (new juce::Slider (juce::Slider::LinearVertical, juce::Slider::NoTextBox));
        fader->setPopupDisplayEnabled (true, true, nullptr);
        fader->setColour (juce::Slider::trackColourId, Palette::gold);
        fader->setDoubleClickReturnValue (true, 0.0);
        addAndMakeVisible (fader);
        faderAttachments.add (new juce::AudioProcessorValueTreeState::SliderAttachment (state, paramId, *fader));
        fader->setComponentID (paramId);
        fader->onDragStart = [this] { switchOn(); };
    }
}

void EqDisplay::setMode (bool isAdvanced, bool isActive)
{
    if (isAdvanced == advanced && isActive == active)
        return;

    advanced = isAdvanced;
    active = isActive;

    for (auto* fader : faders)
        fader->setVisible (! advanced);

    hoveredNode = draggedNode = -1;
    repaint();
}

juce::Rectangle<float> EqDisplay::plotArea() const
{
    return getLocalBounds().toFloat().reduced (10.0f, 14.0f).withTrimmedBottom (6.0f);
}

float EqDisplay::xForFrequency (float frequency) const
{
    const auto plot = plotArea();
    return plot.getX() + plot.getWidth() * std::log (frequency / minFrequency) / std::log (maxFrequency / minFrequency);
}

float EqDisplay::frequencyForX (float x) const
{
    const auto plot = plotArea();
    return minFrequency * std::pow (maxFrequency / minFrequency, (x - plot.getX()) / plot.getWidth());
}

float EqDisplay::yForDb (float db) const
{
    const auto plot = plotArea();
    const auto range = advanced ? advancedRangeDb : basicRangeDb;
    return plot.getCentreY() - db / range * plot.getHeight() * 0.5f;
}

float EqDisplay::dbForY (float y) const
{
    const auto plot = plotArea();
    const auto range = advanced ? advancedRangeDb : basicRangeDb;
    return (plot.getCentreY() - y) / (plot.getHeight() * 0.5f) * range;
}

float EqDisplay::value (const char* paramId) const
{
    return state.getRawParameterValue (paramId)->load();
}

void EqDisplay::setValue (const char* paramId, float newValue)
{
    if (auto* parameter = state.getParameter (paramId))
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (newValue));
}

void EqDisplay::switchOn()
{
    if (auto* parameter = state.getParameter (ParamIds::eqOn); parameter != nullptr && parameter->getValue() < 0.5f)
        parameter->setValueNotifyingHost (1.0f);
}

std::vector<std::array<float, 6>> EqDisplay::currentCoefficients() const
{
    const auto sampleRate = tap.getSampleRate();

    if (advanced)
    {
        const auto coefficients = Equalizer::design (sources.parametricBands(), sampleRate);
        return { coefficients.begin(), coefficients.end() };
    }

    const auto coefficients = Equalizer::design (sources.graphicBands(), sampleRate);
    return { coefficients.begin(), coefficients.end() };
}

void EqDisplay::updateSpectrum()
{
    const auto received = tap.pull (incoming.data(), (int) incoming.size());

    if (received <= 0)
    {
        for (auto& db : spectrumDb)
            db = juce::jmax (spectrumFloorDb, db - spectrumDecayDb);

        repaint();
        return;
    }

    std::move (history.begin() + received, history.end(), history.begin());
    std::copy_n (incoming.begin(), received, history.end() - received);

    std::copy (history.begin(), history.end(), fftData.begin());
    std::fill (fftData.begin() + fftSize, fftData.end(), 0.0f);
    window.multiplyWithWindowingTable (fftData.data(), (size_t) fftSize);
    fft.performFrequencyOnlyForwardTransform (fftData.data());

    // A Hann window halves the amplitude: a full-scale sine reads 0 dB.
    const auto scale = 4.0f / (float) fftSize;

    for (size_t bin = 0; bin < spectrumDb.size(); ++bin)
    {
        const auto db = juce::Decibels::gainToDecibels (fftData[bin] * scale, spectrumFloorDb);
        spectrumDb[bin] = db > spectrumDb[bin] ? db : juce::jmax (db, spectrumDb[bin] - spectrumDecayDb);
    }

    repaint();
}

void EqDisplay::resized()
{
    const auto plot = plotArea();

    for (size_t i = 0; i < faders.size(); ++i)
    {
        const auto x = xForFrequency (Equalizer::graphicFrequencies[i]);
        faders[(int) i]->setBounds (juce::Rectangle<int> (30, juce::roundToInt (plot.getHeight()) + 2 * faderThumbRadius)
                                        .withCentre ({ juce::roundToInt (x), juce::roundToInt (plot.getCentreY()) }));
    }
}

void EqDisplay::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    Draw::screen (g, bounds, 8.0f);

    const auto plot = plotArea();
    paintGrid (g, plot);
    paintSpectrum (g, plot);
    paintCurve (g, plot);

    if (advanced)
        paintNodes (g);
}

void EqDisplay::paintGrid (juce::Graphics& g, juce::Rectangle<float> plot)
{
    g.setFont (Fonts::display (10.0f, false));

    for (const auto frequency : { 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f })
    {
        const auto x = xForFrequency (frequency);
        g.setColour (juce::Colours::white.withAlpha (0.06f));
        g.fillRect (x, plot.getY(), 1.0f, plot.getHeight());
        g.setColour (juce::Colours::white.withAlpha (0.3f));
        const auto text = frequency >= 1000.0f ? juce::String (juce::roundToInt (frequency / 1000.0f)) + "k" : juce::String (juce::roundToInt (frequency));
        g.drawText (text, juce::Rectangle<float> (40.0f, 12.0f).withCentre ({ x, plot.getBottom() + 9.0f }), juce::Justification::centred);
    }

    const auto range = advanced ? advancedRangeDb : basicRangeDb;

    for (auto db = -range; db <= range + 0.1f; db += range / 2.0f)
    {
        const auto y = yForDb (db);
        g.setColour (juce::Colours::white.withAlpha (juce::exactlyEqual (db, 0.0f) ? 0.16f : 0.06f));
        g.fillRect (plot.getX(), y, plot.getWidth(), 1.0f);

        if (! juce::exactlyEqual (db, 0.0f))
        {
            g.setColour (juce::Colours::white.withAlpha (0.3f));
            g.drawText ((db > 0.0f ? "+" : "") + juce::String (juce::roundToInt (db)), juce::Rectangle<float> (plot.getX() + 2.0f, y - 12.0f, 30.0f, 11.0f),
                        juce::Justification::centredLeft);
        }
    }
}

void EqDisplay::paintSpectrum (juce::Graphics& g, juce::Rectangle<float> plot)
{
    const auto sampleRate = tap.getSampleRate();
    const auto binWidth = (float) sampleRate / (float) fftSize;
    const auto numBins = (int) spectrumDb.size();
    juce::Path spectrum;
    spectrum.startNewSubPath (plot.getX(), plot.getBottom());

    for (auto x = plot.getX(); x <= plot.getRight(); x += 2.0f)
    {
        const auto lowBin = juce::jlimit (1, numBins - 1, (int) (frequencyForX (x - 1.0f) / binWidth));
        const auto highBin = juce::jlimit (lowBin, numBins - 1, (int) (frequencyForX (x + 1.0f) / binWidth));
        auto db = spectrumFloorDb;

        for (auto bin = lowBin; bin <= highBin; ++bin)
            db = juce::jmax (db, spectrumDb[(size_t) bin]);

        const auto proportion = juce::jlimit (0.0f, 1.0f, (db - spectrumFloorDb) / -spectrumFloorDb);
        spectrum.lineTo (x, plot.getBottom() - proportion * plot.getHeight());
    }

    spectrum.lineTo (plot.getRight(), plot.getBottom());
    spectrum.closeSubPath();

    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3d8bff).withAlpha (0.35f), 0.0f, plot.getY(),
                                             juce::Colour (0xff3d8bff).withAlpha (0.04f), 0.0f, plot.getBottom(), false));
    g.fillPath (spectrum);
    g.setColour (juce::Colour (0xff6fb0ff).withAlpha (0.35f));
    g.strokePath (spectrum, juce::PathStrokeType (1.0f));
}

void EqDisplay::paintCurve (juce::Graphics& g, juce::Rectangle<float> plot)
{
    const auto colour = active ? Palette::gold : Palette::textDim;
    const auto zeroY = yForDb (0.0f);
    const auto coefficients = currentCoefficients();
    const auto sampleRate = tap.getSampleRate();
    const auto yAt = [&] (float x)
    {
        auto db = 0.0f;

        for (const auto& c : coefficients)
            db += active ? magnitudeDb (c, frequencyForX (x), sampleRate) : 0.0f;

        return juce::jlimit (plot.getY(), plot.getBottom(), yForDb (db));
    };

    juce::Path curve, area;
    curve.startNewSubPath (plot.getX(), yAt (plot.getX()));
    area.startNewSubPath (plot.getX(), zeroY);

    for (auto x = plot.getX(); x <= plot.getRight(); x += 2.0f)
    {
        const auto y = yAt (x);
        curve.lineTo (x, y);
        area.lineTo (x, y);
    }

    area.lineTo (plot.getRight(), zeroY);
    area.closeSubPath();

    g.setColour (colour.withAlpha (0.12f));
    g.fillPath (area);
    g.setColour (colour.withAlpha (0.25f));
    g.strokePath (curve, juce::PathStrokeType (6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (colour);
    g.strokePath (curve, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

juce::Point<float> EqDisplay::nodePosition (int index) const
{
    const auto& node = nodes[(size_t) index];
    const auto x = xForFrequency (value (node.frequencyId));
    const auto y = node.gainId != nullptr ? yForDb (value (node.gainId)) : yForDb (0.0f);
    return { x, y };
}

int EqDisplay::nodeAt (juce::Point<float> position) const
{
    auto best = -1;
    auto bestDistance = nodeRadius * 2.2f;

    for (int i = 0; i < (int) nodes.size(); ++i)
    {
        const auto distance = nodePosition (i).getDistanceFrom (position);

        if (distance < bestDistance)
        {
            best = i;
            bestDistance = distance;
        }
    }

    return best;
}

void EqDisplay::paintNodes (juce::Graphics& g)
{
    for (int i = 0; i < (int) nodes.size(); ++i)
    {
        const auto& node = nodes[(size_t) i];
        const auto centre = nodePosition (i);
        const auto highlighted = i == hoveredNode || i == draggedNode;
        const auto colour = active ? node.colour : Palette::textDim;
        const auto radius = highlighted ? nodeRadius + 2.0f : nodeRadius;

        if (highlighted)
        {
            g.setColour (colour.withAlpha (0.25f));
            g.fillEllipse (juce::Rectangle<float> (radius * 4.0f, radius * 4.0f).withCentre (centre));
        }

        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f).withCentre (centre.translated (0.0f, 1.5f)));
        g.setColour (colour);
        g.fillEllipse (juce::Rectangle<float> (radius * 2.0f, radius * 2.0f).withCentre (centre));
        g.setColour (juce::Colours::black.withAlpha (0.75f));
        g.setFont (Fonts::display (10.0f));
        g.drawText (juce::String (i + 1), juce::Rectangle<float> (radius * 2.0f, radius * 2.0f).withCentre (centre), juce::Justification::centred);
    }

    const auto shown = draggedNode >= 0 ? draggedNode : hoveredNode;

    if (shown < 0)
        return;

    const auto& node = nodes[(size_t) shown];
    juce::String text = juce::String (node.name) + "   " + formatFrequency (value (node.frequencyId));

    if (node.gainId != nullptr)
        text << "   " << (value (node.gainId) >= 0.0f ? "+" : "") << juce::String (value (node.gainId), 1) << " dB";

    if (node.qId != nullptr)
        text << "   Q " << juce::String (value (node.qId), 2);

    const auto plot = plotArea();
    g.setColour (node.colour);
    g.setFont (Fonts::display (12.0f));
    g.drawText (text, juce::Rectangle<float> (plot.getX() + 34.0f, plot.getY() + 2.0f, plot.getWidth() - 40.0f, 14.0f),
                juce::Justification::centredLeft);
}

juce::String EqDisplay::getTooltip()
{
    return Help::get (advanced && hoveredNode >= 0 ? juce::String (nodes[(size_t) hoveredNode].frequencyId) : juce::String ("ui.eqScreen"));
}

void EqDisplay::mouseMove (const juce::MouseEvent& event)
{
    if (! advanced)
        return;

    const auto node = nodeAt (event.position);

    if (node != hoveredNode)
    {
        hoveredNode = node;
        setMouseCursor (node >= 0 ? juce::MouseCursor::DraggingHandCursor : juce::MouseCursor::NormalCursor);
        repaint();
    }
}

void EqDisplay::mouseExit (const juce::MouseEvent&)
{
    hoveredNode = -1;
    repaint();
}

void EqDisplay::mouseDown (const juce::MouseEvent& event)
{
    if (! advanced || (draggedNode = nodeAt (event.position)) < 0)
        return;

    switchOn();
    const auto& node = nodes[(size_t) draggedNode];

    for (const auto* paramId : { node.frequencyId, node.gainId })
        if (paramId != nullptr)
            state.getParameter (paramId)->beginChangeGesture();
}

void EqDisplay::mouseDrag (const juce::MouseEvent& event)
{
    if (draggedNode < 0)
        return;

    const auto& node = nodes[(size_t) draggedNode];
    const auto plot = plotArea();
    setValue (node.frequencyId, frequencyForX (juce::jlimit (plot.getX(), plot.getRight(), event.position.x)));

    if (node.gainId != nullptr)
        setValue (node.gainId, dbForY (juce::jlimit (plot.getY(), plot.getBottom(), event.position.y)));

    repaint();
}

void EqDisplay::mouseUp (const juce::MouseEvent&)
{
    if (draggedNode < 0)
        return;

    const auto& node = nodes[(size_t) draggedNode];

    for (const auto* paramId : { node.frequencyId, node.gainId })
        if (paramId != nullptr)
            state.getParameter (paramId)->endChangeGesture();

    draggedNode = -1;
    repaint();
}

void EqDisplay::mouseDoubleClick (const juce::MouseEvent& event)
{
    const auto index = advanced ? nodeAt (event.position) : -1;

    if (index < 0)
        return;

    for (const auto* paramId : { nodes[(size_t) index].frequencyId, nodes[(size_t) index].gainId, nodes[(size_t) index].qId })
        if (auto* parameter = paramId != nullptr ? state.getParameter (paramId) : nullptr)
            parameter->setValueNotifyingHost (parameter->getDefaultValue());

    repaint();
}

void EqDisplay::mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    const auto index = advanced ? nodeAt (event.position) : -1;

    if (index < 0 || nodes[(size_t) index].qId == nullptr)
        return;

    switchOn();
    const auto* qId = nodes[(size_t) index].qId;
    setValue (qId, value (qId) * std::pow (1.25f, wheel.deltaY * 4.0f));
    repaint();
}
