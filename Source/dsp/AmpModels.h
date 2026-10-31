#pragma once

#include <optional>

#include "EqBand.h"
#include "ToneStack.h"

enum class Clipping { tube, solidState };

struct AmpModel
{
    const char* name;
    Clipping clipping;
    float minDriveDb;     // preamp drive at Gain 0
    float maxDriveDb;     // preamp drive at Gain 10
    float bias;           // asymmetry: more bias, more even harmonics
    float secondStageGain;// 0 = single gain stage
    float crossoverHz;    // > 0: only the content above is driven, the lows stay clean
    float powerDrive;     // power amp saturation
    float sag;            // supply droop: compression that blooms with the attack
    float outputTrimDb;   // levels the models against each other
    float bassHz;         // active tone stack: bass shelf, treble shelf and mid width
    float trebleHz;
    float midQ;
    std::optional<ToneStackCircuit> passiveToneStack;  // replaces the active tone stack when set
    std::array<EqBand, 4> voicing;
    bool neuralCapture = false;  // preamp and power amp replaced by a NAM capture; Gain sets its input level
};

using EqType = EqBand::Type;

// Each model follows the circuit and voicing of a classic bass head.
// Italian Hi-Fi: Italian solid-state head, flat and fast (Markbass Little Mark).
// Super Valve: all-tube 300 W head, warm low-mids and rounded highs (Ampeg SVT).
// Nordic Drive: crossover preamp, clean lows under a gritty top (Darkglass B7K).
// Tweed '59: 1959 tweed bass amp with its passive tone stack, early break-up (Fender Bassman 5F6-A).
// Flip Top '60s: small studio tube amp, round and dark, compresses early (Ampeg B-15).
// West Coast 800: solid-state rock head, mid scoop and bright 4 kHz bite (Gallien-Krueger 800RB).
// Stomp DI: tube-emulation preamp DI, focused and punchy (Tech 21 SansAmp Bass Driver).
// NAM Capture: a Neural Amp Modeler capture of a real amp, with the tone controls after it.
inline const std::array<AmpModel, 8> ampModels {{
    { "Italian Hi-Fi", Clipping::solidState, 0.0f, 18.0f, 0.03f, 0.0f, 0.0f, 0.5f, 0.0f, -2.5f,
      50.0f, 7000.0f, 0.7f, std::nullopt,
      {{ { EqType::lowShelf, 45.0f, 0.7f, 1.0f }, { EqType::peak, 380.0f, 0.7f, -1.0f },
         { EqType::highShelf, 8000.0f, 0.7f, 1.0f }, { EqType::none } }} },

    { "Super Valve", Clipping::tube, -6.0f, 22.0f, 0.25f, 1.5f, 0.0f, 1.6f, 0.6f, 0.5f,
      60.0f, 4000.0f, 0.9f, std::nullopt,
      {{ { EqType::lowShelf, 90.0f, 0.7f, 3.0f }, { EqType::peak, 250.0f, 0.9f, 2.0f },
         { EqType::lowPass, 6500.0f, 0.6f, 0.0f }, { EqType::none } }} },

    { "Nordic Drive", Clipping::tube, 6.0f, 40.0f, 0.15f, 3.0f, 250.0f, 1.0f, 0.2f, 4.5f,
      80.0f, 5000.0f, 1.0f, std::nullopt,
      {{ { EqType::highPass, 35.0f, 0.7f, 0.0f }, { EqType::peak, 1200.0f, 1.0f, 4.0f },
         { EqType::peak, 3000.0f, 1.2f, 3.0f }, { EqType::none } }} },

    { "Tweed '59", Clipping::tube, -4.0f, 26.0f, 0.3f, 2.0f, 0.0f, 2.2f, 0.8f, 2.8f,
      80.0f, 3000.0f, 0.8f, ToneStackCircuit { 250e3, 1e6, 25e3, 56e3, 250e-12, 20e-9, 20e-9 },
      {{ { EqType::lowShelf, 100.0f, 0.7f, 2.0f }, { EqType::peak, 700.0f, 0.8f, 1.5f },
         { EqType::lowPass, 5500.0f, 0.7f, 0.0f }, { EqType::none } }} },

    { "Flip Top '60s", Clipping::tube, -8.0f, 20.0f, 0.35f, 1.0f, 0.0f, 2.5f, 0.9f, 1.7f,
      80.0f, 2500.0f, 0.7f, std::nullopt,
      {{ { EqType::peak, 110.0f, 0.9f, 3.0f }, { EqType::peak, 800.0f, 0.8f, -2.0f },
         { EqType::lowPass, 4500.0f, 0.7f, 0.0f }, { EqType::none } }} },

    { "West Coast 800", Clipping::solidState, 0.0f, 30.0f, 0.05f, 0.0f, 0.0f, 0.8f, 0.1f, -2.4f,
      60.0f, 4000.0f, 1.0f, std::nullopt,
      {{ { EqType::highPass, 40.0f, 0.7f, 0.0f }, { EqType::peak, 500.0f, 0.8f, -3.0f },
         { EqType::peak, 4000.0f, 1.0f, 3.0f }, { EqType::none } }} },

    { "Stomp DI", Clipping::solidState, 0.0f, 30.0f, 0.15f, 2.0f, 0.0f, 1.2f, 0.15f, 2.5f,
      80.0f, 3000.0f, 0.8f, std::nullopt,
      {{ { EqType::highPass, 30.0f, 0.7f, 0.0f }, { EqType::peak, 900.0f, 0.9f, 2.0f },
         { EqType::highShelf, 2500.0f, 0.7f, 2.0f }, { EqType::lowPass, 6000.0f, 0.7f, 0.0f } }} },

    { "NAM Capture", Clipping::solidState, -12.0f, 12.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      80.0f, 4000.0f, 0.8f, std::nullopt,
      {{ { EqType::none }, { EqType::none }, { EqType::none }, { EqType::none } }}, true },
}};

struct CabModel
{
    const char* name;
    std::array<EqBand, 6> bands;
    bool usesImpulseResponse = false;
};

// Speaker cabinets as filter curves: box tuning, low resonance, cone colour and break-up roll-off.
// The last entry plays an impulse response file instead.
inline const std::array<CabModel, 7> cabModels {{
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

    { "2x12", {{ { EqType::highPass, 60.0f, 0.8f, 0.0f }, { EqType::peak, 110.0f, 1.2f, 2.5f },
                 { EqType::peak, 800.0f, 1.0f, -2.0f }, { EqType::peak, 2200.0f, 1.5f, 3.0f },
                 { EqType::lowPass, 4800.0f, 0.8f, 0.0f }, { EqType::lowPass, 6000.0f, 0.7f, 0.0f } }} },

    { "2x10 Hi-Fi", {{ { EqType::highPass, 50.0f, 0.9f, 0.0f }, { EqType::peak, 130.0f, 1.1f, 2.0f },
                       { EqType::peak, 500.0f, 1.0f, -1.5f }, { EqType::peak, 3000.0f, 1.0f, 1.5f },
                       { EqType::lowPass, 12000.0f, 0.7f, 0.0f }, { EqType::none } }} },

    { "IR File", {}, true },
}};
