#include "Translations.h"

const Translations& frenchTranslations()
{
    static const Translations translations {
        {
            // Pedals
            { "pedal.gate", "GATE (noise gate)\nCoupe le son quand tu ne joues pas, pour supprimer le souffle et les bruits de micro. "
                            "Mets-le en premier dans la chaîne, surtout avant la disto ou le fuzz qui amplifient le bruit." },
            { "pedal.comp", "COMP (compresseur)\nResserre les écarts de volume : les notes fortes sont baissées, les faibles ressortent. "
                            "Le son devient plus égal et plus « devant ». Indispensable pour le slap et pour un son de studio." },
            { "pedal.octaver", "OCTAVER\nAjoute l'octave du dessous (Sub) et celle du dessus (Up) à ta note. "
                               "Joue des notes simples : les accords le perdent. Idéal pour un son énorme ou une ligne de synthé." },
            { "pedal.synth", "SYNTH\nTransforme ta basse en synthé : un oscillateur suit la note que tu joues, passe dans un filtre "
                             "qui s'ouvre à l'attaque. Joue une note à la fois, bien nette, pour qu'il la suive." },
            { "pedal.pitch", "PITCH (transpositeur)\nDécale la hauteur de ta note, de -12 à +12 demi-tons, mélangée au son direct. "
                             "+12 pour une octave haute, +7 pour une quinte, un léger Detune pour épaissir." },
            { "pedal.filter", "FILTER\nFiltre résonant réglé à la main ou balayé par un LFO (une oscillation lente). "
                              "Passe-bas pour un son sourd et rond, passe-haut pour un son fin, passe-bande pour un effet « radio »." },
            { "pedal.envelope", "ENVELOPE (auto-wah)\nUn filtre qui s'ouvre selon la force de ton attaque : plus tu frappes, plus le son "
                                "s'éclaircit. Le son funk par excellence (Bootsy Collins, Mark Adams)." },
            { "pedal.wah", "WAH\nUn filtre qu'on balaie au pied, qui fait « ouah ». Ici, le potard Pedal le règle, ou une pédale "
                           "d'expression MIDI (CC 11 par défaut). Clean garde du son direct pour ne pas perdre les graves." },
            { "pedal.fuzz", "FUZZ\nSaturation très forte et carrée, façon pédale vintage au silicium. Blend remet du son clair "
                            "en dessous pour garder les graves solides." },
            { "pedal.drive", "DRIVE (overdrive)\nSaturation plus douce et plus naturelle que le fuzz, comme un ampli qui chauffe. "
                             "Blend mélange le son clair pour garder de la définition." },
            { "pedal.phaser", "PHASER\nCreuse des encoches qui se déplacent dans le spectre : un son tournant et liquide, très années 70." },
            { "pedal.flanger", "FLANGER\nComme le phaser mais plus métallique : le son passe par un délai très court qui varie. "
                               "Avec beaucoup de Regen, on obtient l'effet « avion à réaction »." },
            { "pedal.chorus", "CHORUS\nDouble ton son avec une copie légèrement désaccordée qui bouge, pour l'élargir. "
                              "Les graves restent centrés et fermes, seul le haut du spectre est traité." },
            { "pedal.tremolo", "TREMOLO\nFait varier le volume en rythme, comme les vieux amplis. Shape passe d'une ondulation douce "
                               "à un hachage net." },
            { "pedal.delay", "DELAY (écho)\nRépète ce que tu joues après un temps réglable. TAP : tape le tempo pour caler l'écho sur le morceau." },
            { "pedal.reverb", "REVERB\nSimule l'acoustique d'une pièce : le son se prolonge et prend de l'espace. "
                              "À petite dose sur la basse, pour ne pas noyer les graves." },

            // Gate
            { "gate_threshold", "THRESH (seuil)\nNiveau en dessous duquel le gate coupe. Monte-le jusqu'à ce que le souffle disparaisse "
                                "quand tu ne joues pas, sans couper la fin de tes notes." },
            { "gate_release", "RELEASE\nTemps que met le gate à se refermer. Court : coupe net ; long : laisse les notes mourir naturellement." },

            // Compressor
            { "comp_threshold", "THRESH (seuil)\nNiveau à partir duquel le compresseur agit. Plus il est bas, plus il compresse. "
                                "Regarde la jauge COMP dans LEVELS : 3 à 6 dB de réduction est un bon début." },
            { "comp_ratio", "RATIO\nForce de la compression au-dessus du seuil. 2:1 à 4:1 : naturel ; 8:1 et plus : très égal, "
                            "presque un limiteur." },
            { "comp_attack", "ATTACK\nVitesse de réaction. Lente (10 ms et plus) : laisse passer le claquement de l'attaque, plus punchy ; "
                             "rapide : écrase l'attaque." },
            { "comp_release", "RELEASE\nTemps pour relâcher la compression après une note. Trop court : le son « pompe » ; trop long : "
                              "les notes suivantes restent compressées." },
            { "comp_makeup", "MAKEUP\nRemonte le volume après la compression, pour retrouver le même niveau qu'avec la pédale éteinte." },
            { "comp_mix", "MIX\nMélange le son compressé avec le son direct (compression parallèle) : on garde l'attaque naturelle "
                          "et on gagne la densité." },

            // Octaver
            { "oct_sub", "SUB\nVolume de l'octave du dessous. Pour un son très grave, façon synthé." },
            { "oct_up", "UP\nVolume de l'octave du dessus, qui ajoute un son brillant et creux." },
            { "oct_dry", "DRY\nVolume de ton son d'origine dans le mélange." },

            // Envelope
            { "env_type", "TYPE\nForme du filtre : LP (passe-bas, le wah classique), BP (passe-bande, plus nasal) ou HP (passe-haut, plus fin). "
                          "Clique pour changer." },
            { "env_dir", "SWEEP\nUp : le filtre s'ouvre quand tu frappes. Down : il se ferme, pour un effet inversé « wouo »." },
            { "env_sens", "SENS (sensibilité)\nÀ quel point ton attaque ouvre le filtre. Règle-le selon la force de ton jeu." },
            { "env_reso", "RESO (résonance)\nAccentue la fréquence du filtre. Plus c'est haut, plus l'effet « couac » est marqué." },
            { "env_mix", "MIX\nMélange l'effet avec ton son direct, pour garder des graves." },

            // Fuzz and drive
            { "fuzz_amount", "FUZZ\nQuantité de saturation." },
            { "fuzz_tone", "TONE\nSombre à gauche, agressif à droite." },
            { "fuzz_blend", "BLEND\nMélange le fuzz avec le son clair. Vers la gauche, les graves restent propres." },
            { "dist_drive", "DRIVE\nQuantité de saturation, du léger grain à la vraie disto." },
            { "dist_tone", "TONE\nCoupe les aigus de la saturation. Plus bas : plus rond ; plus haut : plus mordant." },
            { "dist_blend", "BLEND\nMélange la saturation avec le son clair, pour garder la définition des notes." },

            // Filter
            { "filter_type", "TYPE\nLP : passe-bas (garde les graves) ; BP : passe-bande (garde une zone) ; HP : passe-haut (garde les aigus). "
                             "12 ou 24 : pente douce ou raide. Clique pour changer." },
            { "filter_cutoff", "CUTOFF (fréquence de coupure)\nOù le filtre agit. Sur un passe-bas, baisser rend le son plus sourd." },
            { "filter_reso", "RESO (résonance)\nAccentue la fréquence de coupure : le son devient plus « synthé » et sifflant." },
            { "filter_rate", "RATE\nVitesse du LFO qui fait bouger la coupure. Sans effet si Depth est à zéro." },
            { "filter_depth", "DEPTH\nAmplitude du balayage par le LFO. À zéro, le filtre reste fixe." },

            // Wah
            { "wah_pedal", "PEDAL\nPosition de la pédale : talon (grave) à pointe (aigu). Pilotable par une pédale d'expression MIDI." },
            { "wah_reso", "RESO\nForce du pic de la wah. Haut : effet très vocal ; bas : plus subtil." },
            { "wah_blend", "CLEAN\nQuantité de son direct gardé sous la wah, pour ne pas perdre les graves." },

            // Synth
            { "synth_wave", "WAVE\nForme de l'oscillateur : Square (carré, creux et rond) ou Saw (dent de scie, riche et brillant). "
                            "Clique pour changer." },
            { "synth_cutoff", "CUTOFF\nOuverture du filtre au repos. Bas : son sourd ; haut : son brillant." },
            { "synth_reso", "RESO\nRésonance du filtre, pour le côté « acid » et sifflant." },
            { "synth_env", "ENV\nÀ quel point ton attaque ouvre le filtre. Haut : chaque note fait « wow »." },
            { "synth_mix", "MIX\nMélange le synthé avec ta basse." },

            // Pitch
            { "pitch_shift", "SHIFT\nDécalage en demi-tons : +12 = octave au-dessus, +7 = quinte, -12 = octave en dessous." },
            { "pitch_detune", "DETUNE\nDésaccord fin, en centièmes de demi-ton. Quelques centièmes épaississent le son comme un chorus." },
            { "pitch_mix", "MIX\nMélange la note transposée avec ta note d'origine." },

            // Amp
            { "amp_gain", "GAIN\nQuantité de saturation du préampli. Bas : son clair ; vers 5 : léger grain ; au-delà : saturé. "
                          "Sur NAM Capture, règle le niveau d'entrée de la capture." },
            { "amp_bass", "BASS\nGraves : la rondeur et le poids." },
            { "amp_mid", "MIDDLE\nMédiums : ce qui fait ressortir la basse dans le mix. Les baisser donne un son creusé, les monter un son qui perce." },
            { "amp_mid_freq", "FREQ\nFréquence sur laquelle agit Middle : bas (200 Hz) pour l'épaisseur, haut (1 à 3 kHz) pour l'attaque. "
                              "Inactif sur Tweed '59, dont le circuit fixe les fréquences." },
            { "amp_treble", "TREBLE\nAigus : la clarté et le bruit des cordes." },
            { "amp_master", "MASTER\nVolume général de l'ampli. Si le témoin OUT CLIP s'allume, baisse-le." },
            { "amp_blend", "DI BLEND\nMélange le son direct de la basse (avant l'ampli) avec l'ampli et le baffle, comme en studio : "
                           "le direct apporte des graves nets, l'ampli le caractère." },
            { "amp_contour", "CONTOUR\nCreuse les médiums vers 380 Hz et ouvre graves et aigus : le son « smiley », idéal pour le slap." },
            { "amp_deep", "DEEP\nAjoute du bas et retire un peu de 500 Hz, pour un son plus profond." },
            { "amp_bright", "BRIGHT\nAjoute de l'aigu, pour plus de clarté et de claquant." },
            { "amp_low_cut", "LO CUT\nCoupe les infra-graves en sortie d'ampli. Utile pour resserrer le son ou pour les petites enceintes "
                             "qui saturent dans le grave." },
            { "amp_high_cut", "HI CUT\nCoupe les aigus en sortie d'ampli, pour adoucir un son trop brillant ou une disto qui grésille." },

            // Amp models
            { "model.Italian Hi-Fi", "ITALIAN HI-FI\nTransistor neutre et moderne, inspiré du Markbass Little Mark. Rapide et précis, idéal pour le slap et le funk." },
            { "model.Super Valve", "SUPER VALVE\nTout-lampes de 300 W, inspiré de l'Ampeg SVT : bas-médiums chauds, aigus arrondis, le son rock." },
            { "model.Nordic Drive", "NORDIC DRIVE\nInspiré du Darkglass B7K : les graves restent propres, seul le haut du spectre sature. Le son metal moderne." },
            { "model.Tweed '59", "TWEED '59\nInspiré du Fender Bassman de 1959, avec son tone stack passif modélisé : rond, vintage, sature tôt." },
            { "model.Flip Top '60s", "FLIP TOP '60S\nPetit ampli à lampes de studio, inspiré de l'Ampeg B-15 : sombre et rond, le son Motown." },
            { "model.West Coast 800", "WEST COAST 800\nTransistor rock inspiré du Gallien-Krueger 800RB : médiums creusés, mordant à 4 kHz." },
            { "model.Stomp DI", "STOMP DI\nPréampli DI inspiré du SansAmp Bass Driver : direct et punchy, du propre au grain." },
            { "model.NAM Capture", "NAM CAPTURE\nJoue une capture d'un vrai ampli (fichier .nam du Neural Amp Modeler), "
                                   "à télécharger gratuitement sur TONE3000. Charge-la avec LOAD NAM." },

            // Cabinets
            { "cab.Off (DI)", "OFF (DI)\nPas de baffle : le son direct de l'ampli, comme une sortie DI." },
            { "cab.8x10", "8x10\nLe gros frigo des concerts rock : graves puissants, médiums présents." },
            { "cab.4x10", "4x10\nPolyvalent et punchy, avec plus d'attaque que le 8x10." },
            { "cab.1x15", "1x15\nUn seul gros haut-parleur : rond, profond, vintage, peu d'aigus." },
            { "cab.2x12", "2x12\nCompact et médium, il perce bien dans un groupe." },
            { "cab.2x10 Hi-Fi", "2x10 HI-FI\nAvec tweeter : son large et clair, pour le slap et le jazz moderne." },
            { "cab.IR File", "IR FILE\nJoue ta propre réponse impulsionnelle (IR) de baffle, chargée avec LOAD IR." },

            // EQ
            { "eq_on", "ON\nAllume ou éteint l'égaliseur. Il s'allume tout seul dès que tu touches un curseur ou un point." },
            { "cut_low", "CUT 80 HZ\nCoupe d'un coup tout ce qui est sous 80 Hz : le son se resserre et laisse de la place à la grosse caisse." },
            { "kill_low", "KILL LOW\nSupprime les graves (sous 250 Hz). Pour un effet de coupure en live ou pour isoler l'attaque." },
            { "kill_mid", "KILL MID\nSupprime les médiums (de 250 Hz à 2,5 kHz) : il ne reste que le grave et le claquant." },
            { "kill_high", "KILL HIGH\nSupprime les aigus (au-dessus de 2,5 kHz) : son sourd et rond, comme une basse à cordes plates." },
            { "cut_high", "CUT 5 KHZ\nCoupe d'un coup tout ce qui est au-dessus de 5 kHz : adoucit le son et enlève le souffle." },
            { "geq_33", "33 Hz\nInfra-graves : la vibration qu'on sent plus qu'on n'entend. À monter avec prudence." },
            { "geq_63", "63 Hz\nLe poids et l'assise du son." },
            { "geq_125", "125 Hz\nLa rondeur. Trop : le son devient boomy." },
            { "geq_250", "250 Hz\nL'épaisseur. Trop : le son devient boueux et brouillon." },
            { "geq_500", "500 Hz\nLe corps et le côté « boîte ». Le baisser aère le son." },
            { "geq_1k", "1 kHz\nLa présence : ce qui fait entendre la basse dans le mix." },
            { "geq_2k", "2 kHz\nL'attaque et le grain des notes." },
            { "geq_4k", "4 kHz\nLe claquant des cordes et le bruit des doigts." },
            { "geq_8k", "8 kHz\nLa brillance et le souffle." },
            { "peq_low_cut", "LOW CUT\nPoint gris de gauche : coupe tout ce qui est en dessous. Glisse-le à droite pour resserrer les graves. Il ne bouge qu'à gauche ou à droite : il n'a pas de gain." },
            { "peq_low_freq", "LOW (shelf grave)\nMonte ou baisse tout le grave sous sa fréquence. Glisse en hauteur pour le gain, "
                              "à gauche ou à droite pour la fréquence." },
            { "peq_mid1_freq", "LO-MID\nBande de bas-médiums : épaisseur et boue. Glisse pour régler, molette pour la largeur (Q), "
                               "double-clic pour remettre à zéro." },
            { "peq_mid2_freq", "HI-MID\nBande de hauts-médiums : attaque et présence. Glisse pour régler, molette pour la largeur (Q), "
                               "double-clic pour remettre à zéro." },
            { "peq_high_freq", "HIGH (shelf aigu)\nMonte ou baisse tout l'aigu au-dessus de sa fréquence." },
            { "peq_high_cut", "HIGH CUT\nPoint gris de droite : coupe tout ce qui est au-dessus. Glisse-le à gauche pour adoucir le son. Il ne bouge qu'à gauche ou à droite : il n'a pas de gain." },

            // Post-amp effects
            { "phaser_rate", "RATE\nVitesse du mouvement." },
            { "phaser_depth", "DEPTH\nAmplitude du mouvement." },
            { "phaser_feedback", "REGEN\nRéinjection : plus c'est haut, plus l'effet est marqué et chantant." },
            { "flanger_rate", "RATE\nVitesse du balayage." },
            { "flanger_depth", "DEPTH\nAmplitude du balayage." },
            { "flanger_feedback", "REGEN\nRéinjection : plus c'est haut, plus le son devient métallique." },
            { "flanger_mix", "MIX\nMélange l'effet avec le son direct." },
            { "chorus_rate", "RATE\nVitesse de l'ondulation. Lent : large et doux ; rapide : effet vibrato." },
            { "chorus_depth", "DEPTH\nAmplitude du désaccord." },
            { "chorus_mix", "MIX\nMélange l'effet avec le son direct." },
            { "tremolo_rate", "RATE\nVitesse des variations de volume." },
            { "tremolo_depth", "DEPTH\nAmplitude des variations : à fond, le son est coupé à chaque cycle." },
            { "tremolo_shape", "SHAPE\nForme de l'onde : à gauche une ondulation douce, à droite un hachage net." },
            { "delay_time", "TIME\nTemps entre ton jeu et l'écho, en millisecondes. Ou tape le tempo avec TAP." },
            { "delay_feedback", "REPEAT\nNombre de répétitions : bas, un seul écho ; haut, des échos qui durent." },
            { "delay_mix", "MIX\nVolume des échos." },
            { "reverb_size", "SIZE\nTaille de la pièce : petite salle ou grande église." },
            { "reverb_damping", "DAMP\nAbsorption des aigus : haut pour une réverb sombre et discrète." },
            { "reverb_mix", "MIX\nQuantité de réverb." },
            { "limiter_on", "LIMIT\nLimiteur de sortie : empêche le son de dépasser -1 dBFS, pour protéger tes oreilles et éviter la saturation "
                            "numérique. Transparent tant que tu restes sous ce niveau." },

            // Interface
            { "ui.footswitch", "FOOTSWITCH\nAllume ou éteint la pédale : la LED s'allume quand elle est active. Clic droit : "
                               "MIDI learn, pour la commander avec un pad de ton contrôleur. Un point doré sur un réglage "
                               "indique qu'un contrôle MIDI le pilote." },
            { "ui.tap", "TAP\nTape au moins deux fois en rythme : le temps du delay se cale sur ton tempo." },
            { "ui.pedalPresets", "PRESETS DE LA PÉDALE\nDes réglages tout prêts pour cette pédale. Le reste de ton son ne change pas." },
            { "ui.removePedal", "RETIRER\nÉteint la pédale et la range. Tu peux la remettre avec + ADD." },
            { "ui.addPedal", "+ ADD\nAjoute une pédale sur ce pedalboard. Ensuite, glisse une pédale pour changer sa place : "
                             "le son suit l'ordre affiché, de gauche à droite." },
            { "ui.presetBox", "PRESETS\nLes sons d'usine et les tiens. Choisis-en un pour tout régler d'un coup. Une étoile "
                              "dorée indique que tu as modifié le son depuis son chargement : enregistre-le avec SAVE pour le "
                              "garder." },
            { "ui.previousPreset", "PRESET PRÉCÉDENT" },
            { "ui.nextPreset", "PRESET SUIVANT" },
            { "ui.savePreset", "SAVE\nEnregistre le son actuel comme preset, dans Documents/Bass Amp/Presets." },
            { "ui.presetMenu", "MENU DES PRESETS\nRenomme ou supprime le preset utilisateur chargé, importe un preset qu'on "
                               "t'a envoyé, exporte ce son pour le partager (un zip avec ses fichiers IR et capture), ou ouvre "
                               "le dossier des presets." },
            { "ui.record", "REC\nEnregistre ce qui sort de l'appli (effets compris) en WAV, dans Documents/Bass Amp/Recordings. "
                           "Clique à nouveau pour arrêter." },
            { "ui.recordings", "FILES\nOuvre le dossier des enregistrements." },
            { "ui.tuner", "ACCORDEUR\nJoue une corde à vide : la note s'affiche et l'aiguille est au centre quand elle est "
                          "juste. Mi La Ré Sol, et Si grave sur une cinq cordes. Molette sur l'écran : La de référence de 430 "
                          "à 450 Hz ; double-clic : retour à 440 Hz." },
            { "ui.mute", "MUTE\nCoupe le son de sortie pendant que tu t'accordes." },
            { "ui.meters", "NIVEAUX\nIN : ce qui entre (ta basse) ; OUT : ce qui sort ; COMP : réduction du compresseur. CLIP "
                           "rouge : saturation, baisse le gain de ta carte son (IN) ou le Master (OUT). Clique pour effacer "
                           "les témoins. Clique sur IN 1 / IN 2 pour choisir l'entrée de la carte son où ta basse est "
                           "branchée." },
            { "ui.eqPresets", "PRESETS D'EQ\nDes courbes toutes prêtes : voix, guitare, basse ronde, slap… Seul l'égaliseur change." },
            { "ui.eqBasic", "BASIC\nÉgaliseur graphique 9 bandes, avec des faders." },
            { "ui.eqAdvanced", "ADVANCED\nÉgaliseur paramétrique : 6 points à glisser sur la courbe, plus précis." },
            { "ui.eqScreen", "ÉCRAN DE L'EQ\nLa courbe montre la correction, le fond le spectre du son qui sort. "
                             "Repère les bosses de fréquences pour les corriger." },
            { "ui.ampPresets", "PRESETS D'AMPLI\nDes réglages d'ampli et de baffle tout prêts. Les pédales et l'EQ ne changent pas." },
            { "ui.power", "POWER\nLe voyant d'alimentation, comme sur un vrai ampli : il reste allumé tant que l'appli tourne. "
                          "Il n'y a rien à régler ici." },
            { "ui.language", "LANGUE\nLa langue des bulles d'aide et des menus. Les noms des réglages restent en anglais, comme sur le vrai matériel." },
            { "ui.audioSettings", "AUDIO\nLes réglages audio et MIDI : pilote (ASIO, WASAPI…), carte son, canaux d'entrée et "
                                  "de sortie, fréquence d'échantillonnage, taille du buffer (plus petit = moins de latence) et "
                                  "entrées MIDI. Cliquer sur la ligne du pilote à côté fait pareil." },
            { "ui.update", "NOUVELLE VERSION\nUne version plus récente de Bass Amp est sortie. Clique pour ouvrir sa page de "
                           "téléchargement. Bass Amp vérifie une fois par lancement en interrogeant GitHub ; rien sur toi "
                           "n'est envoyé." },
            { "ui.fullScreen", "PLEIN ÉCRAN\nAgrandit l'appli à tout l'écran. F11 fait pareil, Échap revient en fenêtre." },
            { "ui.loadIr", "LOAD IR\nCharge une réponse impulsionnelle de baffle (WAV, AIFF, FLAC) et passe le baffle sur IR File." },
            { "ui.loadNam", "LOAD NAM\nCharge une capture .nam d'ampli (TONE3000 en propose des milliers). "
                            "Les potards de tonalité et le baffle restent actifs après elle." },
            { "ui.player", "PLAYER\nJoue un morceau ou une piste pour jouer par-dessus." },
            { "ui.playerLoad", "LOAD\nOuvre un fichier audio (WAV, AIFF, FLAC, OGG, MP3) et le lance." },
            { "ui.playerPlay", "PLAY / PAUSE" },
            { "ui.playerStop", "STOP\nArrête et revient au début." },
            { "ui.playerPosition", "POSITION\nGlisse pour te déplacer dans le morceau." },
            { "ui.playerLoop", "LOOP\nRejoue le morceau en boucle." },
            { "ui.reamp", "RE-AMP\nEnvoie le fichier dans la chaîne à la place de ta basse : pour repasser une piste DI enregistrée "
                          "dans les pédales et l'ampli, et tester des réglages sans jouer." },
            { "ui.playerVolume", "VOL\nVolume du morceau." },
            { "ui.looper", "LOOPER\n4 pistes de boucles pour construire un morceau en live. Clique un pad pour enregistrer, "
                           "OPEN pour le looper complet." },
            { "ui.loopPad", "PISTE\n1er clic : enregistre (avec AUTO, attend ta première note ; avec COUNT-IN, une mesure "
                            "entière). 2e clic : la boucle tourne. Ensuite chaque clic superpose une couche (DUB) ou revient à "
                            "la lecture. Clic droit : MIDI learn, pour la lancer avec un pad ou une pédale." },
            { "ui.looperOpen", "OPEN\nOuvre le looper complet : réglages de chaque piste, annuler, export." },
            { "ui.looperClose", "CLOSE\nReferme le looper : les boucles continuent de tourner." },
            { "ui.looperStopAll", "STOP ALL\nArrête toutes les pistes, ou les relance si tout est arrêté. Elles repartent en place." },
            { "ui.looperClearAll", "CLEAR ALL\nEfface toutes les pistes." },
            { "ui.looperUndo", "UNDO\nAnnule le dernier enregistrement ou la dernière couche ajoutée." },
            { "ui.looperRedo", "REDO\nRemet ce que UNDO vient d'enlever." },
            { "ui.looperSync", "SYNC\nQuand le métronome tourne, les boucles démarrent et s'arrêtent sur une mesure : "
                               "lance l'enregistrement, il attend le début de la mesure suivante. "
                               "Sinon, la première boucle donne la longueur que suivent les autres." },
            { "ui.looperCountIn", "COUNT-IN\nAvec SYNC et le métronome en marche, la première boucle attend une mesure entière "
                                  "de clics avant d'enregistrer, pour savoir quand entrer." },
            { "ui.looperAuto", "AUTO\nSans métronome, la première boucle t'attend : après le clic sur son pad, "
                               "l'enregistrement démarre à la première note que tu joues, sans blanc au début de la boucle. "
                               "Éteint : l'enregistrement démarre au clic." },
            { "ui.looperExport", "EXPORT WAV\nEnregistre le mélange des pistes qui tournent en un fichier WAV, "
                                 "dans Documents/Bass Amp/Recordings." },
            { "ui.looperTrackStop", "STOP\nArrête cette piste sans l'effacer. Un clic sur son pad la relance, en place." },
            { "ui.looperTrackClear", "CLEAR\nEfface cette piste." },
            { "ui.looperTrim", "TRIM\nCoupe le début ou la fin de la boucle, pour un clic sur le pad arrivé un peu tôt ou "
                               "tard. Glisse les poignées sur la forme d'onde pendant qu'elle tourne. Seulement tant que cette "
                               "piste est la seule." },
            { "ui.looperTrimReset", "RESET\nRejoue la prise entière, telle qu'enregistrée." },
            { "ui.looperTrimDone", "DONE\nFerme la vue de découpe. La boucle garde son nouveau début et sa nouvelle fin." },
            { "ui.looperReverse", "REV\nJoue la piste à l'envers." },
            { "ui.looperHalf", "HALF\nJoue la piste deux fois plus lentement : une octave plus bas." },
            { "ui.looperLevel", "LEVEL\nVolume de la piste." },
            { "ui.metronome", "MÉTRONOME\nUn clic pour travailler en rythme. Il n'est jamais enregistré par REC." },
            { "ui.metronomeOn", "ON\nLance ou arrête le métronome." },
            { "ui.metronomeTempo", "TEMPO\nVitesse en battements par minute (BPM). Dans un DAW, le métronome et le looper "
                                   "suivent le tempo du morceau." },
            { "ui.metronomeTap", "TAP\nTape le tempo pour régler le métronome." },
            { "ui.metronomeVolume", "VOL\nVolume du clic." },
        },
        {
            { "no file loaded", "aucun fichier chargé" },
            { "No capture loaded", "Aucune capture chargée" },
            { "Can't read this file", "Fichier illisible" },
            { "Set audio to", "Règle l'audio sur" },
            { "Load a Neural Amp Modeler capture", "Charger une capture Neural Amp Modeler" },
            { "Load a cabinet impulse response", "Charger une réponse impulsionnelle de baffle" },
            { "Saved", "Enregistré :" },
            { "Export failed", "Échec de l'export" },
            { "Synced to the metronome: loops start and end on a bar.", "Calé sur le métronome : les boucles démarrent et finissent sur une mesure." },
            { "Free: the first loop sets the length the others follow.", "Libre : la première boucle donne la longueur que suivent les autres." },
            { "MIDI learn", "MIDI learn" },
            { "Cancel MIDI learn", "Annuler le MIDI learn" },
            { "Forget", "Oublier" },
            { "MOVE A MIDI CONTROL", "BOUGE UN CONTRÔLE MIDI" },
            { "Choose a backing track", "Choisir un morceau" },
            { "No track loaded", "Aucun morceau chargé" },
            { "Factory", "Usine" },
            { "User", "Les miens" },
            { "Save preset", "Enregistrer le preset" },
            { "Preset name:", "Nom du preset :" },
            { "copy", "copie" },
            { "Save", "Enregistrer" },
            { "Cancel", "Annuler" },
            { "No audio device: Options > Audio/MIDI Settings", "Aucun périphérique audio : Options > Réglages audio/MIDI" },
            { "latency", "latence" },
            { "Language", "Langue" },
            { "Drag the handles to cut the start or the end of the loop. The whole take is kept.", "Glisse les poignées pour couper le début ou la fin de la boucle. La prise entière est gardée." },
            { "Synced to the metronome: a bar of count-in, then loops start and end on a bar.", "Calé sur le métronome : une mesure de décompte, puis les boucles démarrent et finissent sur une mesure." },
            { "Free: recording starts on your first note; the first loop sets the length the others follow.", "Libre : l'enregistrement démarre à ta première note ; la première boucle donne la longueur que suivent les autres." },
            { "available", "disponible" },
            { "Rename...", "Renommer..." },
            { "Delete...", "Supprimer..." },
            { "Import...", "Importer..." },
            { "Export...", "Exporter..." },
            { "Open presets folder", "Ouvrir le dossier des presets" },
            { "Rename preset", "Renommer le preset" },
            { "Rename", "Renommer" },
            { "Delete preset?", "Supprimer le preset ?" },
            { "Delete", "Supprimer" },
            { "Import a preset", "Importer un preset" },
            { "Export this sound", "Exporter ce son" },
            { "This file isn't a Bass Amp preset.", "Ce fichier n'est pas un preset Bass Amp." },
            { "File not found", "Fichier introuvable" },
            { "Replace preset?", "Remplacer le preset ?" },
            { "A preset with this name already exists.", "Un preset porte déjà ce nom." },
            { "Replace", "Remplacer" },
            { "Audio input is muted to avoid feedback loop", "L'entrée audio est coupée pour éviter le larsen" },
            { "Settings...", "Réglages..." },
            { "Input", "Entrée" },
            { "Options", "Options" },
            { "Audio/MIDI Settings", "Réglages audio/MIDI" },
            { "Audio/MIDI Settings...", "Réglages audio/MIDI..." },
            { "Save current state", "Enregistrer l'état actuel" },
            { "Save current state...", "Enregistrer l'état actuel..." },
            { "Load a saved state", "Charger un état enregistré" },
            { "Load a saved state...", "Charger un état enregistré..." },
            { "Reset to default state", "Revenir à l'état par défaut" },
            { "Audio device type:", "Type de pilote audio :" },
            { "Device:", "Périphérique :" },
            { "Output:", "Sortie :" },
            { "Input:", "Entrée :" },
            { "Active output channels:", "Canaux de sortie actifs :" },
            { "Active input channels:", "Canaux d'entrée actifs :" },
            { "Sample rate:", "Fréquence d'échantillonnage :" },
            { "Audio buffer size:", "Taille du buffer audio :" },
            { "Active MIDI inputs:", "Entrées MIDI actives :" },
            { "MIDI Output:", "Sortie MIDI :" },
            { "Test", "Test" },
            { "Plays a test tone", "Joue un son de test" },
            { "Control Panel", "Panneau de contrôle" },
            { "Opens the device's own control panel", "Ouvre le panneau de réglages du périphérique" },
            { "Reset Device", "Réinitialiser" },
            { "Show advanced settings...", "Afficher les réglages avancés..." },
            { "No MIDI inputs available", "Aucune entrée MIDI disponible" },
            { "(no audio input channels found)", "(aucun canal d'entrée audio trouvé)" },
            { "(no audio output channels found)", "(aucun canal de sortie audio trouvé)" },
            { "none", "aucun" },
            { "Error when trying to open audio device!", "Erreur à l'ouverture du périphérique audio !" },
            { "Error whilst loading", "Erreur pendant le chargement" },
            { "Error whilst saving", "Erreur pendant l'enregistrement" },
            { "Couldn't read from the specified file!", "Impossible de lire ce fichier !" },
            { "Couldn't write to the specified file!", "Impossible d'écrire ce fichier !" },
            { "OK", "OK" },
            { "Unsaved changes", "Modifications non enregistrées" },
            { "Your changes to this sound will be lost.", "Tes modifications de ce son seront perdues." },
            { "Load anyway", "Charger quand même" },
        },
    };

    return translations;
}
