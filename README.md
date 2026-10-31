# Bass Amp

Ampli de basse en logiciel (JUCE 8, C++17) : application autonome et plugins pour Windows (VST3) et macOS (VST3, AU).

Chaîne : **Gate → Compresseur → Octaver → Filtre d'enveloppe → Fuzz → Disto → Ampli → Baffle + DI → EQ → Phaser → Chorus → Delay → Reverb**.

En plus : accordeur (31 à 500 Hz, détection YIN), vumètres d'entrée et de sortie avec témoins CLIP, réduction de gain du compresseur, analyseur de spectre et enregistrement en WAV.

## Ampli

Préampli (lampes ou transistors selon le modèle), voicing du modèle, section de tonalité, puis ampli de puissance avec sag. Tout l'ampli tourne en suréchantillonnage 2x.

| Modèle | Inspiré de | Caractère |
|---|---|---|
| Hi-Fi | Markbass Little Mark | Transistor neutre et moderne, idéal pour le slap |
| Tube 300 | Ampeg SVT-CL | Tout-lampes, bas-médiums chauds, aigus arrondis, sag |
| Crossover | Darkglass B7K / Microtubes | Graves propres, haut du spectre saturé, grain dans les médiums |
| Tweed | Fender Bassman 5F6-A | Tone stack passif modélisé d'après le circuit (Yeh, Guitarix), rond et vintage |
| Flip Top | Ampeg B-15 | Lampes à faible puissance, le son Motown |
| Solid Punch | Gallien-Krueger 800RB | Transistor punchy, aigus à 4 kHz, attaque nette |
| Tube DI | Tech 21 SansAmp Bass Driver | Préampli DI de studio, du propre au grain |

Les potards de l'ampli :

- **Gain**, **Bass**, **Middle** avec fréquence réglable (**Freq**), **Treble** et **Master**. Freq est inactif sur Tweed, dont le tone stack passif a ses fréquences fixes.
- **Contour** creuse les médiums vers 380 Hz et ouvre graves et aigus, comme le VPF des Markbass.
- **Deep** ajoute du bas et retire du 500 Hz. **Bright** ajoute de l'aigu.
- **DI Blend** mélange le signal direct (avant l'ampli) avec l'ampli et le baffle, comme en studio.

Baffles : 8x10, 4x10, 1x15, 2x12, 2x10 Hi-Fi, Off (DI seul), ou **IR File**. **LOAD IR** charge une réponse impulsionnelle de baffle (WAV, AIFF, FLAC) et passe le baffle sur IR File. Le chemin du fichier est sauvegardé avec les presets utilisateur et la session du DAW. Sans IR chargée, IR File laisse passer le signal tel quel.

Les modèles sont calibrés au même volume perçu à gain égal (écart < 1 dB).

## Égaliseur

Deux modes, avec un bouton ON pour le bypass :

- **Basic** : graphique 9 bandes par octave (33 Hz à 8 kHz), ±12 dB, avec des faders.
- **Advanced** : paramétrique 6 points (coupe-bas, shelf grave, deux bandes de médiums, shelf aigu, coupe-haut), ±18 dB. On glisse les points sur la courbe ; la molette règle le Q des médiums et le double-clic remet un point à zéro.

Le passage d'un mode à l'autre se fait en fondu, sans clic. L'écran affiche la courbe de l'EQ par-dessus le spectre du son de sortie.

## Effets

- **Compresseur** avec **Mix** pour la compression parallèle.
- **Octaver** : octave basse (**Sub**), octave haute (**Up**) et signal direct (**Dry**).
- **Phaser** : 4 étages, vitesse, profondeur et réinjection (**Regen**).
- **Chorus** de basse : le chorus ne s'applique qu'au-dessus de 200 Hz, les graves restent centrés et fermes.
- **Delay** et **Reverb** : coupe-bas sur le signal d'effet pour ne pas brouiller les graves.

## Presets

18 presets d'usine, pensés autour des sons de référence : Tube 300 Rock, Tube 300 Ultra, Motown Flip Top, Fifties Tweed, Rock Punch 4x10, Slap Hi-Fi, Jazz Warm, Tube DI Studio, Tube DI Grit, Crossover Metal, Crossover Djent, Punk Pick, Fuzz Lead, Funk Envelope, Synth Octave, Dub Deep, Fretless Phase, Ambient Chorus. Ils sont équilibrés au même volume, à moins de 1 dB près.

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

Tests DSP (accordeur, gate, compresseur, octaver, modèles d'ampli, tone stack, baffles et IR, égaliseur, presets, stabilité de la chaîne) : `test.bat`.
`test.bat --snapshot ui.png [preset]` exporte une capture de l'interface, avec le preset d'usine choisi.

## Latence

- Le SDK ASIO de Steinberg (GPLv3) est dans `libs/asiosdk` : le support ASIO est compilé automatiquement.
- Au premier lancement, l'appli choisit le pilote ASIO de l'interface déjà utilisée (les pilotes génériques comme ASIO4ALL ou Realtek passent en dernier), avec un buffer de 64 samples. Sans ASIO, elle passe en WASAPI exclusif.
- En ASIO, l'entrée et la sortie sont sur la même interface : branche ton casque ou tes enceintes sur la carte son.
- La taille du buffer ASIO se règle dans le panneau du pilote (**Options → Audio/MIDI Settings → Control Panel**).
- La ligne en haut à droite affiche le pilote, le buffer et la latence aller-retour estimée.
- Aucun effet n'ajoute de latence : pas de look-ahead, oversampling en IIR à phase minimale, et l'accordeur analyse le signal hors du thread audio.

## Utilisation

- Double-clic sur un potard : retour à sa valeur par défaut. Survol ou glisser : la valeur s'affiche.
- La fenêtre se redimensionne (coin en bas à droite) de 60 % à 160 %, et s'ouvre à la taille de l'écran.
- **IN CLIP** : l'entrée de la carte son sature, baisse le gain de l'interface. **OUT CLIP** : la sortie sature, baisse le Master.
- Clic sur le bloc LEVELS : efface les témoins CLIP.
- **MUTE** dans l'accordeur coupe la sortie pendant l'accordage.
- **REC** enregistre le son de sortie (effets compris) en WAV stéréo 24 bits dans `Documents/Bass Amp/Recordings`. **STOP** termine le fichier, **Files** ouvre le dossier sur le dernier enregistrement. L'écriture sur disque se fait sur un thread séparé, donc elle n'ajoute ni latence ni craquement.

## Licence

AGPLv3 (voir `LICENSE`), comme l'impose l'usage gratuit de JUCE. Le SDK ASIO dans `libs/asiosdk` est utilisé sous sa licence GPLv3.
