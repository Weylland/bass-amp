#include "../Source/PluginProcessor.h"
#include "../Source/ParameterIds.h"
#include "../Source/ui/Logo.h"
#include "../Source/ui/MainPanel.h"

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
        double sumOfSquares = 0.0;
        int numSamples = 0;
        bool finite = true;

        float rmsDb() const { return juce::Decibels::gainToDecibels ((float) std::sqrt (sumOfSquares / juce::jmax (1, numSamples)), -120.0f); }
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
                result.sumOfSquares += (double) value * value;
                ++result.numSamples;
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

        // Neutral baseline: every parameter at its default, clean amp, no cabinet.
        for (auto* parameter : processor->getParameters())
            if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (parameter))
                ranged->setValueNotifyingHost (ranged->getDefaultValue());

        setParam (*processor, ParamIds::ampModel, 0.0f);
        setParam (*processor, ParamIds::ampCab, 0.0f);

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

    // RMS of a sine through the processor, after the filters settle.
    float sineLevelDb (BassAmpProcessor& processor, float frequency)
    {
        render (processor, sine (frequency, 0.1f), 0.2);
        return render (processor, sine (frequency, 0.1f), 0.3).rmsDb();
    }

    void testFilterPedal()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::filterOn, 1.0f);
        setParam (*processor, ParamIds::filterCutoff, 200.0f);
        setParam (*processor, ParamIds::filterResonance, 0.0f);

        setParam (*processor, ParamIds::filterType, 1.0f);
        const auto lowPassAttenuation = sineLevelDb (*processor, 60.0f) - sineLevelDb (*processor, 3200.0f);
        setParam (*processor, ParamIds::filterType, 5.0f);
        const auto highPassAttenuation = sineLevelDb (*processor, 3200.0f) - sineLevelDb (*processor, 40.0f);

        check (lowPassAttenuation > 70.0f && highPassAttenuation > 25.0f, "filter pedal LP 24 and HP 24",
               juce::String (lowPassAttenuation, 1) + " / " + juce::String (highPassAttenuation, 1) + " dB");
    }

    void testEnvelopeSweepsDown()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::envOn, 1.0f);
        setParam (*processor, ParamIds::envSensitivity, 1.0f);
        const auto up = sineLevelDb (*processor, 2000.0f);
        setParam (*processor, ParamIds::envDirection, 1.0f);
        const auto down = sineLevelDb (*processor, 2000.0f);

        check (up - down > 12.0f, "envelope filter sweeps up or down", juce::String (up, 1) + " / " + juce::String (down, 1) + " dB at 2 kHz");
    }

    void testAmpCuts()
    {
        auto processor = makeProcessor();
        const auto flat = sineLevelDb (*processor, 50.0f) - sineLevelDb (*processor, 5000.0f);
        setParam (*processor, ParamIds::ampLowCut, 200.0f);
        setParam (*processor, ParamIds::ampHighCut, 1500.0f);
        const auto lowCut = sineLevelDb (*processor, 50.0f);
        const auto highCut = sineLevelDb (*processor, 6000.0f);
        const auto mid = sineLevelDb (*processor, 550.0f);

        check (std::abs (flat) < 1.5f && mid - lowCut > 18.0f && mid - highCut > 18.0f, "amp low and high cut",
               "flat " + juce::String (flat, 1) + " dB, cuts " + juce::String (mid - lowCut, 1) + " / " + juce::String (mid - highCut, 1) + " dB");
    }

    void testWah()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::wahOn, 1.0f);
        setParam (*processor, ParamIds::wahPedal, 0.0f);
        const auto heel = sineLevelDb (*processor, 1500.0f);
        setParam (*processor, ParamIds::wahPedal, 1.0f);
        const auto toe = sineLevelDb (*processor, 1500.0f);

        check (toe - heel > 9.0f, "wah sweeps with the pedal", "1.5 kHz: heel " + juce::String (heel, 1) + ", toe " + juce::String (toe, 1) + " dB");
    }

    // The frequency the output tuner hears for a played note.
    float detectedOutputFrequency (BassAmpProcessor& processor, Signal note)
    {
        Tuner outputTuner;
        outputTuner.prepare (testSampleRate);
        render (processor, note, 0.6, &outputTuner);
        return outputTuner.detectFrequency().value_or (0.0f);
    }

    void testPitchShifterAndSynth()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::pitchOn, 1.0f);
        setParam (*processor, ParamIds::pitchMix, 1.0f);
        setParam (*processor, ParamIds::pitchShift, 12.0f);
        const auto up = detectedOutputFrequency (*processor, sine (55.0f, 0.3f));
        setParam (*processor, ParamIds::pitchShift, -7.0f);
        const auto fifthDown = detectedOutputFrequency (*processor, sine (110.0f, 0.3f));

        check (std::abs (up - 110.0f) < 2.0f && std::abs (fifthDown - 73.42f) < 2.0f, "pitch shifter +12 and -7 semitones",
               juce::String (up, 1) + " Hz, " + juce::String (fifthDown, 1) + " Hz");

        auto synthProcessor = makeProcessor();
        setParam (*synthProcessor, ParamIds::synthOn, 1.0f);
        setParam (*synthProcessor, ParamIds::synthMix, 1.0f);
        const auto square = detectedOutputFrequency (*synthProcessor, bassTone (55.0f, 0.3f));
        setParam (*synthProcessor, ParamIds::synthWave, 1.0f);
        const auto saw = detectedOutputFrequency (*synthProcessor, bassTone (82.4f, 0.3f));
        const auto level = render (*synthProcessor, bassTone (82.4f, 0.3f), 0.3).rmsDb();

        check (std::abs (square - 55.0f) < 1.5f && std::abs (saw - 82.4f) < 1.5f && level > -40.0f, "synth tracks the note",
               "square " + juce::String (square, 1) + " Hz, saw " + juce::String (saw, 1) + " Hz, " + juce::String (level, 1) + " dB");
    }

    void testModulationEffects()
    {
        auto processor = makeProcessor();
        const auto dry = sineLevelDb (*processor, 440.0f);

        setParam (*processor, ParamIds::tremoloOn, 1.0f);
        setParam (*processor, ParamIds::tremoloDepth, 1.0f);
        const auto tremolo = sineLevelDb (*processor, 440.0f);
        setParam (*processor, ParamIds::tremoloOn, 0.0f);

        setParam (*processor, ParamIds::flangerOn, 1.0f);
        const auto flanger = render (*processor, sine (440.0f, 0.1f), 1.0);

        check (tremolo < dry - 2.0f && flanger.finite && std::abs (flanger.rmsDb() - dry) < 6.0f, "tremolo and flanger",
               "dry " + juce::String (dry, 1) + ", tremolo " + juce::String (tremolo, 1) + ", flanger " + juce::String (flanger.rmsDb(), 1) + " dB");
    }

    void testLimiter()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::ampMaster, 6.0f);
        setParam (*processor, ParamIds::ampModel, 0.0f);
        const auto limited = render (*processor, sine (100.0f, 1.0f), 0.5);
        setParam (*processor, ParamIds::limiterOn, 0.0f);
        const auto open = render (*processor, sine (100.0f, 1.0f), 0.5);

        check (limited.peak <= 0.8913f && open.peak > 1.0f, "output limiter holds -1 dBFS",
               "peak " + juce::String (limited.peak, 4) + " limited, " + juce::String (open.peak, 3) + " open");
    }

    void testPedalChain()
    {
        auto processor = makeProcessor();
        auto& chain = processor->pedalChain;
        auto order = [&chain] (Board board)
        {
            std::vector<PedalId> pedals;
            chain.forEach (board, [&] (PedalId pedal) { pedals.push_back (pedal); });
            return pedals;
        };

        const auto defaultShown = chain.getShown (Board::pre).size() == 6 && chain.getShown (Board::post).size() == 4;
        chain.show (PedalId::synth);
        const auto synthShown = chain.getShown (Board::pre)[3] == PedalId::synth && processor->parameters.getRawParameterValue (ParamIds::synthOn)->load() > 0.5f;
        chain.move (PedalId::drive, 0);
        chain.hide (PedalId::gate);
        const auto shown = chain.getShown (Board::pre);
        const auto audioOrder = order (Board::pre);
        const auto position = [&audioOrder] (PedalId pedal) { return std::find (audioOrder.begin(), audioOrder.end(), pedal) - audioOrder.begin(); };
        const auto movedFirst = ! shown.empty() && shown.front() == PedalId::drive && position (PedalId::drive) < position (PedalId::compressor);
        const auto gateHidden = std::find (shown.begin(), shown.end(), PedalId::gate) == shown.end() && audioOrder.size() == 10;

        setParam (*processor, ParamIds::wahOn, 1.0f);
        const auto wahShown = chain.getShown (Board::pre).size() == shown.size() + 1;

        processor->setCurrentProgram (0);
        const auto reset = chain.getShown (Board::pre).size() == 6 && order (Board::pre).front() == PedalId::gate;

        check (defaultShown && synthShown && movedFirst && gateHidden && wahShown && reset, "pedal chain show, move, hide, reset",
               juce::String ((int) defaultShown) + juce::String ((int) synthShown) + juce::String ((int) movedFirst)
                   + juce::String ((int) gateHidden) + juce::String ((int) wahShown) + juce::String ((int) reset));
    }

    void testMidiLearn()
    {
        auto processor = makeProcessor();
        auto& parameters = processor->parameters;
        const auto mapFile = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("BassAmpMidiMapTest.xml");
        mapFile.deleteFile();

        auto value = [&parameters] (const char* id) { return parameters.getRawParameterValue (id)->load(); };
        auto send = [] (MidiLearn& midi, const juce::MidiMessage& message)
        {
            juce::MidiBuffer buffer;
            buffer.addEvent (message, 0);
            midi.process (buffer);
        };

        {
            MidiLearn midi (parameters, mapFile);
            send (midi, juce::MidiMessage::controllerEvent (1, 11, 0));
            const auto expressionDefault = value (ParamIds::wahPedal) < 0.01f;

            midi.learn (ParamIds::filterCutoff);
            send (midi, juce::MidiMessage::noteOn (1, 40, 1.0f));
            const auto noteIgnoredForKnob = midi.getLearningParameter() == ParamIds::filterCutoff;
            send (midi, juce::MidiMessage::controllerEvent (1, 20, 127));
            const auto ccLearnt = midi.describe (ParamIds::filterCutoff) == "CC 20" && value (ParamIds::filterCutoff) > 7999.0f;

            midi.learn (ParamIds::filterOn);
            send (midi, juce::MidiMessage::noteOn (1, 36, 1.0f));
            const auto noteToggles = value (ParamIds::filterOn) > 0.5f;
            send (midi, juce::MidiMessage::noteOn (1, 36, 1.0f));
            const auto noteTogglesBack = value (ParamIds::filterOn) < 0.5f;

            midi.forget (ParamIds::wahPedal);

            check (expressionDefault && noteIgnoredForKnob && ccLearnt && noteToggles && noteTogglesBack, "MIDI learn: CC, note toggle, expression",
                   juce::String ((int) expressionDefault) + juce::String ((int) noteIgnoredForKnob) + juce::String ((int) ccLearnt)
                       + juce::String ((int) noteToggles) + juce::String ((int) noteTogglesBack));
        }

        MidiLearn reloaded (parameters, mapFile);
        send (reloaded, juce::MidiMessage::controllerEvent (1, 20, 0));
        check (reloaded.describe (ParamIds::filterCutoff) == "CC 20" && reloaded.describe (ParamIds::wahPedal).isEmpty()
                   && value (ParamIds::filterCutoff) < 41.0f,
               "MIDI map saved and reloaded", reloaded.describe (ParamIds::filterOn));
        mapFile.deleteFile();
    }

    void testFullChainIsStable()
    {
        auto processor = makeProcessor();

        for (const auto* toggle : { ParamIds::gateOn, ParamIds::compOn, ParamIds::octOn, ParamIds::envOn, ParamIds::fuzzOn,
                                    ParamIds::distOn, ParamIds::eqOn, ParamIds::phaserOn, ParamIds::chorusOn, ParamIds::delayOn,
                                    ParamIds::reverbOn, ParamIds::ampDeep, ParamIds::ampBright, ParamIds::filterOn,
                                    ParamIds::wahOn, ParamIds::synthOn, ParamIds::pitchOn, ParamIds::flangerOn,
                                    ParamIds::tremoloOn })
            setParam (*processor, toggle, 1.0f);

        setParam (*processor, ParamIds::octUp, 1.0f);
        setParam (*processor, ParamIds::ampContour, 1.0f);
        setParam (*processor, ParamIds::phaserFeedback, 0.9f);
        setParam (*processor, ParamIds::flangerFeedback, 0.9f);
        setParam (*processor, ParamIds::filterResonance, 1.0f);
        setParam (*processor, ParamIds::filterDepth, 1.0f);
        setParam (*processor, ParamIds::synthResonance, 1.0f);
        setParam (*processor, ParamIds::wahResonance, 12.0f);

        for (const auto* band : ParamIds::graphicEq)
            setParam (*processor, band, 12.0f);

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

    void testAmpModels()
    {
        auto quietest = 0.0f, loudest = -120.0f;

        for (int model = 0; model < (int) ampModels.size(); ++model)
        {
            if (ampModels[(size_t) model].neuralCapture)
                continue;

            for (const auto gainValue : { 2.0f, 5.0f, 8.0f })
            {
                auto processor = makeProcessor();
                setParam (*processor, ParamIds::ampModel, (float) model);
                setParam (*processor, ParamIds::ampCab, 1.0f);
                setParam (*processor, ParamIds::ampBlend, 0.0f);
                setParam (*processor, ParamIds::ampMaster, 0.0f);
                setParam (*processor, ParamIds::ampGain, gainValue);

                render (*processor, bassTone (55.0f, 0.2f), 0.5);
                const auto result = render (*processor, bassTone (55.0f, 0.2f), 1.0);

                if (juce::exactlyEqual (gainValue, 5.0f))
                {
                    quietest = juce::jmin (quietest, result.rmsDb());
                    loudest = juce::jmax (loudest, result.rmsDb());
                }

                check (result.finite && result.rmsDb() > -30.0f && result.peak < 1.5f,
                       juce::String ("amp ") + ampModels[(size_t) model].name + " gain " + juce::String (gainValue, 0),
                       "rms " + juce::String (result.rmsDb(), 1) + " dB, peak " + juce::String (result.peak, 3));
            }
        }

        check (loudest - quietest < 6.0f, "amp models are level-matched at gain 5", "spread " + juce::String (loudest - quietest, 1) + " dB");
    }

    // Harmonic distortion of a pure 110 Hz sine through an amp model at gain 5, cabinet off.
    float measureDistortion (int model)
    {
        constexpr int fftOrder = 15;
        constexpr int fftSize = 1 << fftOrder;
        constexpr int fundamentalBin = 75;
        const auto frequency = (float) (testSampleRate * fundamentalBin / fftSize);

        auto processor = makeProcessor();
        setParam (*processor, ParamIds::ampModel, (float) model);
        setParam (*processor, ParamIds::ampMaster, 0.0f);
        setParam (*processor, ParamIds::ampGain, 5.0f);
        render (*processor, sine (frequency, 0.2f), 0.5);

        std::vector<float> samples ((size_t) fftSize * 2, 0.0f);
        juce::AudioBuffer<float> buffer (2, blockSize);
        juce::MidiBuffer midi;
        auto index = 0;

        while (index < fftSize)
        {
            for (int i = 0; i < blockSize; ++i)
                buffer.setSample (0, i, sine (frequency, 0.2f) (index + i));

            processor->processBlock (buffer, midi);
            std::copy_n (buffer.getReadPointer (0), blockSize, samples.begin() + index);
            index += blockSize;
        }

        juce::dsp::WindowingFunction<float> window ((size_t) fftSize, juce::dsp::WindowingFunction<float>::hann, false);
        window.multiplyWithWindowingTable (samples.data(), (size_t) fftSize);
        juce::dsp::FFT (fftOrder).performFrequencyOnlyForwardTransform (samples.data());

        auto bandEnergy = [&samples] (int bin)
        {
            auto sum = 0.0f;

            for (int b = bin - 2; b <= bin + 2; ++b)
                sum += samples[(size_t) b] * samples[(size_t) b];

            return sum;
        };

        auto harmonics = 0.0f;

        for (int h = 2; h <= 10; ++h)
            harmonics += bandEnergy (fundamentalBin * h);

        return 100.0f * std::sqrt (harmonics / bandEnergy (fundamentalBin));
    }

    void testNamCapture()
    {
        const auto models = juce::File (BASS_AMP_NAM_MODELS);
        const auto captureIndex = (float) ampModels.size() - 1.0f;
        auto renderWith = [&] (const juce::File& capture)
        {
            auto processor = makeProcessor();
            setParam (*processor, ParamIds::ampModel, captureIndex);
            setParam (*processor, ParamIds::ampCab, 1.0f);
            setParam (*processor, ParamIds::ampBlend, 0.0f);
            setParam (*processor, ParamIds::ampMaster, 0.0f);
            processor->setCapture (capture);
            const auto rate = processor->getCaptureSampleRate();
            render (*processor, bassTone (55.0f, 0.2f), 0.5);
            return std::pair { render (*processor, bassTone (55.0f, 0.2f), 1.0), rate };
        };

        const auto [captured, rate] = renderWith (models.getChildFile ("A2.nam"));
        const auto [lstm, lstmRate] = renderWith (models.getChildFile ("lstm.nam"));
        const auto [missing, missingRate] = renderWith (models.getChildFile ("missing.nam"));

        auto reference = makeProcessor();
        setParam (*reference, ParamIds::ampModel, 1.0f);
        setParam (*reference, ParamIds::ampCab, 1.0f);
        setParam (*reference, ParamIds::ampBlend, 0.0f);
        setParam (*reference, ParamIds::ampMaster, 0.0f);
        render (*reference, bassTone (55.0f, 0.2f), 0.5);
        const auto tube = render (*reference, bassTone (55.0f, 0.2f), 1.0);

        check (captured.finite && lstm.finite && juce::exactlyEqual (rate, 48000.0) && juce::exactlyEqual (lstmRate, 48000.0)
                   && captured.rmsDb() > -40.0f && lstm.rmsDb() > -40.0f && std::abs (captured.rmsDb() - missing.rmsDb()) > 1.0f
                   && missing.finite && juce::exactlyEqual (missingRate, 0.0) && missing.rmsDb() > -40.0f,
               "NAM captures load and play, a missing file passes the signal",
               "A2 WaveNet " + juce::String (captured.rmsDb(), 1) + " dB at " + juce::String (rate / 1000.0, 1) + " kHz, LSTM "
                   + juce::String (lstm.rmsDb(), 1) + " dB, Tube 300 " + juce::String (tube.rmsDb(), 1) + " dB, no file "
                   + juce::String (missing.rmsDb(), 1) + " dB");
    }

    void testAmpModelCharacter()
    {
        enum { hiFi, tube300, crossover, tweed, flipTop, solidPunch, tubeDi };
        std::array<float, 7> thd {};
        juce::String detail;

        for (int model = 0; model < (int) thd.size(); ++model)
        {
            thd[(size_t) model] = measureDistortion (model);
            detail << ampModels[(size_t) model].name << " " << juce::String (thd[(size_t) model], 1) << " %  ";
        }

        // Crossover keeps the fundamental clean under its crossover, so it only has to beat Hi-Fi.
        check (thd[hiFi] < 3.0f && thd[solidPunch] < 6.0f && thd[tube300] > 3.0f && thd[tube300] < 15.0f
                   && thd[crossover] > thd[hiFi] && thd[tweed] > thd[tube300] && thd[flipTop] > thd[hiFi]
                   && thd[tubeDi] > thd[hiFi],
               "amp models: clean heads stay clean, tube heads break up", detail.trim());
    }

    // Level of a full-scale sine through a passive tone stack, in dB.
    float toneStackLevel (PassiveToneStack& stack, float frequency)
    {
        stack.prepare (testSampleRate);
        auto sumOfSquares = 0.0;
        const auto tone = sine (frequency, 1.0f);
        const auto numSamples = (int) testSampleRate;

        for (int n = 0; n < numSamples; ++n)
        {
            const auto y = stack.processSample (tone (n));

            if (n >= numSamples / 2)
                sumOfSquares += (double) y * y;
        }

        return juce::Decibels::gainToDecibels ((float) std::sqrt (2.0 * sumOfSquares / (numSamples / 2)), -120.0f);
    }

    void testPassiveToneStack()
    {
        PassiveToneStack stack;
        stack.setCircuit (*ampModels[3].passiveToneStack);
        stack.prepare (testSampleRate);
        stack.setControls (0.5, 0.5, 0.5);

        const auto low = toneStackLevel (stack, 80.0f);
        const auto middle = toneStackLevel (stack, 500.0f);
        const auto high = toneStackLevel (stack, 4000.0f);
        check (middle < low - 3.0f && middle < high - 3.0f && juce::jmax (low, high) < 0.5f,
               "tweed tone stack scoops the mids at noon",
               juce::String (low, 1) + " / " + juce::String (middle, 1) + " / " + juce::String (high, 1) + " dB");

        stack.setControls (0.5, 0.5, 1.0);
        const auto bassUp = toneStackLevel (stack, 80.0f);
        stack.setControls (0.5, 1.0, 0.5);
        const auto middleUp = toneStackLevel (stack, 500.0f);
        stack.setControls (1.0, 0.5, 0.5);
        const auto trebleUp = toneStackLevel (stack, 4000.0f);
        check (bassUp > low + 1.0f && middleUp > middle + 2.0f && trebleUp > high + 1.0f, "tweed tone stack controls",
               "bass +" + juce::String (bassUp - low, 1) + ", mid +" + juce::String (middleUp - middle, 1)
                   + ", treble +" + juce::String (trebleUp - high, 1) + " dB");

        auto stable = true;

        for (const auto t : { 0.0, 1.0 })
            for (const auto m : { 0.0, 1.0 })
                for (const auto b : { 0.0, 1.0 })
                {
                    stack.setControls (t, m, b);
                    const auto level = toneStackLevel (stack, 60.0f);
                    stable = stable && std::isfinite (level) && level < 12.0f;
                }

        check (stable, "tweed tone stack is stable at every extreme", "8 corners");
    }

    float renderedLevel (BassAmpProcessor& processor, float frequency)
    {
        render (processor, sine (frequency, 0.05f), 0.2);
        return render (processor, sine (frequency, 0.05f), 0.3).rmsDb();
    }

    void testEqualizer()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::ampMaster, 0.0f);
        setParam (*processor, ParamIds::ampGain, 0.0f);
        const auto reference = renderedLevel (*processor, 250.0f);
        const auto reference50 = renderedLevel (*processor, 50.0f);
        const auto reference2k = renderedLevel (*processor, 2000.0f);

        setParam (*processor, ParamIds::eqOn, 1.0f);
        const auto flat = renderedLevel (*processor, 250.0f);
        check (std::abs (flat - reference) < 0.3f, "graphic EQ flat is transparent", juce::String (flat - reference, 2) + " dB");

        setParam (*processor, ParamIds::graphicEq[3], 12.0f);
        const auto boosted = renderedLevel (*processor, 250.0f);
        const auto neighbour = renderedLevel (*processor, 2000.0f) - reference2k;
        check (boosted - reference > 10.0f && std::abs (neighbour) < 1.0f, "graphic EQ 250 Hz +12 dB",
               "+" + juce::String (boosted - reference, 1) + " dB at 250 Hz, " + juce::String (neighbour, 1) + " dB at 2 kHz");

        setParam (*processor, ParamIds::eqMode, 1.0f);
        const auto advancedFlat = renderedLevel (*processor, 250.0f);
        setParam (*processor, ParamIds::peqMid1Freq, 250.0f);
        setParam (*processor, ParamIds::peqMid1Gain, -12.0f);
        const auto cut = renderedLevel (*processor, 250.0f);
        setParam (*processor, ParamIds::peqLowCut, 200.0f);
        const auto lowCut = renderedLevel (*processor, 50.0f);
        check (std::abs (advancedFlat - reference) < 0.3f && reference - cut > 10.0f && reference50 - lowCut > 15.0f,
               "advanced EQ: flat, mid cut and low cut",
               juce::String (advancedFlat - reference, 2) + " / " + juce::String (cut - reference, 1) + " / "
                   + juce::String (lowCut - reference50, 1) + " dB");

        setParam (*processor, ParamIds::eqOn, 0.0f);
        const auto bypassed = renderedLevel (*processor, 250.0f);
        check (std::abs (bypassed - reference) < 0.3f, "EQ off is bypassed", juce::String (bypassed - reference, 2) + " dB");
    }

    void testCompressorMix()
    {
        auto processor = makeProcessor();
        setParam (*processor, ParamIds::ampMaster, 0.0f);
        const auto dry = render (*processor, sine (100.0f, 0.5f), 0.5).rmsDb();

        setParam (*processor, ParamIds::compOn, 1.0f);
        setParam (*processor, ParamIds::compMakeup, 0.0f);
        setParam (*processor, ParamIds::compMix, 0.0f);
        render (*processor, sine (100.0f, 0.5f), 0.2);
        const auto parallelDry = render (*processor, sine (100.0f, 0.5f), 0.5).rmsDb();

        setParam (*processor, ParamIds::compMix, 1.0f);
        render (*processor, sine (100.0f, 0.5f), 0.2);
        const auto compressed = render (*processor, sine (100.0f, 0.5f), 0.5).rmsDb();
        check (std::abs (parallelDry - dry) < 0.2f && dry - compressed > 6.0f, "compressor mix 0 % is dry, 100 % compressed",
               juce::String (parallelDry - dry, 2) + " / " + juce::String (compressed - dry, 1) + " dB");
    }

    void testImpulseResponseCabinet()
    {
        // A smoothing impulse response, which acts as a low-pass filter.
        const auto file = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("BassAmpTestIR.wav");
        file.deleteFile();

        {
            juce::AudioBuffer<float> impulse (1, 2400);

            for (int n = 0; n < impulse.getNumSamples(); ++n)
                impulse.setSample (0, n, std::exp (-(float) n / 80.0f));

            auto stream = std::make_unique<juce::FileOutputStream> (file);
            std::unique_ptr<juce::AudioFormatWriter> writer (juce::WavAudioFormat().createWriterFor (stream.get(), testSampleRate, 1, 24, {}, 0));
            stream.release();
            writer->writeFromAudioSampleBuffer (impulse, 0, impulse.getNumSamples());
        }

        auto processor = makeProcessor();
        setParam (*processor, ParamIds::ampMaster, 0.0f);
        setParam (*processor, ParamIds::ampBlend, 0.0f);
        setParam (*processor, ParamIds::ampCab, (float) (cabModels.size() - 1));
        const auto withoutFile = renderedLevel (*processor, 3000.0f);

        processor->setCabinetImpulseResponse (file);
        auto level = withoutFile;

        for (int attempt = 0; attempt < 100 && withoutFile - level < 10.0f; ++attempt)
        {
            juce::Thread::sleep (20);
            level = renderedLevel (*processor, 3000.0f);
        }

        const auto bass = renderedLevel (*processor, 41.2f);
        check (withoutFile - level > 10.0f && bass > -45.0f && processor->getCabinetImpulseResponse() == file,
               "impulse response cabinet loads and filters",
               "3 kHz " + juce::String (level - withoutFile, 1) + " dB, 41 Hz at " + juce::String (bass, 1) + " dB");

        file.deleteFile();
    }

    void testCabinetsAndBlend()
    {
        for (int cab = 0; cab < (int) cabModels.size(); ++cab)
        {
            auto processor = makeProcessor();
            setParam (*processor, ParamIds::ampModel, 1.0f);
            setParam (*processor, ParamIds::ampCab, (float) cab);
            setParam (*processor, ParamIds::ampBlend, 0.3f);
            setParam (*processor, ParamIds::ampMaster, 0.0f);

            const auto result = render (*processor, bassTone (41.2f, 0.3f), 1.0);
            check (result.finite && result.rmsDb() > -30.0f && result.peak < 1.5f, juce::String ("cabinet ") + cabModels[(size_t) cab].name,
                   "rms " + juce::String (result.rmsDb(), 1) + " dB, peak " + juce::String (result.peak, 3));
        }
    }

    void testFactoryPresets()
    {
        auto processor = makeProcessor();
        check (processor->getNumPrograms() >= 10, "factory presets exposed as programs", juce::String (processor->getNumPrograms()));
        auto quietest = 0.0f, loudest = -120.0f;

        for (int i = 0; i < processor->getNumPrograms(); ++i)
        {
            processor->setCurrentProgram (i);
            processor->prepareToPlay (testSampleRate, blockSize);

            render (*processor, bassTone (41.2f, 0.3f), 0.5);
            const auto result = render (*processor, bassTone (41.2f, 0.3f), 1.5);
            const auto name = processor->getProgramName (i);
            quietest = juce::jmin (quietest, result.rmsDb());
            loudest = juce::jmax (loudest, result.rmsDb());

            check (result.finite && result.peak < 1.2f && result.rmsDb() > -30.0f
                       && processor->presets.getCurrentPresetName() == name,
                   "preset " + name, "rms " + juce::String (result.rmsDb(), 1) + " dB, peak " + juce::String (result.peak, 3));
        }

        check (loudest - quietest < 3.0f, "presets are level-matched", "spread " + juce::String (loudest - quietest, 1) + " dB");
    }

    void testUserPresetRoundTrip()
    {
        auto processor = makeProcessor();
        const auto directory = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("BassAmpPresetTest");
        directory.deleteRecursively();
        PresetManager presets (processor->parameters, directory);

        setParam (*processor, ParamIds::ampGain, 7.3f);
        setParam (*processor, ParamIds::reverbOn, 1.0f);
        const auto file = presets.saveUserPreset ("My Sound");

        setParam (*processor, ParamIds::ampGain, 1.0f);
        setParam (*processor, ParamIds::reverbOn, 0.0f);
        const auto loaded = presets.loadUserPreset (file);

        const auto gainValue = processor->parameters.getRawParameterValue (ParamIds::ampGain)->load();
        const auto reverbValue = processor->parameters.getRawParameterValue (ParamIds::reverbOn)->load();
        check (loaded && std::abs (gainValue - 7.3f) < 0.01f && reverbValue > 0.5f
                   && presets.getUserPresets().size() == 1 && presets.getCurrentPresetName() == "My Sound",
               "user preset save and load", file.getFileName() + ", gain " + juce::String (gainValue, 2));

        directory.deleteRecursively();
    }

    void testRecorder()
    {
        auto processor = makeProcessor();
        const auto file = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("BassAmpRecorderTest.wav");

        const auto started = processor->recorder.start (file);
        render (*processor, bassTone (55.0f, 0.3f), 1.0);
        const auto seconds = processor->recorder.getRecordedSeconds();
        processor->recorder.stop();

        juce::AudioFormatManager formats;
        formats.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader (formats.createReaderFor (file));

        auto peak = 0.0f;

        if (reader != nullptr)
        {
            juce::AudioBuffer<float> recorded ((int) reader->numChannels, (int) reader->lengthInSamples);
            reader->read (&recorded, 0, (int) reader->lengthInSamples, 0, true, true);
            peak = recorded.getMagnitude (0, recorded.getNumSamples());
        }

        const auto length = reader != nullptr ? (double) reader->lengthInSamples / reader->sampleRate : 0.0;
        check (started && reader != nullptr && reader->numChannels == 2 && reader->bitsPerSample == 24
                   && std::abs (length - 1.0) < 0.01 && peak > 0.05f && processor->recorder.getDroppedSamples() == 0,
               "recorder writes a 24-bit stereo WAV",
               juce::String (length, 3) + " s in file, " + juce::String (seconds, 3) + " s counted, peak " + juce::String (peak, 3));

        reader.reset();
        file.deleteFile();
    }

    void testLooper()
    {
        Looper looper;
        looper.prepare (testSampleRate, 2);
        juce::AudioBuffer<float> buffer (2, 480);
        auto runBlocks = [&] (float input, int blocks)
        {
            auto peak = 0.0f;

            for (int b = 0; b < blocks; ++b)
            {
                for (int channel = 0; channel < 2; ++channel)
                    juce::FloatVectorOperations::fill (buffer.getWritePointer (channel), input, buffer.getNumSamples());

                juce::dsp::AudioBlock<float> block (buffer);
                looper.process (block);
                peak = buffer.getSample (0, buffer.getNumSamples() - 1);
            }

            return peak;
        };

        looper.press (Looper::Command::main);
        const auto whileRecording = runBlocks (0.5f, 10);
        looper.press (Looper::Command::main);
        const auto played = runBlocks (0.0f, 20);
        const auto length = looper.getLengthSeconds();
        looper.press (Looper::Command::main);
        runBlocks (0.25f, 10);
        looper.press (Looper::Command::main);
        const auto layered = runBlocks (0.0f, 10);
        looper.press (Looper::Command::stop);
        const auto stopped = runBlocks (0.0f, 2);
        looper.press (Looper::Command::clear);
        runBlocks (0.0f, 1);

        check (std::abs (whileRecording - 0.5f) < 1.0e-6f && std::abs (played - 0.5f) < 1.0e-6f && std::abs (length - 0.1) < 1.0e-6
                   && std::abs (layered - 0.75f) < 1.0e-6f && stopped == 0.0f && looper.getState() == Looper::State::empty,
               "looper records, plays, overdubs, stops, clears",
               "loop " + juce::String (length, 3) + " s, played " + juce::String (played, 2) + ", layered " + juce::String (layered, 2));
    }

    void testMetronome()
    {
        Metronome metronome;
        metronome.prepare (testSampleRate);
        metronome.bpm.store (120.0f);
        juce::AudioBuffer<float> buffer (2, 480);
        auto clicks = 0;
        auto peak = 0.0f;
        auto silentFor = 1000;

        auto run = [&] (double seconds)
        {
            for (int b = 0; b < (int) (seconds * testSampleRate) / buffer.getNumSamples(); ++b)
            {
                buffer.clear();
                juce::dsp::AudioBlock<float> block (buffer);
                metronome.process (block);

                for (int i = 0; i < buffer.getNumSamples(); ++i)
                {
                    const auto sample = std::abs (buffer.getSample (1, i));
                    peak = juce::jmax (peak, sample);

                    if (sample > 0.001f && silentFor > 1000)
                        ++clicks;

                    silentFor = sample > 0.001f ? 0 : silentFor + 1;
                }
            }
        };

        run (0.5);
        const auto offClicks = clicks;
        metronome.enabled.store (true);
        run (1.99);
        const auto beat = metronome.getBeat();

        check (offClicks == 0 && clicks == 4 && beat == 3 && peak > 0.3f, "metronome clicks four beats in 2 s at 120 BPM",
               juce::String (clicks) + " clicks, beat " + juce::String (beat) + ", peak " + juce::String (peak, 2));
    }

    void testBackingTrack()
    {
        const auto file = juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile ("BassAmpBackingTest.wav");
        file.deleteFile();

        {
            juce::WavAudioFormat wav;
            std::unique_ptr<juce::AudioFormatWriter> writer (wav.createWriterFor (new juce::FileOutputStream (file), testSampleRate, 2, 24, {}, 0));
            juce::AudioBuffer<float> tone (2, (int) testSampleRate);

            for (int i = 0; i < tone.getNumSamples(); ++i)
                for (int channel = 0; channel < 2; ++channel)
                    tone.setSample (channel, i, sine (220.0f, 0.5f) (i));

            writer->writeFromAudioSampleBuffer (tone, 0, tone.getNumSamples());
        }

        BackingTrack track;
        track.volume.store (1.0f);
        track.prepare (testSampleRate, blockSize);
        const auto loaded = track.load (file);
        track.play();
        juce::Thread::sleep (300);

        auto peakOf = [&track] (int blocks)
        {
            auto peak = 0.0f;

            for (int b = 0; b < blocks; ++b)
            {
                track.render (blockSize);

                for (int i = 0; i < blockSize; ++i)
                    peak = juce::jmax (peak, std::abs (track.getMonoSample (i)));
            }

            return peak;
        };

        const auto playing = peakOf (200);
        track.stop();
        const auto stopped = peakOf (20);

        check (loaded && std::abs (track.getLengthSeconds() - 1.0) < 0.01 && std::abs (playing - 0.5f) < 0.01f && stopped == 0.0f,
               "backing track plays a WAV and stops",
               juce::String (track.getLengthSeconds(), 2) + " s, peak " + juce::String (playing, 3) + ", stopped " + juce::String (stopped, 3));

        file.deleteFile();
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
static int saveEditorSnapshot (const juce::File& target, int preset)
{
    auto processor = makeProcessor();
    processor->setCurrentProgram (preset);
    processor->audioDeviceInfo = "ASIO  |  64 smp @ 48.0 kHz  |  latence ~4.2 ms";

    std::unique_ptr<juce::AudioProcessorEditor> editor (processor->createEditor());
    editor->setSize (MainPanel::designWidth, MainPanel::designHeight);
    const auto image = editor->createComponentSnapshot (editor->getLocalBounds(), true, 1.0f);

    target.deleteFile();
    juce::FileOutputStream stream (target);
    return stream.openedOk() && juce::PNGImageFormat().writeImageToStream (image, stream) ? 0 : 1;
}

static int saveArtwork (const juce::File& directory)
{
    auto save = [&directory] (const juce::Image& image, const char* name)
    {
        const auto target = directory.getChildFile (name);
        target.deleteFile();
        juce::FileOutputStream stream (target);
        return stream.openedOk() && juce::PNGImageFormat().writeImageToStream (image, stream);
    };

    return directory.createDirectory() && save (Logo::createIcon (1024), "icon.png")
               && save (Logo::createSplash (JucePlugin_VersionString), "splash.png") ? 0 : 1;
}

int main (int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;

    if (argc >= 3 && juce::String (argv[1]) == "--snapshot")
        return saveEditorSnapshot (juce::File::getCurrentWorkingDirectory().getChildFile (argv[2]), argc > 3 ? juce::String (argv[3]).getIntValue() : 0);

    if (argc >= 3 && juce::String (argv[1]) == "--artwork")
        return saveArtwork (juce::File::getCurrentWorkingDirectory().getChildFile (argv[2]));

    testTuner();
    testGate();
    testCompressor();
    testOctaver();
    testFilterPedal();
    testEnvelopeSweepsDown();
    testAmpCuts();
    testWah();
    testPitchShifterAndSynth();
    testModulationEffects();
    testLimiter();
    testPedalChain();
    testMidiLearn();
    testFullChainIsStable();
    testBypassIsTransparentEnough();
    testAmpModels();
    testNamCapture();
    testAmpModelCharacter();
    testPassiveToneStack();
    testCabinetsAndBlend();
    testImpulseResponseCabinet();
    testEqualizer();
    testCompressorMix();
    testFactoryPresets();
    testUserPresetRoundTrip();
    testRecorder();
    testLooper();
    testMetronome();
    testBackingTrack();
    testMeters();

    std::cout << std::endl << (failures == 0 ? "All tests passed" : juce::String (failures) + " test(s) failed") << std::endl;
    return failures == 0 ? 0 : 1;
}
