#include "Translations.h"

const Translations& germanTranslations()
{
    static const Translations translations {
        {
            // Pedals
            { "pedal.gate", "GATE (Noise Gate)\nSchaltet den Ton stumm, wenn du nicht spielst, gegen Rauschen und Tonabnehmer-Brummen. "
                            "Setz es an den Anfang der Kette, vor allem vor Drive oder Fuzz, die das Rauschen verstärken." },
            { "pedal.comp", "COMP (Kompressor)\nGleicht die Lautstärke aus: laute Noten werden leiser, leise treten hervor. "
                            "Der Klang wird gleichmäßiger und präsenter. Unverzichtbar für Slap und einen Studiosound." },
            { "pedal.octaver", "OCTAVER\nFügt deiner Note die Oktave darunter (Sub) und darüber (Up) hinzu. "
                               "Spiel Einzeltöne: Akkorde verwirren ihn. Ideal für einen riesigen Sound oder eine Synth-Linie." },
            { "pedal.synth", "SYNTH\nMacht aus deinem Bass einen Synthesizer: ein Oszillator folgt deiner Note durch einen Filter, "
                             "der sich beim Anschlag öffnet. Spiel jeweils eine saubere Note, damit er folgen kann." },
            { "pedal.pitch", "PITCH (Pitch Shifter)\nVerschiebt deine Note um -12 bis +12 Halbtöne, gemischt mit dem Direktsignal. "
                             "+12 für eine Oktave höher, +7 für eine Quinte, etwas Detune zum Andicken." },
            { "pedal.filter", "FILTER\nResonanzfilter, von Hand eingestellt oder von einem LFO (einer langsamen Schwingung) bewegt. "
                              "Tiefpass für einen dunklen, runden Klang, Hochpass für einen dünnen, Bandpass für einen «Radio»-Effekt." },
            { "pedal.envelope", "ENVELOPE (Auto-Wah)\nEin Filter, der sich mit deinem Anschlag öffnet: je härter du spielst, desto heller "
                                "der Klang. Der Funk-Sound schlechthin (Bootsy Collins, Mark Adams)." },
            { "pedal.wah", "WAH\nEin Filter, den man mit dem Fuß bewegt und der «wah» macht. Hier stellt ihn der Pedal-Regler ein, oder ein "
                           "MIDI-Expressionpedal (standardmäßig CC 11). Clean behält Direktsignal, damit die Tiefen bleiben." },
            { "pedal.fuzz", "FUZZ\nSehr starke, eckige Verzerrung wie ein Vintage-Silizium-Pedal. Blend mischt cleanen Klang "
                            "darunter, damit die Tiefen stabil bleiben." },
            { "pedal.drive", "DRIVE (Overdrive)\nWeichere, natürlichere Verzerrung als Fuzz, wie ein Verstärker, der heiß läuft. "
                             "Blend mischt cleanen Klang für mehr Definition bei." },
            { "pedal.phaser", "PHASER\nErzeugt wandernde Kerben im Spektrum: ein kreisender, flüssiger, typischer Siebziger-Sound." },
            { "pedal.flanger", "FLANGER\nWie der Phaser, aber metallischer: der Klang läuft durch eine sehr kurze, sich ändernde Verzögerung. "
                               "Mit viel Regen entsteht der «Düsenjet»-Effekt." },
            { "pedal.chorus", "CHORUS\nDoppelt deinen Klang mit einer leicht verstimmten, sich bewegenden Kopie, um ihn breiter zu machen. "
                              "Die Tiefen bleiben mittig und fest; nur der obere Teil des Spektrums wird bearbeitet." },
            { "pedal.tremolo", "TREMOLO\nLässt die Lautstärke im Rhythmus schwanken, wie alte Verstärker. Shape reicht von sanftem Wogen "
                               "bis zu hartem Zerhacken." },
            { "pedal.delay", "DELAY (Echo)\nWiederholt dein Spiel nach einer einstellbaren Zeit. TAP: tippe das Tempo, um das Echo an den Song anzupassen." },
            { "pedal.reverb", "REVERB\nSimuliert einen Raum: der Klang klingt nach und bekommt Weite. "
                              "Am Bass sparsam einsetzen, damit die Tiefen nicht verschwimmen." },

            // Gate
            { "gate_threshold", "THRESH (Schwelle)\nPegel, unter dem das Gate stummschaltet. Erhöhe ihn, bis das Rauschen in Spielpausen "
                                "verschwindet, ohne das Ende deiner Noten abzuschneiden." },
            { "gate_release", "RELEASE\nWie schnell das Gate schließt. Kurz: schneidet sauber ab; lang: lässt Noten natürlich ausklingen." },

            // Compressor
            { "comp_threshold", "THRESH (Schwelle)\nPegel, ab dem der Kompressor arbeitet. Je niedriger, desto stärker die Kompression. "
                                "Beobachte die COMP-Anzeige unter LEVELS: 3 bis 6 dB Reduktion sind ein guter Anfang." },
            { "comp_ratio", "RATIO\nStärke der Kompression über der Schwelle. 2:1 bis 4:1: natürlich; ab 8:1: sehr gleichmäßig, "
                            "fast ein Limiter." },
            { "comp_attack", "ATTACK\nReaktionsgeschwindigkeit. Langsam (ab 10 ms): lässt den Anschlag durch, mehr Punch; "
                             "schnell: drückt den Anschlag platt." },
            { "comp_release", "RELEASE\nZeit, bis die Kompression nach einer Note nachlässt. Zu kurz: der Klang «pumpt»; zu lang: "
                              "die folgenden Noten bleiben komprimiert." },
            { "comp_makeup", "MAKEUP\nHebt die Lautstärke nach der Kompression wieder an, auf den Pegel wie bei ausgeschaltetem Pedal." },
            { "comp_mix", "MIX\nMischt das komprimierte mit dem direkten Signal (Parallelkompression): der natürliche Anschlag bleibt "
                          "und es gewinnt an Dichte." },

            // Octaver
            { "oct_sub", "SUB\nLautstärke der unteren Oktave. Für einen sehr tiefen, synthartigen Klang." },
            { "oct_up", "UP\nLautstärke der oberen Oktave, die einen hellen, hohlen Klang hinzufügt." },
            { "oct_dry", "DRY\nLautstärke deines Originalklangs in der Mischung." },

            // Envelope
            { "env_type", "TYPE\nFilterform: LP (Tiefpass, das klassische Wah), BP (Bandpass, nasaler) oder HP (Hochpass, dünner). "
                          "Klicken zum Wechseln." },
            { "env_dir", "SWEEP\nUp: der Filter öffnet sich beim Anschlag. Down: er schließt sich, für ein umgekehrtes «wouw»." },
            { "env_sens", "SENS (Empfindlichkeit)\nWie stark dein Anschlag den Filter öffnet. Stell sie nach deiner Spielstärke ein." },
            { "env_reso", "RESO (Resonanz)\nBetont die Filterfrequenz. Je höher, desto deutlicher das «Quaken»." },
            { "env_mix", "MIX\nMischt den Effekt mit deinem Direktsignal, damit die Tiefen bleiben." },

            // Fuzz and drive
            { "fuzz_amount", "FUZZ\nStärke der Verzerrung." },
            { "fuzz_tone", "TONE\nLinks dunkel, rechts aggressiv." },
            { "fuzz_blend", "BLEND\nMischt den Fuzz mit dem cleanen Klang. Nach links bleiben die Tiefen sauber." },
            { "dist_drive", "DRIVE\nStärke der Verzerrung, von leichtem Crunch bis zur echten Zerre." },
            { "dist_tone", "TONE\nNimmt Höhen aus der Verzerrung. Niedriger: runder; höher: bissiger." },
            { "dist_blend", "BLEND\nMischt die Verzerrung mit dem cleanen Klang, damit die Noten definiert bleiben." },

            // Filter
            { "filter_type", "TYPE\nLP: Tiefpass (behält die Tiefen); BP: Bandpass (behält einen Bereich); HP: Hochpass (behält die Höhen). "
                             "12 oder 24: sanfte oder steile Flanke. Klicken zum Wechseln." },
            { "filter_cutoff", "CUTOFF (Grenzfrequenz)\nWo der Filter arbeitet. Beim Tiefpass macht ein niedrigerer Wert den Klang dunkler." },
            { "filter_reso", "RESO (Resonanz)\nBetont die Grenzfrequenz: der Klang wird «synthiger» und pfeifender." },
            { "filter_rate", "RATE\nGeschwindigkeit des LFO, der die Grenzfrequenz bewegt. Ohne Wirkung, wenn Depth auf null steht." },
            { "filter_depth", "DEPTH\nWeite der LFO-Bewegung. Auf null bleibt der Filter fest." },

            // Wah
            { "wah_pedal", "PEDAL\nPedalstellung: Ferse (tief) bis Spitze (hoch). Steuerbar über ein MIDI-Expressionpedal." },
            { "wah_reso", "RESO\nStärke der Wah-Spitze. Hoch: sehr vokal; niedrig: dezenter." },
            { "wah_blend", "CLEAN\nAnteil des Direktsignals unter dem Wah, damit die Tiefen nicht verloren gehen." },

            // Synth
            { "synth_wave", "WAVE\nOszillatorform: Square (Rechteck, hohl und rund) oder Saw (Sägezahn, reich und hell). "
                            "Klicken zum Wechseln." },
            { "synth_cutoff", "CUTOFF\nFilteröffnung in Ruhe. Niedrig: dunkler Klang; hoch: heller Klang." },
            { "synth_reso", "RESO\nFilterresonanz, für den «Acid»-Charakter und das Pfeifen." },
            { "synth_env", "ENV\nWie stark dein Anschlag den Filter öffnet. Hoch: jede Note macht «wow»." },
            { "synth_mix", "MIX\nMischt den Synth mit deinem Bass." },

            // Pitch
            { "pitch_shift", "SHIFT\nVerschiebung in Halbtönen: +12 = Oktave höher, +7 = Quinte, -12 = Oktave tiefer." },
            { "pitch_detune", "DETUNE\nFeinverstimmung in Cent. Ein paar Cent machen den Klang dicker, wie ein Chorus." },
            { "pitch_mix", "MIX\nMischt die verschobene Note mit deiner Originalnote." },

            // Amp
            { "amp_gain", "GAIN\nWie stark die Vorstufe angefahren wird. Niedrig: clean; um 5: leichter Crunch; darüber: verzerrt. "
                          "Bei NAM Capture der Eingangspegel des Captures." },
            { "amp_bass", "BASS\nTiefen: Rundheit und Gewicht." },
            { "amp_mid", "MIDDLE\nMitten: was den Bass im Mix hervortreten lässt. Absenken ergibt einen ausgehöhlten Klang, Anheben einen durchsetzungsfähigen." },
            { "amp_mid_freq", "FREQ\nFrequenz, auf die Middle wirkt: tief (200 Hz) für Fülle, hoch (1 bis 3 kHz) für den Anschlag. "
                              "Inaktiv beim Tweed '59, dessen Schaltung die Frequenzen festlegt." },
            { "amp_treble", "TREBLE\nHöhen: Klarheit und Saitengeräusche." },
            { "amp_master", "MASTER\nGesamtlautstärke des Verstärkers. Wenn die OUT-CLIP-Lampe aufleuchtet, dreh ihn zurück." },
            { "amp_blend", "DI BLEND\nMischt das Direktsignal des Basses (vor dem Verstärker) mit Verstärker und Box, wie im Studio: "
                           "das DI bringt saubere Tiefen, der Verstärker den Charakter." },
            { "amp_contour", "CONTOUR\nSenkt die Mitten um 380 Hz ab und öffnet Tiefen und Höhen: der «Smiley»-Sound, ideal für Slap." },
            { "amp_deep", "DEEP\nFügt Tiefen hinzu und nimmt etwas 500 Hz heraus, für einen tieferen Klang." },
            { "amp_bright", "BRIGHT\nFügt Höhen hinzu, für mehr Klarheit und Biss." },
            { "amp_low_cut", "LO CUT\nSchneidet die Subbässe am Verstärkerausgang ab. Nützlich, um den Klang zu straffen, oder für kleine Boxen, "
                             "die im Bass verzerren." },
            { "amp_high_cut", "HI CUT\nSchneidet die Höhen am Verstärkerausgang ab, um einen zu hellen Klang oder eine kratzige Verzerrung zu glätten." },

            // Amp models
            { "model.Italian Hi-Fi", "ITALIAN HI-FI\nNeutraler, moderner Transistorverstärker, inspiriert vom Markbass Little Mark. Schnell und präzise, ideal für Slap und Funk." },
            { "model.Super Valve", "SUPER VALVE\nVollröhre mit 300 W, inspiriert vom Ampeg SVT: warme Tiefmitten, runde Höhen, der Rock-Sound." },
            { "model.Nordic Drive", "NORDIC DRIVE\nInspiriert vom Darkglass B7K: die Tiefen bleiben clean, nur der obere Bereich zerrt. Der moderne Metal-Sound." },
            { "model.Tweed '59", "TWEED '59\nInspiriert vom Fender Bassman von 1959, mit modellierter passiver Klangregelung: rund, vintage, zerrt früh." },
            { "model.Flip Top '60s", "FLIP TOP '60S\nKleiner Studio-Röhrenverstärker, inspiriert vom Ampeg B-15: dunkel und rund, der Motown-Sound." },
            { "model.West Coast 800", "WEST COAST 800\nRock-Transistorverstärker, inspiriert vom Gallien-Krueger 800RB: ausgehöhlte Mitten, Biss bei 4 kHz." },
            { "model.Stomp DI", "STOMP DI\nDI-Vorstufe, inspiriert vom SansAmp Bass Driver: direkt und druckvoll, von clean bis angezerrt." },
            { "model.NAM Capture", "NAM CAPTURE\nSpielt ein Capture eines echten Verstärkers (eine .nam-Datei von Neural Amp Modeler), "
                                   "kostenlos auf TONE3000. Lade es mit LOAD NAM." },

            // Cabinets
            { "cab.Off (DI)", "OFF (DI)\nKeine Box: der Direktklang des Verstärkers, wie ein DI-Ausgang." },
            { "cab.8x10", "8x10\nDer große Kühlschrank der Rockkonzerte: kräftige Tiefen, präsente Mitten." },
            { "cab.4x10", "4x10\nVielseitig und druckvoll, mit mehr Attack als die 8x10." },
            { "cab.1x15", "1x15\nEin einzelner großer Lautsprecher: rund, tief, vintage, wenig Höhen." },
            { "cab.2x12", "2x12\nKompakt und mittig, setzt sich in einer Band gut durch." },
            { "cab.2x10 Hi-Fi", "2x10 HI-FI\nMit Hochtöner: breiter, klarer Klang für Slap und modernen Jazz." },
            { "cab.IR File", "IR FILE\nSpielt deine eigene Box-Impulsantwort (IR), geladen mit LOAD IR." },

            // EQ
            { "eq_on", "ON\nSchaltet den Equalizer ein oder aus. Er schaltet sich von selbst ein, sobald du einen Fader oder Punkt berührst." },
            { "cut_low", "CUT 80 HZ\nSchneidet alles unter 80 Hz auf einmal ab: der Klang wird straffer und lässt der Bassdrum Platz." },
            { "kill_low", "KILL LOW\nEntfernt die Tiefen (unter 250 Hz). Für einen Live-Drop oder um den Anschlag zu isolieren." },
            { "kill_mid", "KILL MID\nEntfernt die Mitten (250 Hz bis 2,5 kHz): nur Tiefen und Snap bleiben übrig." },
            { "kill_high", "KILL HIGH\nEntfernt die Höhen (über 2,5 kHz): dunkler, runder Klang wie ein Bass mit Flatwound-Saiten." },
            { "cut_high", "CUT 5 KHZ\nSchneidet alles über 5 kHz auf einmal ab: macht den Klang weicher und entfernt Rauschen." },
            { "geq_33", "33 Hz\nSubbass: das Grollen, das man mehr spürt als hört. Vorsichtig anheben." },
            { "geq_63", "63 Hz\nGewicht und Fundament des Klangs." },
            { "geq_125", "125 Hz\nRundheit. Zu viel: der Klang dröhnt." },
            { "geq_250", "250 Hz\nFülle. Zu viel: der Klang wird matschig." },
            { "geq_500", "500 Hz\nKörper und der «pappige» Anteil. Absenken öffnet den Klang." },
            { "geq_1k", "1 kHz\nPräsenz: was den Bass im Mix hörbar macht." },
            { "geq_2k", "2 kHz\nAnschlag und Körnung der Noten." },
            { "geq_4k", "4 kHz\nSaitensnap und Fingergeräusche." },
            { "geq_8k", "8 kHz\nGlanz und Rauschen." },
            { "peq_low_cut", "LOW CUT\nGrauer Punkt links: schneidet alles darunter ab. Zieh ihn nach rechts, um die Tiefen zu straffen. "
                             "Er bewegt sich nur nach links und rechts: er hat keine Verstärkung." },
            { "peq_low_freq", "LOW (Low Shelf)\nHebt oder senkt alle Tiefen unter seiner Frequenz. Nach oben oder unten ziehen für die Verstärkung, "
                              "nach links oder rechts für die Frequenz." },
            { "peq_mid1_freq", "LO-MID\nTiefmittenband: Fülle und Matsch. Ziehen zum Einstellen, Mausrad für die Breite (Q), "
                               "Doppelklick zum Zurücksetzen." },
            { "peq_mid2_freq", "HI-MID\nHochmittenband: Anschlag und Präsenz. Ziehen zum Einstellen, Mausrad für die Breite (Q), "
                               "Doppelklick zum Zurücksetzen." },
            { "peq_high_freq", "HIGH (High Shelf)\nHebt oder senkt alle Höhen über seiner Frequenz." },
            { "peq_high_cut", "HIGH CUT\nGrauer Punkt rechts: schneidet alles darüber ab. Zieh ihn nach links, um den Klang weicher zu machen. "
                              "Er bewegt sich nur nach links und rechts: er hat keine Verstärkung." },

            // Post-amp effects
            { "phaser_rate", "RATE\nGeschwindigkeit der Bewegung." },
            { "phaser_depth", "DEPTH\nWeite der Bewegung." },
            { "phaser_feedback", "REGEN\nRückkopplung: je höher, desto ausgeprägter und singender der Effekt." },
            { "flanger_rate", "RATE\nGeschwindigkeit der Bewegung." },
            { "flanger_depth", "DEPTH\nWeite der Bewegung." },
            { "flanger_feedback", "REGEN\nRückkopplung: je höher, desto metallischer der Klang." },
            { "flanger_mix", "MIX\nMischt den Effekt mit dem Direktsignal." },
            { "chorus_rate", "RATE\nGeschwindigkeit des Schwebens. Langsam: breit und weich; schnell: Vibrato-Effekt." },
            { "chorus_depth", "DEPTH\nStärke der Verstimmung." },
            { "chorus_mix", "MIX\nMischt den Effekt mit dem Direktsignal." },
            { "tremolo_rate", "RATE\nGeschwindigkeit der Lautstärkeänderungen." },
            { "tremolo_depth", "DEPTH\nStärke der Änderungen: voll aufgedreht wird der Klang bei jedem Zyklus unterbrochen." },
            { "tremolo_shape", "SHAPE\nWellenform: links sanftes Wogen, rechts hartes Zerhacken." },
            { "delay_time", "TIME\nZeit zwischen deinem Spiel und dem Echo, in Millisekunden. Oder tippe das Tempo mit TAP." },
            { "delay_feedback", "REPEAT\nAnzahl der Wiederholungen: niedrig, ein einzelnes Echo; hoch, lang anhaltende Echos." },
            { "delay_mix", "MIX\nLautstärke der Echos." },
            { "reverb_size", "SIZE\nRaumgröße: vom kleinen Zimmer bis zur großen Kirche." },
            { "reverb_damping", "DAMP\nDämpfung der Höhen: hoch für einen dunklen, dezenten Hall." },
            { "reverb_mix", "MIX\nHallanteil." },
            { "limiter_on", "LIMIT\nAusgangslimiter: hält den Klang unter -1 dBFS, um deine Ohren zu schützen und digitales Clipping zu vermeiden. "
                            "Transparent, solange du unter diesem Pegel bleibst." },

            // Interface
            { "ui.footswitch", "FUSSSCHALTER\nSchaltet das Pedal ein oder aus: die LED leuchtet, wenn es aktiv ist. "
                               "Rechtsklick: MIDI Learn, um es mit einem Pad deines Controllers zu schalten. Ein goldener "
                               "Punkt an einem Regler zeigt, dass ein MIDI-Regler ihn steuert." },
            { "ui.tap", "TAP\nTippe mindestens zweimal im Rhythmus: die Delay-Zeit passt sich deinem Tempo an." },
            { "ui.pedalPresets", "PEDAL-PRESETS\nFertige Einstellungen für dieses Pedal. Der Rest deines Sounds bleibt unverändert." },
            { "ui.removePedal", "ENTFERNEN\nSchaltet das Pedal aus und räumt es weg. Mit + ADD holst du es zurück." },
            { "ui.addPedal", "+ ADD\nFügt diesem Pedalboard ein Pedal hinzu. Danach ziehst du ein Pedal, um es zu verschieben: "
                             "der Klang folgt der angezeigten Reihenfolge, von links nach rechts." },
            { "ui.presetBox", "PRESETS\nDie Werkssounds und deine eigenen. Wähle einen, um alles auf einmal einzustellen. Ein "
                              "goldener Stern zeigt, dass du den Sound seit dem Laden geändert hast: speichere ihn mit SAVE, "
                              "um ihn zu behalten." },
            { "ui.previousPreset", "VORHERIGES PRESET" },
            { "ui.nextPreset", "NÄCHSTES PRESET" },
            { "ui.savePreset", "SAVE\nSpeichert den aktuellen Sound als Preset in Dokumente/Bass Amp/Presets." },
            { "ui.record", "REC\nNimmt auf, was aus der App kommt (mit Effekten), als WAV in Dokumente/Bass Amp/Recordings. "
                           "Erneut klicken zum Stoppen." },
            { "ui.recordings", "FILES\nÖffnet den Ordner mit den Aufnahmen." },
            { "ui.tuner", "STIMMGERÄT\nSpiel eine leere Saite: die Note erscheint und die Nadel steht mittig, wenn die Saite "
                          "stimmt. E A D G, und tiefes H (B) beim Fünfsaiter. Mausrad auf dem Display: Kammerton A von 430 bis "
                          "450 Hz; Doppelklick: zurück auf 440 Hz." },
            { "ui.mute", "MUTE\nSchaltet den Ausgang stumm, während du stimmst." },
            { "ui.meters", "PEGEL\nIN: was hereinkommt (dein Bass); OUT: was hinausgeht; COMP: Reduktion des Kompressors. "
                           "Rotes CLIP: Übersteuerung, senke den Gain deines Interfaces (IN) oder den Master (OUT). Klicken setzt die Lampen zurück." },
            { "ui.eqPresets", "EQ-PRESETS\nFertige Kurven: Gesang, Gitarre, runder Bass, Slap… Nur der Equalizer ändert sich." },
            { "ui.eqBasic", "BASIC\nGrafischer 9-Band-Equalizer mit Fadern." },
            { "ui.eqAdvanced", "ADVANCED\nParametrischer Equalizer: 6 Punkte zum Ziehen auf der Kurve, präziser." },
            { "ui.eqScreen", "EQ-ANZEIGE\nDie Kurve zeigt die Korrektur, der Hintergrund das Spektrum des Ausgangs. "
                             "Erkenne Frequenzbuckel, um sie zu korrigieren." },
            { "ui.ampPresets", "AMP-PRESETS\nFertige Verstärker- und Boxeneinstellungen. Pedale und EQ bleiben unverändert." },
            { "ui.power", "POWER\nDie Betriebsleuchte, wie an einem echten Verstärker: sie leuchtet, solange die App läuft. "
                          "Hier gibt es nichts einzustellen." },
            { "ui.language", "SPRACHE\nDie Sprache der Hilfeblasen und Menüs. Die Reglernamen bleiben auf Englisch, "
                             "wie auf echtem Equipment." },
            { "ui.fullScreen", "VOLLBILD\nVergrößert die App auf den ganzen Bildschirm. F11 macht dasselbe, Escape führt zurück ins Fenster." },
            { "ui.loadIr", "LOAD IR\nLädt eine Box-Impulsantwort (WAV, AIFF, FLAC) und stellt die Box auf IR File." },
            { "ui.loadNam", "LOAD NAM\nLädt ein .nam-Verstärker-Capture (TONE3000 bietet Tausende). "
                            "Die Klangregler und die Box bleiben danach aktiv." },
            { "ui.player", "PLAYER\nSpielt einen Song oder eine Spur ab, zu der du spielen kannst." },
            { "ui.playerLoad", "LOAD\nÖffnet eine Audiodatei (WAV, AIFF, FLAC, OGG, MP3) und spielt sie ab." },
            { "ui.playerPlay", "PLAY / PAUSE" },
            { "ui.playerStop", "STOP\nStoppt und springt zum Anfang." },
            { "ui.playerPosition", "POSITION\nZiehen, um im Song zu springen." },
            { "ui.playerLoop", "LOOP\nSpielt den Song in Schleife." },
            { "ui.reamp", "RE-AMP\nSchickt die Datei statt deines Basses in die Kette: um eine aufgenommene DI-Spur "
                          "durch Pedale und Verstärker zu schicken und Einstellungen ohne Spielen zu testen." },
            { "ui.playerVolume", "VOL\nLautstärke des Songs." },
            { "ui.looper", "LOOPER\n4 Loop-Spuren, um live einen Song aufzubauen. Klick auf ein Pad zum Aufnehmen, "
                           "OPEN für den vollständigen Looper." },
            { "ui.loopPad", "SPUR\n1. Klick: Aufnahme. 2. Klick: der Loop läuft. Danach fügt jeder Klick eine Schicht hinzu (DUB) "
                            "oder kehrt zur Wiedergabe zurück. Rechtsklick: MIDI Learn, um sie mit einem Pad oder Fußschalter auszulösen." },
            { "ui.looperOpen", "OPEN\nÖffnet den vollständigen Looper: Einstellungen jeder Spur, Rückgängig, Export." },
            { "ui.looperClose", "CLOSE\nSchließt den Looper: die Loops laufen weiter." },
            { "ui.looperStopAll", "STOP ALL\nStoppt alle Spuren oder startet sie neu, wenn alle gestoppt sind. Sie kommen im Takt zurück." },
            { "ui.looperClearAll", "CLEAR ALL\nLöscht alle Spuren." },
            { "ui.looperUndo", "UNDO\nMacht die letzte Aufnahme oder die zuletzt hinzugefügte Schicht rückgängig." },
            { "ui.looperRedo", "REDO\nStellt wieder her, was UNDO gerade entfernt hat." },
            { "ui.looperSync", "SYNC\nWenn das Metronom läuft, beginnen und enden Loops auf einem Takt: "
                               "starte die Aufnahme und sie wartet auf den nächsten Takt. "
                               "Sonst bestimmt der erste Loop die Länge, der die anderen folgen." },
            { "ui.looperExport", "EXPORT WAV\nSpeichert die Mischung der laufenden Spuren als WAV-Datei "
                                 "in Dokumente/Bass Amp/Recordings." },
            { "ui.looperTrackStop", "STOP\nStoppt diese Spur, ohne sie zu löschen. Ein Klick auf ihr Pad startet sie wieder, im Takt." },
            { "ui.looperTrackClear", "CLEAR\nLöscht diese Spur." },
            { "ui.looperReverse", "REV\nSpielt die Spur rückwärts." },
            { "ui.looperHalf", "HALF\nSpielt die Spur mit halber Geschwindigkeit: eine Oktave tiefer." },
            { "ui.looperLevel", "LEVEL\nLautstärke der Spur." },
            { "ui.metronome", "METRONOM\nEin Klick, um im Takt zu üben. Er wird nie von REC aufgenommen." },
            { "ui.metronomeOn", "ON\nStartet oder stoppt das Metronom." },
            { "ui.metronomeTempo", "TEMPO\nGeschwindigkeit in Schlägen pro Minute (BPM). In einer DAW folgen Metronom und "
                                   "Looper dem Tempo des Songs." },
            { "ui.metronomeTap", "TAP\nTippe das Tempo, um das Metronom einzustellen." },
            { "ui.metronomeVolume", "VOL\nLautstärke des Klicks." },
        },
        {
            { "no file loaded", "keine Datei geladen" },
            { "No capture loaded", "Kein Capture geladen" },
            { "Can't read this file", "Datei nicht lesbar" },
            { "Set audio to", "Stell das Audio auf" },
            { "Load a Neural Amp Modeler capture", "Neural-Amp-Modeler-Capture laden" },
            { "Load a cabinet impulse response", "Box-Impulsantwort laden" },
            { "Saved", "Gespeichert:" },
            { "Export failed", "Export fehlgeschlagen" },
            { "Synced to the metronome: loops start and end on a bar.", "Mit dem Metronom synchron: Loops beginnen und enden auf einem Takt." },
            { "Free: the first loop sets the length the others follow.", "Frei: der erste Loop bestimmt die Länge, der die anderen folgen." },
            { "MIDI learn", "MIDI Learn" },
            { "Cancel MIDI learn", "MIDI Learn abbrechen" },
            { "Forget", "Vergessen" },
            { "MOVE A MIDI CONTROL", "BEWEGE EINEN MIDI-REGLER" },
            { "Choose a backing track", "Song auswählen" },
            { "No track loaded", "Kein Song geladen" },
            { "Factory", "Werk" },
            { "User", "Eigene" },
            { "Save preset", "Preset speichern" },
            { "Preset name:", "Name des Presets:" },
            { "copy", "Kopie" },
            { "Save", "Speichern" },
            { "Cancel", "Abbrechen" },
            { "No audio device: Options > Audio/MIDI Settings", "Kein Audiogerät: Optionen > Audio/MIDI-Einstellungen" },
            { "latency", "Latenz" },
            { "Language", "Sprache" },
            { "Options", "Optionen" },
            { "Audio/MIDI Settings", "Audio/MIDI-Einstellungen" },
            { "Audio/MIDI Settings...", "Audio/MIDI-Einstellungen..." },
            { "Save current state", "Aktuellen Zustand speichern" },
            { "Save current state...", "Aktuellen Zustand speichern..." },
            { "Load a saved state", "Gespeicherten Zustand laden" },
            { "Load a saved state...", "Gespeicherten Zustand laden..." },
            { "Reset to default state", "Auf Standard zurücksetzen" },
            { "Audio device type:", "Audiotreiber-Typ:" },
            { "Device:", "Gerät:" },
            { "Output:", "Ausgang:" },
            { "Input:", "Eingang:" },
            { "Active output channels:", "Aktive Ausgangskanäle:" },
            { "Active input channels:", "Aktive Eingangskanäle:" },
            { "Sample rate:", "Abtastrate:" },
            { "Audio buffer size:", "Audio-Puffergröße:" },
            { "Active MIDI inputs:", "Aktive MIDI-Eingänge:" },
            { "MIDI Output:", "MIDI-Ausgang:" },
            { "Test", "Test" },
            { "Plays a test tone", "Spielt einen Testton" },
            { "Control Panel", "Systemsteuerung" },
            { "Opens the device's own control panel", "Öffnet das Einstellungsfenster des Geräts" },
            { "Reset Device", "Gerät zurücksetzen" },
            { "Show advanced settings...", "Erweiterte Einstellungen anzeigen..." },
            { "No MIDI inputs available", "Keine MIDI-Eingänge verfügbar" },
            { "(no audio input channels found)", "(keine Audio-Eingangskanäle gefunden)" },
            { "(no audio output channels found)", "(keine Audio-Ausgangskanäle gefunden)" },
            { "none", "keiner" },
            { "Error when trying to open audio device!", "Fehler beim Öffnen des Audiogeräts!" },
            { "Error whilst loading", "Fehler beim Laden" },
            { "Error whilst saving", "Fehler beim Speichern" },
            { "Couldn't read from the specified file!", "Datei nicht lesbar!" },
            { "Couldn't write to the specified file!", "Datei nicht beschreibbar!" },
            { "OK", "OK" },
            { "Unsaved changes", "Ungespeicherte Änderungen" },
            { "Your changes to this sound will be lost.", "Deine Änderungen an diesem Sound gehen verloren." },
            { "Load anyway", "Trotzdem laden" },
        },
    };

    return translations;
}
