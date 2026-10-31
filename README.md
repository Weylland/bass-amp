# Bass Amp

Ampli de basse en logiciel (JUCE 8, C++17) : application autonome et plugins pour Windows (VST3) et macOS (VST3, AU).

Chaîne par défaut : **Gate → Compresseur → Octaver → Filtre d'enveloppe → Fuzz → Disto → Ampli → Baffle + DI → EQ → Phaser → Chorus → Delay → Reverb → Limiteur**. Synthé, Pitch, Filtre, Wah, Flanger et Trémolo s'ajoutent au pedalboard, et l'ordre des pédales se change à la souris.

En plus : accordeur (31 à 500 Hz, détection YIN), vumètres d'entrée et de sortie avec témoins CLIP, réduction de gain du compresseur, analyseur de spectre, enregistrement en WAV, lecteur audio, looper, métronome et contrôle MIDI.

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
| NAM Capture | Ta propre capture (Neural Amp Modeler) | Le son d'un vrai ampli, capturé par réseau de neurones |

Les potards de l'ampli :

- **Gain**, **Bass**, **Middle** avec fréquence réglable (**Freq**), **Treble** et **Master**. Freq est inactif sur Tweed, dont le tone stack passif a ses fréquences fixes.
- **Contour** creuse les médiums vers 380 Hz et ouvre graves et aigus, comme le VPF des Markbass.
- **Deep** ajoute du bas et retire du 500 Hz. **Bright** ajoute de l'aigu.
- **DI Blend** mélange le signal direct (avant l'ampli) avec l'ampli et le baffle, comme en studio.
- **Lo Cut** (20 à 250 Hz) et **Hi Cut** (1,5 à 20 kHz), en haut de la tête : coupe-bas et coupe-haut en sortie d'ampli, pour resserrer les graves ou adoucir les aigus.

Baffles : 8x10, 4x10, 1x15, 2x12, 2x10 Hi-Fi, Off (DI seul), ou **IR File**. **LOAD IR** charge une réponse impulsionnelle de baffle (WAV, AIFF, FLAC) et passe le baffle sur IR File. Le chemin du fichier est sauvegardé avec les presets utilisateur et la session du DAW. Sans IR chargée, IR File laisse passer le signal tel quel.

Les modèles sont calibrés au même volume perçu à gain égal (écart < 1 dB).

**NAM Capture** joue une capture `.nam` du [Neural Amp Modeler](https://www.neuralampmodeler.com/) : on en trouve des milliers, gratuites, sur [TONE3000](https://www.tone3000.com/). **LOAD NAM** charge le fichier ; la capture remplace le préampli et l'ampli de puissance, et les potards de tonalité, Contour, Deep, Bright, le baffle et le DI Blend restent actifs après elle. **Gain** règle le niveau d'entrée de la capture (±12 dB). Le volume est normalisé d'après la mesure stockée dans le fichier, comme dans le plugin NAM d'origine. Une capture est faite pour une fréquence d'échantillonnage (souvent 48 kHz) : si la carte son tourne à une autre fréquence, l'écran l'indique. Le chemin du fichier est sauvegardé avec les presets utilisateur et la session du DAW ; sans capture chargée, le signal passe tel quel.

## Égaliseur

Deux modes, avec un bouton ON pour le bypass :

- **Basic** : graphique 9 bandes par octave (33 Hz à 8 kHz), ±12 dB, avec des faders.
- **Advanced** : paramétrique 6 points (coupe-bas, shelf grave, deux bandes de médiums, shelf aigu, coupe-haut), ±18 dB. On glisse les points sur la courbe ; la molette règle le Q des médiums et le double-clic remet un point à zéro.

Le passage d'un mode à l'autre se fait en fondu, sans clic. L'écran affiche la courbe de l'EQ par-dessus le spectre du son de sortie.

## Effets

- **Compresseur** avec **Mix** pour la compression parallèle.
- **Octaver** : octave basse (**Sub**), octave haute (**Up**) et signal direct (**Dry**).
- **Synth** : synthé de basse piloté par la note jouée, onde carrée ou dent de scie (**Wave**), filtre passe-bas résonant (**Cutoff**, **Reso**) ouvert par l'attaque (**Env**).
- **Pitch** : transposition de -12 à +12 demi-tons (**Shift**), désaccord fin (**Detune**) et mélange (**Mix**), pour l'octave haute ou une quinte.
- **Filter** : filtre passe-bas, passe-bande ou passe-haut en 12 ou 24 dB/oct (**Type**), fréquence (**Cutoff**), résonance (**Reso**) et LFO (**Rate**, **Depth**) pour les balayages.
- **Envelope** (auto-wah) : filtre passe-bas, passe-bande ou passe-haut (**Type**), qui s'ouvre ou se ferme à l'attaque (**Sweep** Up/Down).
- **Wah** : wah de basse, réglée au potard **Pedal** ou à la pédale d'expression MIDI, avec résonance et part de signal direct (**Clean**).
- **Phaser** : 4 étages, vitesse, profondeur et réinjection (**Regen**).
- **Flanger** stéréo : vitesse, profondeur, réinjection et mélange.
- **Chorus** de basse : le chorus ne s'applique qu'au-dessus de 200 Hz, les graves restent centrés et fermes.
- **Tremolo** : vitesse, profondeur et forme (**Shape**, de la sinusoïde au créneau).
- **Delay** et **Reverb** : coupe-bas sur le signal d'effet pour ne pas brouiller les graves. **TAP** sur le delay règle le temps en tapant le tempo.
- **LIMIT** (bloc LEVELS, actif par défaut) : limiteur de sortie à -1 dBFS, transparent tant que le son ne dépasse pas ce niveau.

## Pedalboard

- **+ ADD** ajoute une pédale (et l'allume), la croix en haut à droite d'une pédale la retire (et l'éteint).
- On glisse une pédale pour changer sa place sur son pedalboard : le son suit l'ordre affiché.
- L'ordre est sauvegardé avec les presets utilisateur et la session du DAW. Un preset d'usine remet l'ordre par défaut.

## Pratique

La rangée du bas :

- **Player** : **LOAD** ouvre un fichier (WAV, AIFF, FLAC, OGG, MP3) pour jouer par-dessus, avec lecture, pause, stop, barre de position, boucle et volume. **RE-AMP** envoie le fichier dans l'entrée de la chaîne à la place de la basse, pour repasser une piste DI dans les pédales et l'ampli.
- **Looper** (60 s) : le bouton principal enregistre, puis lit (ce qui fixe la longueur de la boucle), puis superpose (**DUB**). **STOP** garde la boucle, **CLEAR** l'efface. **LEVEL** règle son volume.
- **Metronome** : tempo de 40 à 240 BPM au potard ou avec **TAP**, volume, et un voyant par temps. Le clic n'est jamais enregistré par **REC**.

## MIDI

- Clic droit sur un potard, un fader ou un interrupteur, puis **MIDI learn**, puis bouge un contrôle de ton contrôleur : le contrôle clignote en doré tant qu'il attend. Le même menu permet d'oublier l'assignation.
- Un CC pilote n'importe quel réglage. Un pad (note) allume ou éteint un interrupteur, ou passe au choix suivant.
- Par défaut, le CC 11 (pédale d'expression) pilote la wah.
- Les changements de programme chargent les presets d'usine.
- Les assignations sont communes à tous les presets, dans `Bass Amp/MidiMap.xml` (dossier des données d'application). L'appli autonome ouvre toutes les entrées MIDI au lancement.

## Presets

24 presets d'usine, pensés autour des sons de référence : Tube 300 Rock, Tube 300 Ultra, Motown Flip Top, Fifties Tweed, Rock Punch 4x10, Slap Hi-Fi, Jazz Warm, Tube DI Studio, Tube DI Grit, Crossover Metal, Crossover Djent, Punk Pick, Fuzz Lead, Funk Envelope, Synth Octave, Dub Deep, Fretless Phase, Ambient Chorus, Wah Funk, Analog Synth, Filter Sweep, Octave Up Lead, Jet Flanger, Surf Tremolo. Ils sont équilibrés au même volume, à moins de 1 dB près.

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
git submodule update --init --recursive
build.bat
```

Sorties dans `build\BassAmp_artefacts\Release\` (`Standalone\Bass Amp.exe` et `VST3\Bass Amp.vst3`).

macOS : Xcode (outils en ligne de commande) et CMake.

```bash
git submodule update --init --recursive
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build build --parallel
```

La CI GitHub Actions compile et teste les deux plateformes à chaque push. Un tag `v*` publie automatiquement les zips dans une release.

Tests DSP (accordeur, effets, modèles d'ampli, tone stack, baffles et IR, égaliseur, limiteur, ordre des pédales, MIDI, looper, métronome, lecteur, captures NAM, presets, stabilité de la chaîne) : `test.bat`.
`test.bat --snapshot ui.png [preset]` exporte une capture de l'interface, avec le preset d'usine choisi.
`test.bat --artwork dossier` exporte l'icône (1024 px) et l'écran de démarrage. L'icône des builds est `Resources/icon.png`.

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

AGPLv3 (voir `LICENSE`), comme l'impose l'usage gratuit de JUCE. Le SDK ASIO dans `libs/asiosdk` est utilisé sous sa licence GPLv3. [NeuralAmpModelerCore](https://github.com/sdatkinson/NeuralAmpModelerCore) (MIT) et [Eigen](https://eigen.tuxfamily.org/) (MPL 2.0) sont dans `libs/NeuralAmpModelerCore`.
