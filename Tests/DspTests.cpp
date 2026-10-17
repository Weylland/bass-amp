#include "../Source/PluginProcessor.h"
#include "../Source/ParameterIds.h"

#include <iostream>

namespace
{
    constexpr double testSampleRate = 48000.0;
    constexpr int blockSize = 64;
    int failures = 0;

    void check (bool condition, const juce::String& name, const juce::String& detail)
    {
        std::cout << (condition ? "PASS  " : "FAIL  ") << name << "  (" << detail << ")" << std::endl;

        if (! condition)
            ++failures;
    }

    void setParam (BassAmpProcessor& processor, const char* id, float value)
    {
        auto* parameter = processor.parameters.getParameter (id);
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (value));
    }

    using Signal = std::function<float (int)>;

    Signal sine (float frequency, float amplitude)
    {
        return [=] (int n) { return amplitude * (float) std::sin (juce::MathConstants<double>::twoPi * frequency * n / testSampleRate); };
    }

    // Bass-like tone: strong second harmonic, like a plucked string.
    Signal bassTone (float frequency, float amplitude)
    {
        return [=] (int n)
        {
            const auto phase = juce::MathConstants<double>::twoPi * frequency * n / testSampleRate;
            return amplitude * (float) (0.6 * std::sin (phase) + 0.8 * std::sin (2.0 * phase) + 0.3 * std::sin (3.0 * phase));
        };
    }

    struct RenderResult
    {
        float peak = 0.0f;
        bool finite = true;
    };

    RenderResult render (BassAmpProcessor& processor, const Signal& signal, double seconds, Tuner* outputTuner = nullptr)
    {
        juce::AudioBuffer<float> buffer (2, blockSize);
        juce::MidiBuffer midi;
        RenderResult result;
        static int sampleIndex = 0;

        for (int done = 0; done < (int) (seconds * testSampleRate); done += blockSize)
        {
            buffer.clear();

            for (int i = 0; i < blockSize; ++i)
                buffer.setSample (0, i, signal (sampleIndex++));

            processor.processBlock (buffer, midi);

            for (int i = 0; i < blockSize; ++i)
            {
                const auto value = buffer.getSample (0, i);
                result.finite = result.finite && std::isfinite (value);
                result.peak = juce::jmax (result.peak, std::abs (value));
            }

            if (outputTuner != nullptr)
                outputTuner->push (buffer.getReadPointer (0), blockSize);
        }

        return result;
    }

    std::unique_ptr<BassAmpProcessor> makeProcessor()
    {
        auto processor = std::make_unique<BassAmpProcessor>();
        processor->setPlayConfigDetails (1, 2, testSampleRate, blockSize);
        processor->prepareToPlay (testSampleRate, blockSize);
        return processor;
    }

    void testTuner()
    {
        for (const auto frequency : { 30.87f, 41.2f, 55.0f, 98.0f, 196.0f, 392.0f })
        {
            Tuner tuner;
            tuner.prepare (testSampleRate);
            std::vector<float> samples ((size_t) blockSize);
            const auto tone = bassTone (frequency, 0.3f);
            std::optional<float> detected;

            for (int block = 0; block < (int) (0.5 * testSampleRate) / blockSize; ++block)
            {
                for (int i = 0; i < blockSize; ++i)
                    samples[(size_t) i] = tone (block * blockSize + i);

                tuner.push (samples.data(), blockSize);

                if (block % 40 == 0)
                    detected = tuner.detectFrequency();
            }

            detected = tuner.detectFrequency();
            const auto cents = detected ? 1200.0f * std::log2 (*detected / frequency) : 9999.0f;
            check (detected.has_value() && std::abs (cents) < 3.0f, "tuner " + juce::String (frequency, 2) + " Hz",
                   detected ? juce::String (*detected, 2) + " Hz, " + juce::String (cents, 1) + " cents" : juce::String ("nothing"));
        }

        Tuner silent;
        silent.prepare (testSampleRate);
        std::vector<float> zeros ((size_t) 4096, 0.0f);
        silent.push (zeros.data(), (int) zeros.size());
        check (! silent.detectFrequency().has_value(), "tuner silence", "no pitch on silence");
    }

    void testGate()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::gateOn, 1.0f);
        setParam (*processor, ParamIds::gateThreshold, -60.0f);
        setParam (*processor, ParamIds::ampMaster, 0.0f);

        juce::Random random (42);
        const auto noise = render (*processor, [&random] (int) { return (random.nextFloat() * 2.0f - 1.0f) * 0.0003f; }, 1.0);
        const auto tail = render (*processor, [&random] (int) { return (random.nextFloat() * 2.0f - 1.0f) * 0.0003f; }, 0.5);
        check (tail.peak < 1.0e-5f, "gate closes on -70 dB noise", "peak " + juce::String (tail.peak, 8));
        juce::ignoreUnused (noise);

        const auto note = render (*processor, sine (41.2f, 0.3f), 0.5);
        check (note.peak > 0.05f, "gate opens on a note", "peak " + juce::String (note.peak, 4));
    }

    void testCompressor()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::compOn, 1.0f);
        setParam (*processor, ParamIds::compThreshold, -20.0f);
        setParam (*processor, ParamIds::compRatio, 4.0f);
        setParam (*processor, ParamIds::compAttack, 1.0f);

        render (*processor, sine (100.0f, 0.5f), 0.5);
        const auto reduction = processor->getCompressorGainReductionDb();
        check (reduction > 9.0f && reduction < 12.5f, "compressor 4:1 at -6 dBFS", juce::String (reduction, 2) + " dB of reduction");
    }

    void testOctaver()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::octOn, 1.0f);
        setParam (*processor, ParamIds::octDry, 0.0f);
        setParam (*processor, ParamIds::octSub, 1.0f);
        setParam (*processor, ParamIds::ampMaster, 0.0f);

        Tuner outputTuner;
        outputTuner.prepare (testSampleRate);
        render (*processor, sine (82.4f, 0.3f), 0.6, &outputTuner);
        const auto detected = outputTuner.detectFrequency();
        check (detected.has_value() && std::abs (*detected - 41.2f) < 1.0f, "octaver halves 82.4 Hz",
               detected ? juce::String (*detected, 2) + " Hz" : juce::String ("nothing"));
    }

    void testFullChainIsStable()
    {
        auto processor = makeProcessor();

        for (const auto* toggle : { ParamIds::gateOn, ParamIds::compOn, ParamIds::octOn, ParamIds::envOn, ParamIds::fuzzOn,
                                    ParamIds::distOn, ParamIds::chorusOn, ParamIds::delayOn, ParamIds::reverbOn })
            setParam (*processor, toggle, 1.0f);

        setParam (*processor, ParamIds::delayFeedback, 0.9f);
        setParam (*processor, ParamIds::envResonance, 8.0f);
        setParam (*processor, ParamIds::ampGain, 10.0f);

        const auto loud = render (*processor, bassTone (41.2f, 0.9f), 3.0);
        check (loud.finite && loud.peak < 8.0f, "everything on, max settings", "peak " + juce::String (loud.peak, 3));

        const auto silence = render (*processor, [] (int) { return 0.0f; }, 1.0);
        check (silence.finite, "tails stay finite", "peak " + juce::String (silence.peak, 4));
    }

    void testBypassIsTransparentEnough()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::ampMaster, 0.0f);
        setParam (*processor, ParamIds::ampGain, 0.0f);

        const auto result = render (*processor, sine (110.0f, 0.1f), 0.5);
        check (result.peak > 0.07f && result.peak < 0.13f, "all effects off: level preserved", "peak " + juce::String (result.peak, 4));
    }

    void testMeters()
    {
        auto processor = makeProcessor();
        processor->inputMeter.takePeak();
        render (*processor, sine (100.0f, 1.0f), 0.1);
        const auto inputPeak = processor->inputMeter.takePeak();
        check (inputPeak >= 0.98f, "input meter sees full scale", juce::String (inputPeak, 4));
        check (processor->inputMeter.takePeak() == 0.0f, "meter resets after read", "taken twice");
    }
}

// Renders the editor to a PNG so the layout can be checked without opening the app.
int saveEditorSnapshot (const juce::File& target)
{
    auto processor = makeProcessor();
    setParam (*processor, ParamIds::compOn, 1.0f);
    setParam (*processor, ParamIds::fuzzOn, 1.0f);
    setParam (*processor, ParamIds::reverbOn, 1.0f);
    processor->audioDeviceInfo = "ASIO  |  Focusrite USB ASIO  |  64 samples @ 48.0 kHz  |  latence ~4.2 ms";

    std::unique_ptr<juce::AudioProcessorEditor> editor (processor->createEditor());
    const auto image = editor->createComponentSnapshot (editor->getLocalBounds(), true, 1.0f);

    target.deleteFile();
    juce::FileOutputStream stream (target);
    return stream.openedOk() && juce::PNGImageFormat().writeImageToStream (image, stream) ? 0 : 1;
}

int main (int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    if (argc == 3 && juce::String (argv[1]) == "--snapshot")
        return saveEditorSnapshot (juce::File (argv[2]));

    testTuner();
    testGate();
    testCompressor();
    testOctaver();
    testFullChainIsStable();
    testBypassIsTransparentEnough();
    testMeters();

    std::cout << std::endl << (failures == 0 ? "All tests passed" : juce::String (failures) + " test(s) failed") << std::endl;
    return failures == 0 ? 0 : 1;
}
