#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

#include "PluginProcessor.h"
#include "ui/AmpLookAndFeel.h"

namespace
{
    constexpr int preferredBufferSize = 64;
    constexpr auto lowLatencySetupKey = "lowLatencySetupDone";

    int closestBufferSize (juce::AudioIODevice& device)
    {
        const auto sizes = device.getAvailableBufferSizes();

        for (auto size : sizes)
            if (size >= preferredBufferSize)
                return size;

        return sizes.isEmpty() ? device.getDefaultBufferSize() : sizes.getLast();
    }

    juce::AudioIODeviceType* findType (juce::AudioDeviceManager& deviceManager, const juce::String& typeName)
    {
        for (auto* type : deviceManager.getAvailableDeviceTypes())
            if (type->getTypeName() == typeName)
                return type;

        return nullptr;
    }

    // Drivers matching the interface already in use come first; generic and virtual wrappers last.
    juce::StringArray rankAsioDrivers (const juce::StringArray& drivers, const juce::String& currentInputName)
    {
        auto score = [&currentInputName] (const juce::String& driver)
        {
            for (const auto* generic : { "ASIO4ALL", "FlexASIO", "Realtek", "Generic", "Ableton" })
                if (driver.containsIgnoreCase (generic))
                    return -1;

            auto matchingWords = 0;

            for (const auto& word : juce::StringArray::fromTokens (driver, false))
                if (word.length() >= 3 && ! word.equalsIgnoreCase ("ASIO") && currentInputName.containsIgnoreCase (word))
                    ++matchingWords;

            return matchingWords;
        };

        auto ranked = drivers;
        std::stable_sort (ranked.begin(), ranked.end(),
                          [&score] (const juce::String& a, const juce::String& b) { return score (a) > score (b); });
        return ranked;
    }

    bool tryOpen (juce::AudioDeviceManager& deviceManager, const juce::String& typeName,
                  const juce::String& inputName, const juce::String& outputName)
    {
        deviceManager.setCurrentAudioDeviceType (typeName, true);

        auto setup = deviceManager.getAudioDeviceSetup();
        setup.inputDeviceName = inputName;
        setup.outputDeviceName = outputName;
        setup.useDefaultInputChannels = true;
        setup.useDefaultOutputChannels = true;
        setup.bufferSize = preferredBufferSize;

        if (deviceManager.setAudioDeviceSetup (setup, true).isNotEmpty())
            return false;

        auto* device = deviceManager.getCurrentAudioDevice();

        if (device == nullptr || device->getActiveInputChannels().isZero() || device->getActiveOutputChannels().isZero())
            return false;

        setup = deviceManager.getAudioDeviceSetup();
        setup.bufferSize = closestBufferSize (*device);
        return deviceManager.setAudioDeviceSetup (setup, true).isEmpty();
    }

    juce::String keepIfAvailable (juce::AudioIODeviceType& type, const juce::String& name, bool isInput)
    {
        const auto names = type.getDeviceNames (isInput);
        return names.contains (name) ? name : names[type.getDefaultDeviceIndex (isInput)];
    }

    // ASIO on the interface already in use if a driver opens, else WASAPI exclusive on the same devices.
    void selectLowLatencyDevice (juce::AudioDeviceManager& deviceManager)
    {
        const auto previous = deviceManager.getAudioDeviceSetup();
        const auto previousType = deviceManager.getCurrentAudioDeviceType();

        if (auto* asio = findType (deviceManager, "ASIO"))
        {
            asio->scanForDevices();

            for (const auto& driver : rankAsioDrivers (asio->getDeviceNames(), previous.inputDeviceName))
                if (tryOpen (deviceManager, "ASIO", driver, driver))
                    return;
        }

        for (const juce::String typeName : { "Windows Audio (Exclusive Mode)", "Windows Audio (Low Latency Mode)" })
        {
            if (auto* type = findType (deviceManager, typeName))
            {
                type->scanForDevices();

                if (tryOpen (deviceManager, typeName,
                             keepIfAvailable (*type, previous.inputDeviceName, true),
                             keepIfAvailable (*type, previous.outputDeviceName, false)))
                    return;
            }
        }

        deviceManager.setCurrentAudioDeviceType (previousType, true);
        deviceManager.setAudioDeviceSetup (previous, true);
    }

    juce::String describe (juce::AudioIODevice& device)
    {
        const auto sampleRate = device.getCurrentSampleRate();
        const auto bufferSize = device.getCurrentBufferSizeSamples();
        auto latencySamples = device.getInputLatencyInSamples() + device.getOutputLatencyInSamples();

        if (latencySamples <= 0)
            latencySamples = 2 * bufferSize;

        return device.getTypeName()
             + "  |  " + juce::String (bufferSize) + " smp @ " + juce::String (sampleRate / 1000.0, 1) + " kHz"
             + "  |  latence ~" + juce::String (1000.0 * latencySamples / sampleRate, 1) + " ms";
    }
}

class BassAmpStandaloneApp final : public juce::JUCEApplication, private juce::Timer
{
public:
    BassAmpStandaloneApp()
    {
        juce::PropertiesFile::Options options;
        options.applicationName = juce::CharPointer_UTF8 (JucePlugin_Name);
        options.filenameSuffix = ".settings";
        options.osxLibrarySubFolder = "Application Support";
        appProperties.setStorageParameters (options);
    }

    const juce::String getApplicationName() override { return juce::CharPointer_UTF8 (JucePlugin_Name); }
    const juce::String getApplicationVersion() override { return JucePlugin_VersionString; }
    bool moreThanOneInstanceAllowed() override { return false; }
    void anotherInstanceStarted (const juce::String&) override {}

    void initialise (const juce::String&) override
    {
        auto* settings = appProperties.getUserSettings();
        const auto needsLowLatencySetup = ! settings->getBoolValue (lowLatencySetupKey, false);

        // The bass goes in through a DI, not a microphone: no feedback risk, so the input starts open.
        if (! settings->containsKey ("shouldMuteInput"))
            settings->setValue ("shouldMuteInput", false);

        juce::AudioDeviceManager::AudioDeviceSetup preferredSetup;
        preferredSetup.bufferSize = preferredBufferSize;

        auto holder = std::make_unique<juce::StandalonePluginHolder> (settings, false, juce::String(), &preferredSetup);
        mainWindow = std::make_unique<juce::StandaloneFilterWindow> (getApplicationName(), Palette::background, std::move (holder));

        if (needsLowLatencySetup)
        {
            selectLowLatencyDevice (mainWindow->getDeviceManager());
            settings->setValue (lowLatencySetupKey, true);
        }

        mainWindow->setVisible (true);
        startTimer (500);
    }

    void shutdown() override
    {
        stopTimer();
        mainWindow = nullptr;
        appProperties.saveIfNeeded();
    }

    void systemRequestedQuit() override
    {
        if (mainWindow != nullptr)
            mainWindow->pluginHolder->savePluginState();

        if (juce::ModalComponentManager::getInstance()->cancelAllModalComponents())
        {
            juce::Timer::callAfterDelay (100, []
            {
                if (auto* app = juce::JUCEApplicationBase::getInstance())
                    app->systemRequestedQuit();
            });
        }
        else
        {
            quit();
        }
    }

private:
    void timerCallback() override
    {
        auto* processor = dynamic_cast<BassAmpProcessor*> (mainWindow->getAudioProcessor());

        if (processor == nullptr)
            return;

        auto* device = mainWindow->getDeviceManager().getCurrentAudioDevice();
        processor->audioDeviceInfo = device != nullptr ? describe (*device) : juce::String ("Aucun peripherique audio : Options > Audio/MIDI Settings");
    }

    juce::ApplicationProperties appProperties;
    std::unique_ptr<juce::StandaloneFilterWindow> mainWindow;
};

juce::JUCEApplicationBase* juce_CreateApplication()
{
    return new BassAmpStandaloneApp();
}
