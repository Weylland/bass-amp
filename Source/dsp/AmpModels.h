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
    bool tubeCircuit = false;    // 12AX7 stages around the tone stack and a 6550 push-pull, modelled from the circuit
};

using EqType = EqBand::Type;

// Each model follows the circuit of a classic bass head, with its voicing and gain range fitted to
// measurements of NAM captures of the real head.
// Italian Hi-Fi: Italian solid-state head, flat and fast (Markbass Little Mark).
// Super Valve: all-tube 300 W head, warm low-mids and rounded highs (Ampeg SVT), modelled from its
// circuit: two 12AX7 stages around the tone stack, then six 6550s in push-pull.
// Nordic Drive: crossover preamp, clean lows under a gritty top (Darkglass B7K).
// Tweed '59: 1959 tweed bass amp with its passive tone stack, early break-up (Fender Bassman 5F6-A).
// Flip Top '60s: small studio tube amp, round and dark, compresses early (Ampeg B-15).
// West Coast 800: solid-state rock head, mid scoop and bright 4 kHz bite (Gallien-Krueger 800RB).
// Stomp DI: tube-emulation preamp DI, focused and punchy (Tech 21 SansAmp Bass Driver).
// NAM Capture: a Neural Amp Modeler capture of a real amp, with the tone controls after it.
inline const std::array<AmpModel, 8> ampModels {{
    { "Italian Hi-Fi", Clipping::solidState, 0.0f, 18.0f, 0.03f, 0.0f, 0.0f, 0.5f, 0.0f, -4.7f,
      50.0f, 7000.0f, 0.7f, std::nullopt,
      {{ { EqType::lowShelf, 132.0f, 1.10f, 1.0f }, { EqType::peak, 424.0f, 0.69f, -1.3f },
         { EqType::peak, 3790.0f, 0.62f, 1.2f }, { EqType::lowPass, 14000.0f, 0.53f, 0.0f } }} },

    { "Super Valve", Clipping::tube, -6.0f, 34.0f, 0.25f, 1.5f, 0.0f, 2.0f, 0.6f, 0.7f,
      60.0f, 4000.0f, 0.9f, std::nullopt,
      {{ { EqType::highPass, 60.0f, 1.47f, 0.0f }, { EqType::peak, 181.0f, 0.32f, -5.7f },
         { EqType::highShelf, 3840.0f, 0.50f, 12.3f }, { EqType::peak, 45.0f, 1.80f, 3.3f } }}, false, true },

    { "Nordic Drive", Clipping::tube, 6.0f, 60.0f, 0.15f, 3.0f, 350.0f, 4.0f, 0.2f, -0.9f,
      80.0f, 5000.0f, 1.0f, std::nullopt,
      {{ { EqType::highPass, 133.0f, 0.30f, 0.0f }, { EqType::peak, 828.0f, 1.36f, -14.4f },
         { EqType::peak, 2990.0f, 0.90f, 8.7f }, { EqType::peak, 6600.0f, 1.26f, -14.6f } }} },

    { "Tweed '59", Clipping::tube, -4.0f, 26.0f, 0.3f, 2.0f, 0.0f, 2.2f, 0.8f, 6.6f,
      80.0f, 3000.0f, 0.8f, ToneStackCircuit { 250e3, 1e6, 25e3, 56e3, 250e-12, 20e-9, 20e-9 },
      {{ { EqType::highPass, 53.0f, 0.30f, 0.0f }, { EqType::peak, 273.0f, 0.96f, -5.9f },
         { EqType::peak, 709.0f, 0.44f, 7.0f }, { EqType::lowPass, 4740.0f, 1.12f, 0.0f } }} },

    { "Flip Top '60s", Clipping::tube, -8.0f, 20.0f, 0.35f, 1.0f, 0.0f, 2.5f, 0.9f, 3.1f,
      80.0f, 2500.0f, 0.7f, std::nullopt,
      {{ { EqType::highPass, 25.0f, 0.63f, 0.0f }, { EqType::peak, 225.0f, 0.83f, -4.6f },
         { EqType::highShelf, 873.0f, 0.78f, 2.2f }, { EqType::lowPass, 8360.0f, 0.89f, 0.0f } }} },

    { "West Coast 800", Clipping::solidState, 4.0f, 50.0f, 0.05f, 0.0f, 0.0f, 0.8f, 0.1f, 0.7f,
      60.0f, 4000.0f, 1.0f, std::nullopt,
      {{ { EqType::highPass, 30.0f, 0.82f, 0.0f }, { EqType::peak, 322.0f, 0.34f, -4.3f },
         { EqType::peak, 3390.0f, 0.37f, 3.8f }, { EqType::lowPass, 9220.0f, 0.95f, 0.0f } }} },

    { "Stomp DI", Clipping::solidState, 0.0f, 30.0f, 0.15f, 2.0f, 0.0f, 1.2f, 0.15f, -0.2f,
      80.0f, 3000.0f, 0.8f, std::nullopt,
      {{ { EqType::highPass, 62.0f, 1.16f, 0.0f }, { EqType::peak, 748.0f, 1.14f, -22.0f },
         { EqType::peak, 2090.0f, 1.18f, 6.9f }, { EqType::lowPass, 4410.0f, 1.88f, 0.0f } }} },

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

// Speaker cabinets as filter curves: box tuning, low resonance, cone colour and break-up roll-off,
// fitted to the average of several miked impulse responses where we had them (8x10, 4x10, 1x15).
// The last entry plays an impulse response file instead.
inline const std::array<CabModel, 7> cabModels {{
    { "Off (DI)", {} },

    { "8x10", {{ { EqType::highPass, 119.0f, 1.35f, 0.0f }, { EqType::peak, 65.0f, 1.10f, 11.4f },
                 { EqType::peak, 753.0f, 0.65f, -11.1f }, { EqType::peak, 1867.0f, 2.00f, -4.3f },
                 { EqType::lowPass, 5418.0f, 2.00f, 0.0f }, { EqType::lowPass, 2875.0f, 1.43f, 0.0f } }} },

    { "4x10", {{ { EqType::highPass, 137.0f, 1.20f, 0.0f }, { EqType::peak, 64.0f, 1.31f, 12.4f },
                 { EqType::peak, 1723.0f, 0.40f, -8.3f }, { EqType::peak, 1962.0f, 1.29f, 15.0f },
                 { EqType::lowPass, 5711.0f, 2.00f, 0.0f }, { EqType::lowPass, 5278.0f, 0.40f, 0.0f } }} },

    { "1x15", {{ { EqType::highPass, 108.0f, 1.51f, 0.0f }, { EqType::peak, 66.0f, 2.00f, 4.0f },
                 { EqType::peak, 2375.0f, 1.73f, -15.0f }, { EqType::peak, 2140.0f, 2.00f, 13.7f },
                 { EqType::lowPass, 2845.0f, 2.00f, 0.0f }, { EqType::lowPass, 2296.0f, 0.40f, 0.0f } }} },

    { "2x12", {{ { EqType::highPass, 60.0f, 0.8f, 0.0f }, { EqType::peak, 110.0f, 1.2f, 2.5f },
                 { EqType::peak, 800.0f, 1.0f, -2.0f }, { EqType::peak, 2200.0f, 1.5f, 3.0f },
                 { EqType::lowPass, 4800.0f, 0.8f, 0.0f }, { EqType::lowPass, 6000.0f, 0.7f, 0.0f } }} },

    { "2x10 Hi-Fi", {{ { EqType::highPass, 50.0f, 0.9f, 0.0f }, { EqType::peak, 130.0f, 1.1f, 2.0f },
                       { EqType::peak, 500.0f, 1.0f, -1.5f }, { EqType::peak, 3000.0f, 1.0f, 1.5f },
                       { EqType::lowPass, 12000.0f, 0.7f, 0.0f }, { EqType::none } }} },

    { "IR File", {}, true },
}};
