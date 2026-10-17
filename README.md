# Bass Amp

Ampli de basse en logiciel (JUCE 8, C++17) : application autonome Windows + plugin VST3.

Chaîne : **Disto → Ampli (gain, EQ 3 bandes, baffle, master) → Chorus → Reverb**.

## Build

Prérequis : Visual Studio 2019 Build Tools (C++), CMake + Ninja (`pip install --user cmake ninja`).

```bat
git submodule update --init
build.bat
```

Sorties dans `build\BassAmp_artefacts\Release\` (`Standalone\Bass Amp.exe` et `VST3\Bass Amp.vst3`).

## Premier lancement

1. **Options → Audio/MIDI Settings** : choisir la carte son, l'entrée où la basse est branchée et la sortie.
2. Pour une latence minimale, prendre le type **Windows Audio (Exclusive Mode)** avec un buffer de 128 samples ou moins.
3. JUCE coupe l'entrée par défaut pour éviter le larsen : décocher **Mute audio input** (le réglage est mémorisé ensuite).

Double-clic sur un potard : retour à sa valeur par défaut.
