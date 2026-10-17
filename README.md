# Bass Amp

Ampli de basse en logiciel (JUCE 8, C++17) : application autonome Windows + plugin VST3.

Chaîne : **Gate → Compresseur → Octaver → Filtre d'enveloppe → Fuzz → Disto → Ampli → Baffle + DI → Chorus → Delay → Reverb**.

En plus : accordeur (31 à 500 Hz, détection YIN), vumètres d'entrée et de sortie avec témoins CLIP, et réduction de gain du compresseur.

## Ampli

Préampli à lampes (étages triode asymétriques), voicing du modèle, EQ (Bass, Mid avec fréquence réglable, Treble) et ampli de puissance push-pull avec sag. Tout l'ampli tourne en suréchantillonnage 2x.

| Modèle | Caractère |
|---|---|
| Clean | Tête transistor moderne, neutre et hi-fi |
| Vintage | Tête tout-lampes dans l'esprit SVT : bas-médiums chauds, aigus arrondis, compression naturelle (sag) |
| Modern | Préampli à crossover dans l'esprit Darkglass : graves propres, haut du spectre saturé avec du grain dans les médiums |

Baffles : 8x10, 4x10, 1x15, ou Off (DI seul). **DI Blend** mélange le signal direct (avant l'ampli) avec l'ampli et le baffle, comme en studio.

Les modèles sont calibrés au même volume perçu à gain égal.

## Presets

11 presets d'usine : Vintage Tube, Clean DI, Motown Flats, Slap Funk, Modern Growl, Metal Grind, Fuzz Lead, Funk Envelope, Synth Octave, Dub Deep, Ambient Chorus. Ils sont équilibrés au même volume, à moins de 3 dB près.

**Save** enregistre le son en cours dans `Documents\Bass Amp\Presets\<nom>.bassamp`. Les flèches passent d'un preset à l'autre. En VST3, les presets d'usine apparaissent aussi comme programmes dans le DAW.

## Build

Prérequis : Visual Studio 2019 Build Tools (C++), CMake + Ninja (`pip install --user cmake ninja`).

```bat
git submodule update --init
build.bat
```

Sorties dans `build\BassAmp_artefacts\Release\` (`Standalone\Bass Amp.exe` et `VST3\Bass Amp.vst3`).

Tests DSP (accordeur, gate, compresseur, octaver, modèles d'ampli, baffles, presets, stabilité de la chaîne) : `test.bat`.
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

## Licence

AGPLv3 (voir `LICENSE`), comme l'impose l'usage gratuit de JUCE. Le SDK ASIO dans `libs/asiosdk` est utilisé sous sa licence GPLv3.
