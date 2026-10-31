#include "PluginEditor.h"

namespace
{
    constexpr double minScale = 0.6;
    constexpr double maxScale = 1.6;

    // The largest scale, up to 1, at which the window fits on the main screen.
    double initialScale()
    {
        const auto* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();

        if (display == nullptr)
            return 1.0;

        const auto area = display->userArea;
        return juce::jlimit (minScale, 1.0, juce::jmin ((area.getWidth() - 40.0) / MainPanel::designWidth,
                                                        (area.getHeight() - 90.0) / MainPanel::designHeight));
    }
}

BassAmpEditor::BassAmpEditor (BassAmpProcessor& audioProcessor)
    : AudioProcessorEditor (audioProcessor),
      standalone (audioProcessor.wrapperType == juce::AudioProcessor::wrapperType_Standalone)
{
    setLookAndFeel (&lookAndFeel);
    createPanel();

    setResizable (true, true);
    setWindowLimits (false);

    const auto scale = initialScale();
    setSize (juce::roundToInt (MainPanel::designWidth * scale), juce::roundToInt (MainPanel::designHeight * scale));
}

BassAmpEditor::~BassAmpEditor()
{
    if (isFullScreen())
        juce::Desktop::getInstance().setKioskModeComponent (nullptr);

    if (window != nullptr)
        window->removeKeyListener (this);

    setLookAndFeel (nullptr);
}

void BassAmpEditor::createPanel()
{
    panel = std::make_unique<MainPanel> (static_cast<BassAmpProcessor&> (processor));
    addAndMakeVisible (*panel);

    panel->setFullScreenAvailable (standalone);
    panel->onToggleFullScreen = [this] { setFullScreen (! isFullScreen()); };
    panel->onLanguageChanged = [safeThis = juce::Component::SafePointer<BassAmpEditor> (this)]
    {
        juce::MessageManager::callAsync ([safeThis]
        {
            if (safeThis != nullptr)
                safeThis->createPanel();
        });
    };

    resized();
}

void BassAmpEditor::setWindowLimits (bool fullScreen)
{
    const auto maxSize = fullScreen ? 100000.0 : maxScale;
    setResizeLimits (juce::roundToInt (MainPanel::designWidth * minScale), juce::roundToInt (MainPanel::designHeight * minScale),
                     juce::roundToInt (MainPanel::designWidth * maxSize), juce::roundToInt (MainPanel::designHeight * maxSize));
    getConstrainer()->setFixedAspectRatio (fullScreen ? 0.0 : (double) MainPanel::designWidth / MainPanel::designHeight);
}

void BassAmpEditor::parentHierarchyChanged()
{
    if (! standalone || window == getTopLevelComponent())
        return;

    if (window != nullptr)
        window->removeKeyListener (this);

    window = getTopLevelComponent();
    window->addKeyListener (this);
}

bool BassAmpEditor::keyPressed (const juce::KeyPress& key, juce::Component*)
{
    if (key == juce::KeyPress::F11Key)
        setFullScreen (! isFullScreen());
    else if (key == juce::KeyPress::escapeKey && isFullScreen())
        setFullScreen (false);
    else
        return false;

    return true;
}

bool BassAmpEditor::isFullScreen() const
{
    return window != nullptr && juce::Desktop::getInstance().getKioskModeComponent() == window.getComponent();
}

void BassAmpEditor::setFullScreen (bool shouldBeFullScreen)
{
    if (window == nullptr || window == this)
        return;

    setWindowLimits (shouldBeFullScreen);
    juce::Desktop::getInstance().setKioskModeComponent (shouldBeFullScreen ? window.getComponent() : nullptr, false);
}

void BassAmpEditor::paint (juce::Graphics& g)
{
    g.fillAll (Palette::background);
}

void BassAmpEditor::resized()
{
    if (panel == nullptr)
        return;

    const auto scale = juce::jmin ((float) getWidth() / (float) MainPanel::designWidth, (float) getHeight() / (float) MainPanel::designHeight);
    panel->setTransform (juce::AffineTransform::scale (scale).translated (((float) getWidth() - MainPanel::designWidth * scale) / 2.0f,
                                                                        ((float) getHeight() - MainPanel::designHeight * scale) / 2.0f));
}
