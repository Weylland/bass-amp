# Bass Amp

Ampli de basse en logiciel (JUCE 8, C++17) : application autonome Windows + plugin VST3.

Chaîne : **Gate → Compresseur → Octaver → Filtre d'enveloppe → Fuzz → Disto → Ampli (gain, EQ 3 bandes, baffle, master) → Chorus → Delay → Reverb**.

En plus : accordeur (31 à 500 Hz, détection YIN), vumètres d'entrée et de sortie avec témoins CLIP, et réduction de gain du compresseur.

## Build

Prérequis : Visual Studio 2019 Build Tools (C++), CMake + Ninja (`pip install --user cmake ninja`).

```bat
git submodule update --init
build.bat
```

Sorties dans `build\BassAmp_artefacts\Release\` (`Standalone\Bass Amp.exe` et `VST3\Bass Amp.vst3`).

Tests DSP (accordeur, gate, compresseur, octaver, stabilité de la chaîne) : `test.bat`.
`test.bat --snapshot ui.png` exporte une capture de l'interface.

## Latence

- Le SDK ASIO de Steinberg (GPLv3) est dans `libs/asiosdk` : le support ASIO est compilé automatiquement.
- Au premier lancement, l'appli choisit le pilote ASIO de l'interface déjà utilisée (les pilotes génériques comme ASIO4ALL ou Realtek passent en dernier), avec un buffer de 64 samples. Sans ASIO, elle passe en WASAPI exclusif.
- En ASIO, l'entrée et la sortie sont sur la même interface : branche ton casque ou tes enceintes sur la carte son.
- La taille du buffer ASIO se règle dans le panneau du pilote (**Options → Audio/MIDI Settings → Control Panel**).
- La ligne en haut à droite affiche le pilote, le buffer et la latence aller-retour estimée.
- Aucun effet n'ajoute de latence : pas de look-ahead, oversampling en IIR à phase minimale, et l'accordeur analyse le signal hors du thread audio.

## Utilisation

- Double-clic sur un potard : retour à sa valeur par défaut.
- **IN CLIP** : l'entrée de la carte son sature, baisse le gain de l'interface. **OUT CLIP** : la sortie sature, baisse le Master.
- Clic sur le bloc LEVELS : efface les témoins CLIP.
- **MUTE** dans l'accordeur coupe la sortie pendant l'accordage.
