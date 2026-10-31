#pragma once

#include <juce_dsp/juce_dsp.h>

// One-button looper on the rig output: record, then play (that sets the loop length),
// overdub on top, back to play. Stop keeps the loop, clear erases it. Up to 60 seconds.
// The buttons post a command that the audio thread picks up at the next block.
class Looper
{
public:
    enum class State { empty, recording, playing, overdubbing, stopped };
    enum class Command { none, main, stop, clear };

    void prepare (double sampleRate, int numChannels)
    {
        loop.setSize (juce::jlimit (1, 2, numChannels), (int) (sampleRate * maxSeconds));
        loop.clear();
        length = position = 0;
        state.store (State::empty);
        rate = sampleRate;
    }

    // Message thread.
    void press (Command command)            { pending.store (command); }
    State getState() const                  { return state.load(); }
    float getProgress() const               { return progress.load(); }
    double getLengthSeconds() const         { return (double) loopLength.load() / rate; }

    std::atomic<float> level { 1.0f };

    // Audio thread: records the block and adds the loop to it.
    void process (juce::dsp::AudioBlock<float>& block)
    {
        apply (pending.exchange (Command::none));

        auto current = state.load();
        const auto numChannels = juce::jmin ((int) block.getNumChannels(), loop.getNumChannels());
        const auto gain = level.load();

        if (current == State::empty || current == State::stopped)
            return;

        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            for (int channel = 0; channel < numChannels; ++channel)
            {
                auto& sample = block.getChannelPointer ((size_t) channel)[i];
                auto* stored = loop.getWritePointer (channel, position);

                if (current == State::recording)
                {
                    *stored = sample;
                }
                else
                {
                    const auto played = *stored;

                    if (current == State::overdubbing)
                        *stored = played + sample;

                    sample += played * gain;
                }
            }

            if (current == State::recording)
            {
                if (++position == loop.getNumSamples())
                {
                    apply (Command::main);
                    current = state.load();
                }
            }
            else
            {
                position = (position + 1) % length;
            }
        }

        loopLength.store (current == State::recording ? position : length);
        progress.store (length > 0 && current != State::recording ? (float) position / (float) length : 0.0f);
    }

private:
    static constexpr double maxSeconds = 60.0;

    void apply (Command command)
    {
        const auto current = state.load();

        if (command == Command::clear)
        {
            length = position = 0;
            state.store (State::empty);
            loopLength.store (0);
        }
        else if (command == Command::main)
        {
            if (current == State::empty)
            {
                position = 0;
                state.store (State::recording);
            }
            else if (current == State::recording)
            {
                length = juce::jmax (1, position);
                position = 0;
                state.store (State::playing);
            }
            else if (current == State::playing)
            {
                state.store (State::overdubbing);
            }
            else if (current == State::overdubbing)
            {
                state.store (State::playing);
            }
            else if (current == State::stopped)
            {
                position = 0;
                state.store (State::playing);
            }
        }
        else if (command == Command::stop)
        {
            if (current == State::recording)
            {
                length = juce::jmax (1, position);
                position = 0;
                state.store (State::stopped);
            }
            else if (current == State::playing || current == State::overdubbing)
            {
                state.store (State::stopped);
            }
        }
    }

    juce::AudioBuffer<float> loop;
    int length = 0, position = 0;
    double rate = 44100.0;
    std::atomic<State> state { State::empty };
    std::atomic<Command> pending { Command::none };
    std::atomic<float> progress { 0.0f };
    std::atomic<int> loopLength { 0 };
};
