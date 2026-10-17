#pragma once

#include "EqBand.h"

struct AmpModel
{
    const char* name;
    float minDriveDb;     // preamp drive at Gain 0
    float maxDriveDb;     // preamp drive at Gain 10
    float bias;           // triode asymmetry: more bias, more even harmonics
    float secondStageGain;// 0 = single gain stage
    float crossoverHz;    // > 0: only the content above is driven, the lows stay clean
    float powerDrive;     // push-pull power amp saturation
    float sag;            // supply droop: compression that blooms with the attack
    float outputTrimDb;   // levels the models against each other
    std::array<EqBand, 3> voicing;
};

using EqType = EqBand::Type;

// Clean: modern solid-state head, flat and hi-fi.
// Vintage: all-tube head in the SVT tradition, warm low-mids and rounded highs.
// Modern: crossover preamp in the Darkglass tradition, clean lows under a gritty top.
inline const std::array<AmpModel, 3> ampModels {{
    { "Clean",   0.0f, 18.0f, 0.05f, 0.0f, 0.0f,   0.6f, 0.05f, -2.0f,
      {{ { EqType::lowShelf, 60.0f, 0.7f, 1.5f }, { EqType::peak, 800.0f, 0.6f, -1.0f }, { EqType::highShelf, 6000.0f, 0.7f, 1.5f } }} },

    { "Vintage", -6.0f, 22.0f, 0.25f, 1.5f, 0.0f,  1.6f, 0.6f,  1.5f,
      {{ { EqType::lowShelf, 90.0f, 0.7f, 3.0f }, { EqType::peak, 250.0f, 0.9f, 2.0f }, { EqType::lowPass, 6500.0f, 0.6f, 0.0f } }} },

    { "Modern",  6.0f, 40.0f, 0.15f, 3.0f, 250.0f, 1.0f, 0.2f,  4.5f,
      {{ { EqType::highPass, 35.0f, 0.7f, 0.0f }, { EqType::peak, 1200.0f, 1.0f, 4.0f }, { EqType::peak, 3000.0f, 1.2f, 3.0f } }} },
}};

struct CabModel
{
    const char* name;
    std::array<EqBand, 6> bands;
};

// Speaker cabinets as filter curves: box tuning, low resonance, cone colour and break-up roll-off.
inline const std::array<CabModel, 4> cabModels {{
    { "Off (DI)", {} },

    { "8x10", {{ { EqType::highPass, 45.0f, 0.9f, 0.0f }, { EqType::peak, 100.0f, 1.2f, 3.0f },
                 { EqType::peak, 450.0f, 1.0f, -3.0f }, { EqType::peak, 1500.0f, 1.5f, 2.0f },
                 { EqType::lowPass, 4200.0f, 0.8f, 0.0f }, { EqType::lowPass, 5500.0f, 0.7f, 0.0f } }} },

    { "4x10", {{ { EqType::highPass, 50.0f, 0.8f, 0.0f }, { EqType::peak, 120.0f, 1.1f, 2.0f },
                 { EqType::peak, 600.0f, 1.0f, -2.0f }, { EqType::peak, 2500.0f, 1.2f, 3.0f },
                 { EqType::lowPass, 5500.0f, 0.8f, 0.0f }, { EqType::lowPass, 7000.0f, 0.7f, 0.0f } }} },

    { "1x15", {{ { EqType::highPass, 38.0f, 0.9f, 0.0f }, { EqType::peak, 80.0f, 1.0f, 4.0f },
                 { EqType::peak, 1000.0f, 0.8f, -3.0f }, { EqType::none },
                 { EqType::lowPass, 3000.0f, 0.8f, 0.0f }, { EqType::lowPass, 3800.0f, 0.7f, 0.0f } }} },
}};
