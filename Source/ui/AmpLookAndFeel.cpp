#include "AmpLookAndFeel.h"

namespace
{
    using juce::Colour;
    using juce::ColourGradient;
    using juce::Point;
    using juce::Rectangle;

    Rectangle<float> circle (Point<float> centre, float radius)
    {
        return { centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f };
    }

    Point<float> polar (Point<float> centre, float radius, float angle)
    {
        return centre.getPointOnCircumference (radius, angle);
    }

    void dropShadow (juce::Graphics& g, Point<float> centre, float radius)
    {
        ColourGradient shadow (juce::Colours::black.withAlpha (0.55f), centre.x, centre.y + radius * 0.15f,
                               juce::Colours::transparentBlack, centre.x, centre.y + radius * 1.35f, true);
        g.setGradientFill (shadow);
        g.fillEllipse (circle (centre.translated (0.0f, radius * 0.15f), radius * 1.3f));
    }

    bool isBadge (const juce::ComboBox& box)
    {
        return box.getProperties()["comboStyle"].toString() == "badge";
    }

    void knurl (juce::Graphics& g, Point<float> centre, float inner, float outer, int count, float alpha)
    {
        g.setColour (juce::Colours::white.withAlpha (alpha));

        for (int i = 0; i < count; ++i)
        {
            const auto angle = juce::MathConstants<float>::twoPi * (float) i / (float) count;
            g.drawLine ({ polar (centre, inner, angle), polar (centre, outer, angle) }, 1.0f);
        }
    }
}

juce::FontOptions Fonts::display (float height, bool bold)
{
   #if JUCE_WINDOWS
    static const juce::String family { "Bahnschrift" };
   #elif JUCE_MAC
    static const juce::String family { "Avenir Next Condensed" };
   #else
    static const juce::String family = juce::Font::getDefaultSansSerifFontName();
   #endif

    return juce::FontOptions (family, height, bold ? juce::Font::bold : juce::Font::plain);
}

juce::FontOptions Fonts::label (float height)
{
    return display (height).withKerningFactor (0.08f);
}

AmpLookAndFeel::AmpLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, Palette::textDim);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId, Palette::track);
    setColour (juce::Slider::rotarySliderFillColourId, Palette::text);
    setColour (juce::Slider::thumbColourId, juce::Colour (0xff16161a));
    setColour (juce::Slider::trackColourId, Palette::gold);
    setColour (juce::Label::textColourId, Palette::text);
    setColour (juce::ResizableWindow::backgroundColourId, Palette::background);
    setColour (juce::BubbleComponent::backgroundColourId, juce::Colour (0xf0101114));
    setColour (juce::BubbleComponent::outlineColourId, Palette::gold.withAlpha (0.6f));
    setColour (juce::TooltipWindow::textColourId, Palette::text);

    setColour (juce::ComboBox::backgroundColourId, Palette::screen);
    setColour (juce::ComboBox::outlineColourId, juce::Colours::white.withAlpha (0.12f));
    setColour (juce::ComboBox::textColourId, Palette::text);
    setColour (juce::ComboBox::arrowColourId, Palette::gold);
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff15161a));
    setColour (juce::PopupMenu::textColourId, Palette::text);
    setColour (juce::PopupMenu::headerTextColourId, Palette::gold);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, Palette::gold.withAlpha (0.25f));
    setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    setColour (juce::TextButton::buttonColourId, juce::Colour (0xff25272d));
    setColour (juce::TextButton::buttonOnColourId, Palette::gold);
    setColour (juce::TextButton::textColourOffId, Palette::text);
    setColour (juce::TextButton::textColourOnId, juce::Colour (0xff111111));
    setColour (juce::AlertWindow::backgroundColourId, Palette::panel);
    setColour (juce::AlertWindow::textColourId, Palette::text);
    setColour (juce::AlertWindow::outlineColourId, Palette::gold.withAlpha (0.4f));
    setColour (juce::TextEditor::backgroundColourId, Palette::screen);
    setColour (juce::TextEditor::textColourId, Palette::text);
    setColour (juce::TextEditor::outlineColourId, juce::Colours::white.withAlpha (0.15f));
    setColour (juce::TextEditor::focusedOutlineColourId, Palette::gold);
}

void AmpLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    const auto side = (float) juce::jmin (width, height);
    const auto bounds = Rectangle<float> (side, side).withCentre (Rectangle<int> (x, y, width, height).toFloat().getCentre());
    const auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const auto style = slider.getProperties()[KnobStyle::property].toString();

    if (style == KnobStyle::pedal)
        drawPedalKnob (g, bounds, angle, slider);
    else if (style == KnobStyle::skirted)
        drawSkirtedKnob (g, bounds, angle, slider);
    else if (style == KnobStyle::chicken)
        drawChickenKnob (g, bounds, angle, slider);
    else
        drawModernKnob (g, bounds, angle, rotaryStartAngle, rotaryEndAngle, sliderPos, slider);
}

void AmpLookAndFeel::drawPedalKnob (juce::Graphics& g, Rectangle<float> bounds, float angle, juce::Slider& slider)
{
    const auto centre = bounds.getCentre();
    const auto radius = bounds.getWidth() * 0.42f;
    const auto body = slider.findColour (juce::Slider::thumbColourId);

    dropShadow (g, centre, radius);

    g.setGradientFill (ColourGradient (body.brighter (0.25f), centre.x, centre.y - radius,
                                       body.darker (0.8f), centre.x, centre.y + radius, false));
    g.fillEllipse (circle (centre, radius));
    knurl (g, centre, radius * 0.84f, radius * 0.98f, 30, 0.07f);

    const auto top = radius * 0.76f;
    g.setGradientFill (ColourGradient (body.brighter (0.35f), centre.x - top * 0.4f, centre.y - top * 0.6f,
                                       body.darker (0.4f), centre.x + top * 0.5f, centre.y + top, true));
    g.fillEllipse (circle (centre, top));
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.drawEllipse (circle (centre, top), 1.0f);

    g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId));
    g.drawLine ({ polar (centre, top * 0.2f, angle), polar (centre, top * 0.92f, angle) }, juce::jmax (1.6f, radius * 0.1f));
}

void AmpLookAndFeel::drawSkirtedKnob (juce::Graphics& g, Rectangle<float> bounds, float angle, juce::Slider& slider)
{
    const auto centre = bounds.getCentre();
    const auto radius = bounds.getWidth() * 0.44f;
    const auto cap = slider.findColour (juce::Slider::thumbColourId);

    dropShadow (g, centre, radius);

    g.setGradientFill (ColourGradient (Colour (0xff34353b), centre.x, centre.y - radius,
                                       Colour (0xff050506), centre.x, centre.y + radius, false));
    g.fillEllipse (circle (centre, radius));
    g.setColour (juce::Colours::white.withAlpha (0.08f));
    g.drawEllipse (circle (centre, radius - 0.5f), 1.0f);

    g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId));
    g.drawLine ({ polar (centre, radius * 0.66f, angle), polar (centre, radius * 0.97f, angle) }, juce::jmax (2.0f, radius * 0.09f));

    const auto capRadius = radius * 0.6f;
    g.setColour (juce::Colour (0xff0c0c0e));
    g.fillEllipse (circle (centre, capRadius + 1.5f));
    knurl (g, centre, capRadius * 0.98f, capRadius + 1.5f, 40, 0.12f);

    g.setGradientFill (ColourGradient (cap.brighter (0.5f), centre.x - capRadius * 0.6f, centre.y - capRadius * 0.7f,
                                       cap.darker (0.6f), centre.x + capRadius * 0.6f, centre.y + capRadius * 0.8f, false));
    g.fillEllipse (circle (centre, capRadius));

    for (auto ring = 0.3f; ring < 0.95f; ring += 0.16f)
    {
        g.setColour (juce::Colours::white.withAlpha (0.06f));
        g.drawEllipse (circle (centre, capRadius * ring), 0.8f);
    }

    g.setColour (juce::Colours::white.withAlpha (0.35f));
    g.fillEllipse (circle ({ centre.x - capRadius * 0.35f, centre.y - capRadius * 0.4f }, capRadius * 0.18f));
}

void AmpLookAndFeel::drawChickenKnob (juce::Graphics& g, Rectangle<float> bounds, float angle, juce::Slider& slider)
{
    const auto centre = bounds.getCentre();
    const auto radius = bounds.getWidth() * 0.46f;
    const auto ivory = slider.findColour (juce::Slider::thumbColourId);

    dropShadow (g, centre, radius * 0.8f);

    const auto baseRadius = radius * 0.6f;
    g.setGradientFill (ColourGradient (ivory.brighter (0.2f), centre.x, centre.y - baseRadius,
                                       ivory.darker (0.45f), centre.x, centre.y + baseRadius, false));
    g.fillEllipse (circle (centre, baseRadius));

    const auto pointerWidth = radius * 0.5f;
    juce::Path pointer;
    pointer.startNewSubPath (0.0f, -radius);
    pointer.quadraticTo (pointerWidth * 0.5f, -radius * 0.55f, pointerWidth * 0.62f, radius * 0.1f);
    pointer.quadraticTo (0.0f, radius * 0.62f, -pointerWidth * 0.62f, radius * 0.1f);
    pointer.quadraticTo (-pointerWidth * 0.5f, -radius * 0.55f, 0.0f, -radius);
    pointer.closeSubPath();

    const auto transform = juce::AffineTransform::rotation (angle).translated (centre);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.fillPath (pointer, transform.translated (0.0f, 2.0f));
    g.setGradientFill (ColourGradient (ivory.brighter (0.3f), centre.x - radius * 0.3f, centre.y - radius * 0.5f,
                                       ivory.darker (0.35f), centre.x + radius * 0.4f, centre.y + radius * 0.6f, false));
    g.fillPath (pointer, transform);
    g.setColour (ivory.darker (0.7f));
    g.strokePath (pointer, juce::PathStrokeType (0.8f), transform);

    g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId));
    g.drawLine ({ polar (centre, radius * 0.45f, angle), polar (centre, radius * 0.92f, angle) }, juce::jmax (1.5f, radius * 0.07f));

    g.setGradientFill (ColourGradient (ivory.darker (0.1f), centre.x, centre.y - radius * 0.25f,
                                       ivory.darker (0.5f), centre.x, centre.y + radius * 0.25f, false));
    g.fillEllipse (circle (centre, radius * 0.24f));
}

void AmpLookAndFeel::drawModernKnob (juce::Graphics& g, Rectangle<float> bounds, float angle, float startAngle,
                                     float endAngle, float sliderPos, juce::Slider& slider)
{
    const auto centre = bounds.getCentre();
    const auto radius = bounds.getWidth() * 0.46f;
    const auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);
    const auto body = slider.findColour (juce::Slider::thumbColourId);

    const auto isBipolar = slider.getMinimum() < 0.0 && slider.getMaximum() > 0.0;
    const auto origin = isBipolar ? startAngle + (float) slider.valueToProportionOfLength (0.0) * (endAngle - startAngle) : startAngle;
    juce::ignoreUnused (sliderPos);

    const juce::PathStrokeType stroke (juce::jmax (2.5f, radius * 0.1f), juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
    juce::Path track, value;
    track.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, startAngle, endAngle, true);
    value.addCentredArc (centre.x, centre.y, radius, radius, 0.0f, juce::jmin (origin, angle), juce::jmax (origin, angle), true);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.strokePath (track, stroke);
    g.setColour (accent.withAlpha (0.25f));
    g.strokePath (value, juce::PathStrokeType (stroke.getStrokeThickness() * 2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (accent);
    g.strokePath (value, stroke);

    const auto disc = radius * 0.74f;
    dropShadow (g, centre, disc);
    g.setGradientFill (ColourGradient (body.brighter (0.3f), centre.x, centre.y - disc,
                                       body.darker (0.5f), centre.x, centre.y + disc, false));
    g.fillEllipse (circle (centre, disc));
    g.setColour (juce::Colours::white.withAlpha (0.1f));
    g.drawEllipse (circle (centre, disc - 0.5f), 1.0f);

    g.setColour (accent);
    g.fillEllipse (circle (polar (centre, disc * 0.68f, angle), juce::jmax (1.8f, disc * 0.1f)));
}

void AmpLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float,
                                       float, juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style != juce::Slider::LinearVertical)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, 0.0f, 0.0f, style, slider);
        return;
    }

    const auto bounds = Rectangle<int> (x, y, width, height).toFloat();
    const auto centreX = bounds.getCentreX();
    const auto zeroY = bounds.getY() + bounds.getHeight() * (1.0f - (float) slider.valueToProportionOfLength (0.0));
    const auto accent = slider.findColour (juce::Slider::trackColourId);

    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.fillRoundedRectangle (Rectangle<float> (4.0f, bounds.getHeight()).withCentre (bounds.getCentre()), 2.0f);
    g.setColour (accent.withAlpha (0.85f));
    g.fillRect (Rectangle<float> (centreX - 1.5f, juce::jmin (zeroY, sliderPos), 3.0f, std::abs (sliderPos - zeroY)));

    const auto cap = Rectangle<float> (juce::jmin (26.0f, bounds.getWidth() - 4.0f), 14.0f).withCentre ({ centreX, sliderPos });
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillRoundedRectangle (cap.translated (0.0f, 2.0f), 3.0f);
    g.setGradientFill (ColourGradient (Colour (0xffd8dbe0), cap.getX(), cap.getY(), Colour (0xff6b6f78), cap.getX(), cap.getBottom(), false));
    g.fillRoundedRectangle (cap, 3.0f);
    g.setColour (juce::Colours::black.withAlpha (0.25f));

    for (auto groove = -3.0f; groove <= 3.0f; groove += 3.0f)
        g.fillRect (cap.getX() + 3.0f, cap.getCentreY() + groove, cap.getWidth() - 6.0f, 1.0f);

    g.setColour (accent);
    g.fillRect (cap.getX() + 2.0f, cap.getCentreY() - 0.5f, cap.getWidth() - 4.0f, 1.5f);
}

void AmpLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool, bool)
{
    if (button.getProperties()[ToggleStyle::property].toString() == ToggleStyle::batSwitch)
    {
        drawBatSwitch (g, button);
        return;
    }

    const auto bounds = button.getLocalBounds().toFloat().reduced (1.0f);
    const auto radius = bounds.getHeight() / 2.0f;
    const auto isOn = button.getToggleState();
    const auto accent = button.findColour (juce::ToggleButton::tickColourId);

    g.setGradientFill (ColourGradient (Colour (0xff2a2c32), 0.0f, bounds.getY(), Colour (0xff141518), 0.0f, bounds.getBottom(), false));
    g.fillRoundedRectangle (bounds, radius);
    g.setColour (isOn ? accent.withAlpha (0.7f) : juce::Colours::white.withAlpha (0.12f));
    g.drawRoundedRectangle (bounds, radius, 1.0f);

    Draw::led (g, { bounds.getX() + radius + 1.0f, bounds.getCentreY() }, 3.5f, accent, isOn);

    g.setColour (isOn ? Palette::text : Palette::textDim);
    g.setFont (Fonts::label (12.0f));
    g.drawText (button.getButtonText().isNotEmpty() ? button.getButtonText() : (isOn ? "ON" : "OFF"),
                bounds.withTrimmedLeft (radius + 6.0f), juce::Justification::centred);
}

void AmpLookAndFeel::drawBatSwitch (juce::Graphics& g, juce::ToggleButton& button)
{
    const auto bounds = button.getLocalBounds().toFloat();
    const auto centre = bounds.getCentre();
    const auto isOn = button.getToggleState();
    const auto size = juce::jmin (bounds.getWidth(), bounds.getHeight());

    const auto nut = size * 0.2f;
    g.setGradientFill (ColourGradient (Colour (0xffe6e8ec), centre.x - nut, centre.y - nut, Colour (0xff6d7078), centre.x + nut, centre.y + nut, false));
    g.fillEllipse (circle (centre, nut));
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.drawEllipse (circle (centre, nut), 1.0f);

    const auto tip = Point<float> (centre.x, centre.y + (isOn ? -1.0f : 1.0f) * size * 0.42f);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.drawLine ({ centre.translated (1.5f, 2.0f), tip.translated (1.5f, 2.0f) }, size * 0.12f);
    g.setGradientFill (ColourGradient (Colour (0xfff4f5f7), centre.x - 3.0f, 0.0f, Colour (0xff7d8089), centre.x + 3.0f, 0.0f, false));
    g.drawLine ({ centre, tip }, size * 0.12f);
    g.fillEllipse (circle (tip, size * 0.1f));
}

void AmpLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);
    const auto isOn = button.getToggleState();
    auto base = isOn ? button.findColour (juce::TextButton::buttonOnColourId) : backgroundColour;

    if (shouldDrawButtonAsDown)
        base = base.darker (0.2f);
    else if (shouldDrawButtonAsHighlighted)
        base = base.brighter (0.12f);

    g.setGradientFill (ColourGradient (base.brighter (0.12f), 0.0f, bounds.getY(), base.darker (0.25f), 0.0f, bounds.getBottom(), false));
    g.fillRoundedRectangle (bounds, 5.0f);
    g.setColour (isOn ? base.brighter (0.4f) : juce::Colours::white.withAlpha (0.1f));
    g.drawRoundedRectangle (bounds, 5.0f, 1.0f);
}

juce::Font AmpLookAndFeel::getTextButtonFont (juce::TextButton&, int buttonHeight)
{
    return juce::Font (Fonts::label (juce::jmin (14.0f, (float) buttonHeight * 0.55f)));
}

void AmpLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box)
{
    const auto bounds = Rectangle<int> (width, height).toFloat().reduced (0.5f);

    if (! isBadge (box))
    {
        Draw::screen (g, bounds, 5.0f);
        g.setColour (box.findColour (juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle (bounds, 5.0f, 1.0f);
    }

    const auto arrowZone = bounds.withLeft (bounds.getRight() - 20.0f).reduced (6.0f, 0.0f);
    juce::Path arrow;
    arrow.addTriangle (arrowZone.getX(), bounds.getCentreY() - 2.5f, arrowZone.getRight(), bounds.getCentreY() - 2.5f,
                       arrowZone.getCentreX(), bounds.getCentreY() + 3.0f);
    g.setColour (box.findColour (juce::ComboBox::arrowColourId));
    g.fillPath (arrow);
}

juce::Font AmpLookAndFeel::getComboBoxFont (juce::ComboBox& box)
{
    if (isBadge (box))
        return juce::Font (Fonts::display ((float) box.getHeight() * 0.78f).withKerningFactor (0.06f));

    return juce::Font (Fonts::display (juce::jmin (15.0f, (float) box.getHeight() * 0.6f)));
}

void AmpLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (8, 1, box.getWidth() - 26, box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (isBadge (box) ? juce::Justification::centred : juce::Justification::centredLeft);
}

void AmpLookAndFeel::drawBubble (juce::Graphics& g, juce::BubbleComponent& bubble, const Point<float>&,
                                 const Rectangle<float>& body)
{
    g.setColour (bubble.findColour (juce::BubbleComponent::backgroundColourId));
    g.fillRoundedRectangle (body, 4.0f);
    g.setColour (bubble.findColour (juce::BubbleComponent::outlineColourId));
    g.drawRoundedRectangle (body.reduced (0.5f), 4.0f, 1.0f);
}

juce::Font AmpLookAndFeel::getSliderPopupFont (juce::Slider&)
{
    return juce::Font (Fonts::display (14.0f));
}

int AmpLookAndFeel::getSliderPopupPlacement (juce::Slider&)
{
    return juce::BubbleComponent::above;
}

void AmpLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    g.fillAll (findColour (juce::PopupMenu::backgroundColourId));
    g.setColour (Palette::gold.withAlpha (0.3f));
    g.drawRect (0, 0, width, height);
}

void Draw::led (juce::Graphics& g, Point<float> centre, float radius, Colour colour, bool lit)
{
    if (lit)
    {
        g.setGradientFill (ColourGradient (colour.withAlpha (0.5f), centre.x, centre.y,
                                           colour.withAlpha (0.0f), centre.x + radius * 4.0f, centre.y, true));
        g.fillEllipse (circle (centre, radius * 4.0f));
        g.setGradientFill (ColourGradient (colour.brighter (0.9f), centre.x, centre.y, colour, centre.x + radius, centre.y, true));
    }
    else
    {
        g.setGradientFill (ColourGradient (colour.withMultipliedBrightness (0.45f), centre.x, centre.y - radius,
                                           colour.withMultipliedBrightness (0.15f), centre.x, centre.y + radius, false));
    }

    g.fillEllipse (circle (centre, radius));
    g.setColour (juce::Colours::black.withAlpha (0.6f));
    g.drawEllipse (circle (centre, radius + 0.5f), 1.0f);
    g.setColour (juce::Colours::white.withAlpha (lit ? 0.75f : 0.2f));
    g.fillEllipse (circle (centre.translated (-radius * 0.35f, -radius * 0.35f), radius * 0.3f));
}

void Draw::screen (juce::Graphics& g, Rectangle<float> area, float cornerSize)
{
    g.setColour (Palette::screen);
    g.fillRoundedRectangle (area, cornerSize);

    g.saveState();
    juce::Path clip;
    clip.addRoundedRectangle (area, cornerSize);
    g.reduceClipRegion (clip);

    g.setGradientFill (ColourGradient (juce::Colours::black.withAlpha (0.7f), 0.0f, area.getY(),
                                       juce::Colours::transparentBlack, 0.0f, area.getY() + juce::jmin (12.0f, area.getHeight() * 0.3f), false));
    g.fillRect (area);
    g.setGradientFill (ColourGradient (juce::Colours::white.withAlpha (0.045f), area.getX(), area.getY(),
                                       juce::Colours::transparentWhite, area.getX() + area.getWidth() * 0.6f, area.getBottom(), false));
    g.fillRect (area.withHeight (area.getHeight() * 0.5f));
    g.restoreState();

    g.setColour (juce::Colours::white.withAlpha (0.07f));
    g.drawLine (area.getX() + cornerSize, area.getBottom() - 0.5f, area.getRight() - cornerSize, area.getBottom() - 0.5f, 1.0f);
}

void Draw::screw (juce::Graphics& g, Point<float> centre, float radius)
{
    g.setGradientFill (ColourGradient (Colour (0xffcfd2d8), centre.x - radius, centre.y - radius,
                                       Colour (0xff4c4f56), centre.x + radius, centre.y + radius, false));
    g.fillEllipse (circle (centre, radius));
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.drawEllipse (circle (centre, radius), 0.8f);
    g.drawLine (centre.x - radius * 0.6f, centre.y, centre.x + radius * 0.6f, centre.y, 1.2f);
    g.drawLine (centre.x, centre.y - radius * 0.6f, centre.x, centre.y + radius * 0.6f, 1.2f);
}

void Draw::brushedMetal (juce::Graphics& g, Rectangle<float> area, Colour base, float cornerSize)
{
    g.setGradientFill (ColourGradient (base.brighter (0.18f), 0.0f, area.getY(), base.darker (0.22f), 0.0f, area.getBottom(), false));
    g.fillRoundedRectangle (area, cornerSize);

    g.saveState();
    juce::Path clip;
    clip.addRoundedRectangle (area, cornerSize);
    g.reduceClipRegion (clip);

    juce::Random random (7);

    for (auto y = area.getY(); y < area.getBottom(); y += 1.0f)
    {
        const auto light = random.nextBool();
        g.setColour ((light ? juce::Colours::white : juce::Colours::black).withAlpha (random.nextFloat() * 0.05f));
        g.fillRect (area.getX(), y, area.getWidth(), 1.0f);
    }

    g.setGradientFill (ColourGradient (juce::Colours::white.withAlpha (0.12f), area.getX(), area.getY(),
                                       juce::Colours::transparentWhite, area.getX() + area.getWidth() * 0.5f, area.getY(), false));
    g.fillRect (area.withHeight (2.0f));
    g.restoreState();
}

Rectangle<float> Draw::unitChassis (juce::Graphics& g, Rectangle<float> bounds)
{
    const auto body = bounds.reduced (2.0f, 2.0f).withTrimmedBottom (4.0f);

    juce::Path outline;
    outline.addRoundedRectangle (body, 10.0f);
    juce::DropShadow (juce::Colours::black.withAlpha (0.7f), 16, { 0, 6 }).drawForPath (g, outline);
    brushedMetal (g, body, Colour (0xff202227), 10.0f);
    g.setColour (juce::Colours::black.withAlpha (0.7f));
    g.strokePath (outline, juce::PathStrokeType (1.5f));

    for (const auto corner : { body.getTopLeft().translated (9.0f, 9.0f), body.getTopRight().translated (-9.0f, 9.0f),
                               body.getBottomLeft().translated (9.0f, -9.0f), body.getBottomRight().translated (-9.0f, -9.0f) })
        screw (g, corner, 3.5f);

    return body;
}
