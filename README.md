# Bass Amp

Ampli de basse en logiciel (JUCE 8, C++17) : application autonome et plugins pour Windows (VST3) et macOS (VST3, AU).

Chaîne : **Gate → Compresseur → Octaver → Filtre d'enveloppe → Fuzz → Disto → Ampli → Baffle + DI → Chorus → Delay → Reverb**.

En plus : accordeur (31 à 500 Hz, détection YIN), vumètres d'entrée et de sortie avec témoins CLIP, réduction de gain du compresseur, et enregistrement en WAV.

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

**Save** enregistre le son en cours dans `Documents/Bass Amp/Presets/<nom>.bassamp`. Les flèches passent d'un preset à l'autre. En VST3, les presets d'usine apparaissent aussi comme programmes dans le DAW.

## Installation (macOS)

Télécharge les zips macOS de la [dernière release](https://github.com/Weylland/bass-amp/releases/latest). C'est un binaire universel (Apple Silicon et Intel), pour macOS 10.15 ou plus récent.

- **Standalone** : glisse `Bass Amp.app` dans `Applications`.
- **Plugins** : copie `Bass Amp.vst3` dans `~/Library/Audio/Plug-Ins/VST3` et `Bass Amp.component` dans `~/Library/Audio/Plug-Ins/Components` (pour Logic et GarageBand).

L'appli n'est pas notarisée par Apple. Au premier lancement, macOS refuse de l'ouvrir : va dans **Réglages Système → Confidentialité et sécurité** et clique sur **Ouvrir quand même**. Pour les plugins, la commande suivante retire la quarantaine :

```bash
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/"Bass Amp.vst3" ~/Library/Audio/Plug-Ins/Components/"Bass Amp.component"
```

Sur Mac, l'entrée est coupée au premier lancement, car l'entrée par défaut est souvent le micro intégré. Choisis ta carte son dans **Options → Audio/MIDI Settings**, puis décoche **Mute audio input**. macOS demande aussi l'autorisation d'accès au micro : accepte-la, sinon la basse reste muette.

## Build

Windows : Visual Studio 2019 Build Tools (C++), CMake + Ninja (`pip install --user cmake ninja`).

```bat
git submodule update --init
build.bat
```

Sorties dans `build\BassAmp_artefacts\Release\` (`Standalone\Bass Amp.exe` et `VST3\Bass Amp.vst3`).

macOS : Xcode (outils en ligne de commande) et CMake.

```bash
git submodule update --init
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build --parallel
```

La CI GitHub Actions compile et teste les deux plateformes à chaque push. Un tag `v*` publie automatiquement les zips dans une release.

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
- **REC** enregistre le son de sortie (effets compris) en WAV stéréo 24 bits dans `Documents/Bass Amp/Recordings`. **STOP** termine le fichier, **Files** ouvre le dossier sur le dernier enregistrement. L'écriture sur disque se fait sur un thread séparé, donc elle n'ajoute ni latence ni craquement.

## Licence

AGPLv3 (voir `LICENSE`), comme l'impose l'usage gratuit de JUCE. Le SDK ASIO dans `libs/asiosdk` est utilisé sous sa licence GPLv3.
