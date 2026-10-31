#include "Logo.h"
#include "AmpLookAndFeel.h"

void Logo::drawIcon (juce::Graphics& g, juce::Rectangle<float> square)
{
    const auto size = square.getWidth();
    const auto corner = size * 0.2f;
    const auto plate = square.reduced (size * 0.02f);

    Draw::brushedMetal (g, plate, juce::Colour (0xff2a2c32), corner);
    g.setGradientFill ({ juce::Colours::white.withAlpha (0.08f), plate.getX(), plate.getY(),
                         juce::Colours::black.withAlpha (0.35f), plate.getX(), plate.getBottom(), false });
    g.fillRoundedRectangle (plate, corner);

    const auto trim = plate.reduced (size * 0.02f);
    g.setGradientFill ({ juce::Colour (0xfff6dc9a), trim.getX(), trim.getY(),
                         juce::Colour (0xff9c6b25), trim.getRight(), trim.getBottom(), false });
    g.drawRoundedRectangle (trim, corner * 0.9f, size * 0.03f);

    const auto inset = size * 0.15f;
    for (const auto point : { plate.getTopLeft().translated (inset, inset), plate.getTopRight().translated (-inset, inset),
                              plate.getBottomLeft().translated (inset, -inset), plate.getBottomRight().translated (-inset, -inset) })
        Draw::screw (g, point, size * 0.032f);

    const auto centre = plate.getCentre();
    const auto radius = size * 0.32f;
    auto circle = [&centre] (float r) { return juce::Rectangle<float> (r * 2.0f, r * 2.0f).withCentre (centre); };

    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.fillEllipse (circle (radius * 1.1f).translated (0.0f, size * 0.015f));
    g.setColour (juce::Colour (0xff0a0a0c));
    g.fillEllipse (circle (radius * 1.08f));

    g.setGradientFill ({ juce::Colour (0xff3a3c42), centre.translated (-radius * 0.4f, -radius * 0.6f),
                         juce::Colour (0xff111215), centre.translated (radius * 0.5f, radius * 0.9f), false });
    g.fillEllipse (circle (radius));
    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.drawEllipse (circle (radius * 0.86f), size * 0.008f);

    g.setGradientFill ({ juce::Colour (0xff24262b), centre, juce::Colour (0xff17181b), centre.translated (radius * 0.84f, 0.0f), true });
    g.fillEllipse (circle (radius * 0.84f));

    for (const auto rib : { 0.48f, 0.6f, 0.72f })
    {
        g.setColour (juce::Colours::white.withAlpha (0.05f));
        g.drawEllipse (circle (radius * rib), size * 0.004f);
    }

    const auto cap = radius * 0.34f;
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillEllipse (circle (cap * 1.12f).translated (0.0f, size * 0.01f));
    juce::ColourGradient gold (juce::Colour (0xfffbe6ad), centre.translated (-cap * 0.4f, -cap * 0.5f),
                               juce::Colour (0xff7a5320), centre.translated (cap * 0.7f, cap * 0.9f), true);
    gold.addColour (0.45, Palette::gold);
    g.setGradientFill (gold);
    g.fillEllipse (circle (cap));

    g.setColour (juce::Colours::white.withAlpha (0.35f));
    g.fillEllipse (juce::Rectangle<float> (cap * 0.7f, cap * 0.35f).withCentre (centre.translated (-cap * 0.25f, -cap * 0.5f)));
}

juce::Image Logo::createIcon (int size)
{
    juce::Image image (juce::Image::ARGB, size, size, true);
    juce::Graphics g (image);
    drawIcon (g, image.getBounds().toFloat());
    return image;
}

juce::Image Logo::createSplash (const juce::String& version)
{
    juce::Image image (juce::Image::RGB, 620, 280, true);
    juce::Graphics g (image);
    const auto bounds = image.getBounds().toFloat();

    g.fillAll (Palette::background);
    Draw::unitChassis (g, bounds);
    drawIcon (g, juce::Rectangle<float> (190.0f, 190.0f).withCentre ({ 140.0f, bounds.getCentreY() - 2.0f }));

    auto text = bounds.withTrimmedLeft (264.0f).withTrimmedRight (30.0f).withSizeKeepingCentre (bounds.getWidth() - 294.0f, 130.0f);
    g.setColour (Palette::gold);
    g.setFont (Fonts::display (58.0f).withKerningFactor (0.08f));
    g.drawText ("BASS AMP", text.removeFromTop (66.0f), juce::Justification::centredLeft);

    g.setColour (Palette::text.withAlpha (0.85f));
    g.setFont (Fonts::display (17.0f, false));
    g.drawText ("Amp, pedalboard & practice", text.removeFromTop (28.0f), juce::Justification::centredLeft);

    g.setColour (Palette::textDim);
    g.setFont (Fonts::label (13.0f));
    g.drawText ("v" + version, text.removeFromTop (28.0f), juce::Justification::bottomLeft);
    return image;
}
