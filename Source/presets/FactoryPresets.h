#pragma once

#include <utility>
#include <vector>

struct FactoryPreset
{
    const char* name;
    std::vector<std::pair<const char*, float>> values;   // parameters not listed stay at their default
};

const std::vector<FactoryPreset>& getFactoryPresets();
