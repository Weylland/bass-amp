#pragma once

#include <juce_core/juce_core.h>

// Averages the last few taps into a beat length; a pause of two seconds starts over.
class TapTempo
{
public:
    // The beat length in milliseconds once there are at least two taps.
    std::optional<double> tap()
    {
        const auto now = juce::Time::getMillisecondCounterHiRes();

        if (! times.isEmpty() && now - times.getLast() > resetMs)
            times.clearQuick();

        times.add (now);

        if (times.size() > maxTaps)
            times.remove (0);

        if (times.size() < 2)
            return std::nullopt;

        return (times.getLast() - times.getFirst()) / (double) (times.size() - 1);
    }

private:
    static constexpr double resetMs = 2000.0;
    static constexpr int maxTaps = 4;

    juce::Array<double> times;
};
