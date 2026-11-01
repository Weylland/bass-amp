#include "Translations.h"

const Translations& italianTranslations()
{
    static const Translations translations {
        {
            // Pedals
            { "pedal.gate", "GATE (Noise Gate)\nZittisce il suono quando non suoni, per eliminare il fruscio e il ronzio dei pickup. "
                            "Mettilo all'inizio della catena, soprattutto prima di un drive o di un fuzz, che amplificano il rumore." },
            { "pedal.comp", "COMP (Compressore)\nUniforma il volume: abbassa le note forti e fa emergere quelle deboli. "
                            "Il suono diventa più regolare e presente. Indispensabile per lo slap e per un suono da studio." },
            { "pedal.octaver", "OCTAVER\nAggiunge alla tua nota l'ottava sotto (Sub) e l'ottava sopra (Up). "
                               "Suona una nota alla volta: gli accordi lo confondono. Ideale per un suono enorme o una linea synth." },
            { "pedal.synth", "SYNTH\nTrasforma il basso in un sintetizzatore: un oscillatore segue la tua nota attraverso un filtro "
                             "che si apre con la pennata. Suona una nota pulita alla volta perché possa seguirla." },
            { "pedal.pitch", "PITCH (Pitch Shifter)\nSposta la tua nota da -12 a +12 semitoni, mescolata al suono diretto. "
                             "+12 per un'ottava sopra, +7 per una quinta, un po' di detune per ispessire." },
            { "pedal.filter", "FILTER\nFiltro risonante, regolato a mano o mosso da un LFO (un'oscillazione lenta). "
                              "Passa-basso per un suono scuro e rotondo, passa-alto per un suono sottile, passa-banda per un effetto «radio»." },
            { "pedal.envelope", "ENVELOPE (Auto-Wah)\nUn filtro che si apre con la tua pennata: più suoni forte, più il suono "
                                "diventa brillante. Il suono funk per eccellenza (Bootsy Collins, Mark Adams)." },
            { "pedal.wah", "WAH\nUn filtro che si muove col piede e fa «wah». Qui lo regola la manopola Pedal, oppure un pedale "
                           "d'espressione MIDI (CC 11 di default). Clean mantiene il suono diretto per non perdere le basse." },
            { "pedal.fuzz", "FUZZ\nDistorsione molto forte e squadrata, come un pedale al silicio vintage. Blend aggiunge suono pulito "
                            "sotto per mantenere le basse solide." },
            { "pedal.drive", "DRIVE (Overdrive)\nDistorsione più morbida e naturale del fuzz, come un amplificatore che scalda. "
                             "Blend aggiunge suono pulito per più definizione." },
            { "pedal.phaser", "PHASER\nCrea solchi che si muovono nello spettro: un suono rotante, liquido, tipico degli anni Settanta." },
            { "pedal.flanger", "FLANGER\nCome il phaser ma più metallico: il suono passa in un ritardo molto corto che varia. "
                               "Con molto regen ottieni l'effetto «aereo a reazione»." },
            { "pedal.chorus", "CHORUS\nRaddoppia il suono con una copia leggermente stonata e in movimento, per allargarlo. "
                              "Le basse restano al centro e solide; viene trattata solo la parte alta dello spettro." },
            { "pedal.tremolo", "TREMOLO\nFa variare il volume a tempo, come i vecchi amplificatori. Shape va da un'ondulazione morbida "
                               "a un taglio netto." },
            { "pedal.delay", "DELAY (Eco)\nRipete ciò che suoni dopo un tempo regolabile. TAP: batti il tempo per adattare l'eco al brano." },
            { "pedal.reverb", "REVERB\nSimula una stanza: il suono si prolunga e prende spazio. "
                              "Usalo con moderazione sul basso per non impastare le basse." },

            // Gate
            { "gate_threshold", "THRESH (Soglia)\nLivello sotto il quale il gate zittisce. Alzala finché il fruscio sparisce nelle pause "
                                "senza tagliare la coda delle note." },
            { "gate_release", "RELEASE\nVelocità con cui il gate si chiude. Corto: taglia netto; lungo: lascia spegnere le note in modo naturale." },

            // Compressor
            { "comp_threshold", "THRESH (Soglia)\nLivello da cui il compressore lavora. Più è bassa, più comprime. "
                                "Guarda l'indicatore COMP in LEVELS: 3-6 dB di riduzione sono un buon inizio." },
            { "comp_ratio", "RATIO\nForza della compressione sopra la soglia. Da 2:1 a 4:1: naturale; da 8:1: molto uniforme, "
                            "quasi un limiter." },
            { "comp_attack", "ATTACK\nVelocità di reazione. Lento (da 10 ms): lascia passare l'attacco della nota, più punch; "
                             "veloce: schiaccia l'attacco." },
            { "comp_release", "RELEASE\nTempo perché la compressione si rilasci dopo una nota. Troppo corto: il suono «pompa»; troppo lungo: "
                              "le note seguenti restano compresse." },
            { "comp_makeup", "MAKEUP\nRialza il volume dopo la compressione, per ritrovare il livello del pedale spento." },
            { "comp_mix", "MIX\nMescola il suono compresso con il suono diretto (compressione parallela): mantiene l'attacco naturale "
                          "guadagnando densità." },

            // Octaver
            { "oct_sub", "SUB\nVolume dell'ottava sotto. Per un suono molto grave, tipo synth." },
            { "oct_up", "UP\nVolume dell'ottava sopra, che aggiunge un suono brillante e vuoto." },
            { "oct_dry", "DRY\nVolume del tuo suono originale nel mix." },

            // Envelope
            { "env_type", "TYPE\nForma del filtro: LP (passa-basso, il wah classico), BP (passa-banda, più nasale) o HP (passa-alto, più sottile). "
                          "Clicca per cambiare." },
            { "env_dir", "SWEEP\nUp: il filtro si apre con la pennata. Down: si chiude, per un «wouw» al contrario." },
            { "env_sens", "SENS (Sensibilità)\nQuanto la tua pennata apre il filtro. Regolala in base a quanto forte suoni." },
            { "env_reso", "RESO (Risonanza)\nAccentua la frequenza del filtro. Più è alta, più il «quack» è marcato." },
            { "env_mix", "MIX\nMescola l'effetto con il suono diretto per mantenere le basse." },

            // Fuzz and drive
            { "fuzz_amount", "FUZZ\nQuantità di distorsione." },
            { "fuzz_tone", "TONE\nA sinistra scuro, a destra aggressivo." },
            { "fuzz_blend", "BLEND\nMescola il fuzz con il suono pulito. Verso sinistra le basse restano pulite." },
            { "dist_drive", "DRIVE\nQuantità di distorsione, da un leggero crunch alla vera saturazione." },
            { "dist_tone", "TONE\nToglie acuti alla distorsione. Più basso: più rotondo; più alto: più mordente." },
            { "dist_blend", "BLEND\nMescola la distorsione con il suono pulito per mantenere le note definite." },

            // Filter
            { "filter_type", "TYPE\nLP: passa-basso (tiene le basse); BP: passa-banda (tiene una zona); HP: passa-alto (tiene gli acuti). "
                             "12 o 24: pendenza dolce o ripida. Clicca per cambiare." },
            { "filter_cutoff", "CUTOFF (Frequenza di taglio)\nDove agisce il filtro. In passa-basso, abbassarla scurisce il suono." },
            { "filter_reso", "RESO (Risonanza)\nAccentua la frequenza di taglio: il suono diventa più «synth» e fischiante." },
            { "filter_rate", "RATE\nVelocità dell'LFO che muove la frequenza di taglio. Nessun effetto se Depth è a zero." },
            { "filter_depth", "DEPTH\nAmpiezza del movimento dell'LFO. A zero il filtro resta fisso." },

            // Wah
            { "wah_pedal", "PEDAL\nPosizione del pedale: tallone (grave) fino a punta (acuto). Controllabile da un pedale d'espressione MIDI." },
            { "wah_reso", "RESO\nIntensità del picco del wah. Alta: molto vocale; bassa: più discreto." },
            { "wah_blend", "CLEAN\nParte di suono diretto sotto il wah, per non perdere le basse." },

            // Synth
            { "synth_wave", "WAVE\nForma dell'oscillatore: Square (quadra, vuota e rotonda) o Saw (dente di sega, ricca e brillante). "
                            "Clicca per cambiare." },
            { "synth_cutoff", "CUTOFF\nApertura del filtro a riposo. Bassa: suono scuro; alta: suono brillante." },
            { "synth_reso", "RESO\nRisonanza del filtro, per il carattere «acid» e il fischio." },
            { "synth_env", "ENV\nQuanto la tua pennata apre il filtro. Alto: ogni nota fa «wow»." },
            { "synth_mix", "MIX\nMescola il synth con il tuo basso." },

            // Pitch
            { "pitch_shift", "SHIFT\nSpostamento in semitoni: +12 = un'ottava sopra, +7 = una quinta, -12 = un'ottava sotto." },
            { "pitch_detune", "DETUNE\nStonatura fine in centesimi. Qualche centesimo ispessisce il suono, come un chorus." },
            { "pitch_mix", "MIX\nMescola la nota spostata con la tua nota originale." },

            // Amp
            { "amp_gain", "GAIN\nQuanto viene spinto il preamplificatore. Basso: pulito; verso 5: leggero crunch; oltre: saturato. "
                          "Con NAM Capture, il livello d'ingresso della cattura." },
            { "amp_bass", "BASS\nBasse: rotondità e peso." },
            { "amp_mid", "MIDDLE\nMedi: ciò che fa uscire il basso nel mix. Abbassarli dà un suono scavato, alzarli un suono che taglia." },
            { "amp_mid_freq", "FREQ\nFrequenza su cui agisce Middle: grave (200 Hz) per il corpo, alta (1-3 kHz) per l'attacco. "
                              "Inattivo sul Tweed '59, il cui circuito fissa le frequenze." },
            { "amp_treble", "TREBLE\nAcuti: chiarezza e rumori delle corde." },
            { "amp_master", "MASTER\nVolume generale dell'amplificatore. Se la spia OUT CLIP si accende, abbassalo." },
            { "amp_blend", "DI BLEND\nMescola il suono diretto del basso (prima dell'ampli) con ampli e cassa, come in studio: "
                           "il DI porta basse pulite, l'ampli il carattere." },
            { "amp_contour", "CONTOUR\nScava i medi intorno a 380 Hz e apre basse e acuti: il suono «smile», ideale per lo slap." },
            { "amp_deep", "DEEP\nAggiunge basse e toglie un po' di 500 Hz, per un suono più profondo." },
            { "amp_bright", "BRIGHT\nAggiunge acuti, per più chiarezza e mordente." },
            { "amp_low_cut", "LO CUT\nTaglia le sub-basse all'uscita dell'ampli. Utile per asciugare il suono, o per piccole casse "
                             "che saturano nelle basse." },
            { "amp_high_cut", "HI CUT\nTaglia gli acuti all'uscita dell'ampli, per addolcire un suono troppo brillante o una distorsione che graffia." },

            // Amp models
            { "model.Italian Hi-Fi", "ITALIAN HI-FI\nAmpli a transistor neutro e moderno, ispirato al Markbass Little Mark. Veloce e preciso, ideale per slap e funk." },
            { "model.Super Valve", "SUPER VALVE\nTutto valvolare da 300 W, ispirato all'Ampeg SVT: medio-bassi caldi, acuti rotondi, il suono rock." },
            { "model.Nordic Drive", "NORDIC DRIVE\nIspirato al Darkglass B7K: le basse restano pulite, solo la parte alta satura. Il suono metal moderno." },
            { "model.Tweed '59", "TWEED '59\nIspirato al Fender Bassman del 1959, con i controlli di tono passivi modellati: rotondo, vintage, satura presto." },
            { "model.Flip Top '60s", "FLIP TOP '60S\nPiccolo ampli valvolare da studio, ispirato all'Ampeg B-15: scuro e rotondo, il suono Motown." },
            { "model.West Coast 800", "WEST COAST 800\nAmpli a transistor rock, ispirato al Gallien-Krueger 800RB: medi scavati, mordente a 4 kHz." },
            { "model.Stomp DI", "STOMP DI\nPreamplificatore DI, ispirato al SansAmp Bass Driver: diretto e incisivo, da pulito a crunch." },
            { "model.NAM Capture", "NAM CAPTURE\nSuona una cattura di un vero amplificatore (un file .nam di Neural Amp Modeler), "
                                   "gratuite su TONE3000. Caricala con LOAD NAM." },

            // Cabinets
            { "cab.Off (DI)", "OFF (DI)\nNessuna cassa: il suono diretto dell'ampli, come un'uscita DI." },
            { "cab.8x10", "8x10\nIl grande frigorifero dei concerti rock: basse potenti, medi presenti." },
            { "cab.4x10", "4x10\nVersatile e incisivo, con più attacco della 8x10." },
            { "cab.1x15", "1x15\nUn solo grande altoparlante: rotondo, profondo, vintage, pochi acuti." },
            { "cab.2x12", "2x12\nCompatta e centrata sui medi, si sente bene in un gruppo." },
            { "cab.2x10 Hi-Fi", "2x10 HI-FI\nCon tweeter: suono ampio e chiaro per slap e jazz moderno." },
            { "cab.IR File", "IR FILE\nSuona la tua risposta all'impulso (IR) di cassa, caricata con LOAD IR." },

            // EQ
            { "eq_on", "ON\nAccende o spegne l'equalizzatore. Si accende da solo appena tocchi un fader o un punto." },
            { "cut_low", "CUT 80 HZ\nTaglia tutto sotto 80 Hz in un colpo: il suono si asciuga e lascia spazio alla cassa." },
            { "kill_low", "KILL LOW\nElimina le basse (sotto 250 Hz). Per un drop dal vivo o per isolare l'attacco." },
            { "kill_mid", "KILL MID\nElimina i medi (da 250 Hz a 2,5 kHz): restano solo le basse e lo schiocco." },
            { "kill_high", "KILL HIGH\nElimina gli acuti (sopra 2,5 kHz): suono scuro e rotondo, come un basso con corde flatwound." },
            { "cut_high", "CUT 5 KHZ\nTaglia tutto sopra 5 kHz in un colpo: addolcisce il suono ed elimina il fruscio." },
            { "geq_33", "33 Hz\nSub-basse: il rombo che si sente più che si ascolta. Da alzare con cautela." },
            { "geq_63", "63 Hz\nPeso e fondamenta del suono." },
            { "geq_125", "125 Hz\nRotondità. Troppo: il suono rimbomba." },
            { "geq_250", "250 Hz\nCorpo. Troppo: il suono si impasta." },
            { "geq_500", "500 Hz\nCorpo e lato «cartone». Abbassarli apre il suono." },
            { "geq_1k", "1 kHz\nPresenza: ciò che rende il basso udibile nel mix." },
            { "geq_2k", "2 kHz\nAttacco e grana delle note." },
            { "geq_4k", "4 kHz\nSchiocco delle corde e rumori delle dita." },
            { "geq_8k", "8 kHz\nBrillantezza e fruscio." },
            { "peq_low_cut", "LOW CUT\nPunto grigio a sinistra: taglia tutto ciò che sta sotto. Trascinalo a destra per asciugare le basse. "
                             "Si muove solo a sinistra e a destra: non ha guadagno." },
            { "peq_low_freq", "LOW (Low Shelf)\nAlza o abbassa tutte le basse sotto la sua frequenza. Trascina in alto o in basso per il guadagno, "
                              "a sinistra o a destra per la frequenza." },
            { "peq_mid1_freq", "LO-MID\nBanda medio-bassa: corpo e impasto. Trascina per regolare, rotella per la larghezza (Q), "
                               "doppio clic per azzerare." },
            { "peq_mid2_freq", "HI-MID\nBanda medio-alta: attacco e presenza. Trascina per regolare, rotella per la larghezza (Q), "
                               "doppio clic per azzerare." },
            { "peq_high_freq", "HIGH (High Shelf)\nAlza o abbassa tutti gli acuti sopra la sua frequenza." },
            { "peq_high_cut", "HIGH CUT\nPunto grigio a destra: taglia tutto ciò che sta sopra. Trascinalo a sinistra per addolcire il suono. "
                              "Si muove solo a sinistra e a destra: non ha guadagno." },

            // Post-amp effects
            { "phaser_rate", "RATE\nVelocità del movimento." },
            { "phaser_depth", "DEPTH\nAmpiezza del movimento." },
            { "phaser_feedback", "REGEN\nRetroazione: più è alta, più l'effetto è marcato e canta." },
            { "flanger_rate", "RATE\nVelocità del movimento." },
            { "flanger_depth", "DEPTH\nAmpiezza del movimento." },
            { "flanger_feedback", "REGEN\nRetroazione: più è alta, più il suono è metallico." },
            { "flanger_mix", "MIX\nMescola l'effetto con il suono diretto." },
            { "chorus_rate", "RATE\nVelocità dell'ondulazione. Lenta: ampia e morbida; veloce: effetto vibrato." },
            { "chorus_depth", "DEPTH\nQuantità di stonatura." },
            { "chorus_mix", "MIX\nMescola l'effetto con il suono diretto." },
            { "tremolo_rate", "RATE\nVelocità delle variazioni di volume." },
            { "tremolo_depth", "DEPTH\nForza delle variazioni: al massimo il suono si interrompe a ogni ciclo." },
            { "tremolo_shape", "SHAPE\nForma dell'onda: a sinistra ondulazione morbida, a destra taglio netto." },
            { "delay_time", "TIME\nTempo tra ciò che suoni e l'eco, in millisecondi. Oppure batti il tempo con TAP." },
            { "delay_feedback", "REPEAT\nNumero di ripetizioni: basso, un solo eco; alto, echi che durano a lungo." },
            { "delay_mix", "MIX\nVolume degli echi." },
            { "reverb_size", "SIZE\nDimensione della stanza: dalla piccola camera alla grande chiesa." },
            { "reverb_damping", "DAMP\nSmorzamento degli acuti: alto per un riverbero scuro e discreto." },
            { "reverb_mix", "MIX\nQuantità di riverbero." },
            { "limiter_on", "LIMIT\nLimiter d'uscita: tiene il suono sotto -1 dBFS per proteggere le orecchie ed evitare la saturazione digitale. "
                            "Trasparente finché resti sotto quel livello." },

            // Interface
            { "ui.footswitch", "INTERRUTTORE A PEDALE\nAccende o spegne il pedale: il LED è acceso quando è attivo. Clic "
                               "destro: MIDI learn, per comandarlo con un pad del tuo controller. Un punto dorato su un "
                               "controllo indica che un controllo MIDI lo comanda." },
            { "ui.tap", "TAP\nBatti almeno due volte a tempo: il tempo del delay si adatta al tuo tempo." },
            { "ui.pedalPresets", "PRESET DEL PEDALE\nRegolazioni pronte per questo pedale. Il resto del tuo suono non cambia." },
            { "ui.removePedal", "RIMUOVI\nSpegne il pedale e lo mette via. Riprendilo con + ADD." },
            { "ui.addPedal", "+ ADD\nAggiunge un pedale a questa pedaliera. Poi trascina un pedale per spostarlo: "
                             "il suono segue l'ordine mostrato, da sinistra a destra." },
            { "ui.presetBox", "PRESET\nI suoni di fabbrica e i tuoi. Scegline uno per regolare tutto in un colpo. Una stella "
                              "dorata indica che hai modificato il suono dopo averlo caricato: salvalo con SAVE per tenerlo." },
            { "ui.previousPreset", "PRESET PRECEDENTE" },
            { "ui.nextPreset", "PRESET SUCCESSIVO" },
            { "ui.savePreset", "SAVE\nSalva il suono attuale come preset, in Documenti/Bass Amp/Presets." },
            { "ui.presetMenu", "MENU DEI PRESET\nRinomina o elimina il preset utente caricato, importa un preset che ti hanno "
                               "mandato, esporta questo suono per condividerlo (uno zip con i suoi file IR e di cattura) o "
                               "apri la cartella dei preset." },
            { "ui.record", "REC\nRegistra ciò che esce dall'app (con gli effetti), in WAV, in Documenti/Bass Amp/Recordings. "
                           "Clicca di nuovo per fermare." },
            { "ui.recordings", "FILES\nApre la cartella delle registrazioni." },
            { "ui.tuner", "ACCORDATORE\nSuona una corda a vuoto: la nota appare e l'ago è centrato quando è accordata. E A D "
                          "G, e B grave per un cinque corde. Rotella sullo schermo: La di riferimento da 430 a 450 Hz; doppio "
                          "clic: torna a 440 Hz." },
            { "ui.mute", "MUTE\nZittisce l'uscita mentre accordi." },
            { "ui.meters", "LIVELLI\nIN: ciò che entra (il tuo basso); OUT: ciò che esce; COMP: riduzione del compressore. "
                           "CLIP rosso: saturazione, abbassa il gain della scheda audio (IN) o il master (OUT). Clicca per "
                           "spegnere le spie. Clicca su IN 1 / IN 2 per scegliere l'ingresso della scheda a cui è collegato il "
                           "basso." },
            { "ui.eqPresets", "PRESET EQ\nCurve pronte: voce, chitarra, basso rotondo, slap… Cambia solo l'equalizzatore." },
            { "ui.eqBasic", "BASIC\nEqualizzatore grafico a 9 bande con fader." },
            { "ui.eqAdvanced", "ADVANCED\nEqualizzatore parametrico: 6 punti da trascinare sulla curva, più preciso." },
            { "ui.eqScreen", "SCHERMO EQ\nLa curva mostra la correzione, lo sfondo lo spettro dell'uscita. "
                             "Individua le gobbe di frequenza per correggerle." },
            { "ui.ampPresets", "PRESET AMPLI\nRegolazioni pronte di ampli e cassa. Pedali ed EQ non cambiano." },
            { "ui.power", "POWER\nLa spia di accensione, come su un vero ampli: resta accesa finché l'app è in funzione. Qui "
                          "non c'è niente da regolare." },
            { "ui.language", "LINGUA\nLa lingua dei fumetti d'aiuto e dei menu. I nomi dei controlli restano in inglese, "
                             "come sulla vera attrezzatura." },
            { "ui.update", "NUOVA VERSIONE\nÈ uscita una versione più recente di Bass Amp. Clicca per aprire la pagina di "
                           "download. Bass Amp controlla una volta per avvio chiedendo a GitHub; nulla su di te viene inviato." },
            { "ui.fullScreen", "SCHERMO INTERO\nIngrandisce l'app su tutto lo schermo. F11 fa lo stesso, Esc torna alla finestra." },
            { "ui.loadIr", "LOAD IR\nCarica una risposta all'impulso di cassa (WAV, AIFF, FLAC) e imposta la cassa su IR File." },
            { "ui.loadNam", "LOAD NAM\nCarica una cattura di ampli .nam (TONE3000 ne offre migliaia). "
                            "I controlli di tono e la cassa restano attivi dopo." },
            { "ui.player", "LETTORE\nRiproduce un brano o una base su cui suonare." },
            { "ui.playerLoad", "LOAD\nApre un file audio (WAV, AIFF, FLAC, OGG, MP3) e lo riproduce." },
            { "ui.playerPlay", "PLAY / PAUSA" },
            { "ui.playerStop", "STOP\nFerma e torna all'inizio." },
            { "ui.playerPosition", "POSIZIONE\nTrascina per spostarti nel brano." },
            { "ui.playerLoop", "LOOP\nRiproduce il brano in loop." },
            { "ui.reamp", "RE-AMP\nManda il file nella catena al posto del basso: per far passare una traccia DI registrata "
                          "in pedali e ampli, e provare regolazioni senza suonare." },
            { "ui.playerVolume", "VOL\nVolume del brano." },
            { "ui.looper", "LOOPER\n4 tracce in loop per costruire un brano dal vivo. Clicca un pad per registrare, "
                           "OPEN per il looper completo." },
            { "ui.loopPad", "TRACCIA\n1° clic: registra. 2° clic: il loop parte. Poi ogni clic aggiunge uno strato (DUB) "
                            "o torna alla riproduzione. Clic destro: MIDI learn, per lanciarla da un pad o un interruttore a pedale." },
            { "ui.looperOpen", "OPEN\nApre il looper completo: regolazioni di ogni traccia, annulla, esporta." },
            { "ui.looperClose", "CLOSE\nChiude il looper: i loop continuano a suonare." },
            { "ui.looperStopAll", "STOP ALL\nFerma tutte le tracce, o le rilancia se sono tutte ferme. Ripartono a tempo." },
            { "ui.looperClearAll", "CLEAR ALL\nCancella tutte le tracce." },
            { "ui.looperUndo", "UNDO\nAnnulla l'ultima registrazione o l'ultimo strato aggiunto." },
            { "ui.looperRedo", "REDO\nRipristina ciò che UNDO ha appena tolto." },
            { "ui.looperSync", "SYNC\nQuando il metronomo suona, i loop iniziano e finiscono su una battuta: "
                               "lancia la registrazione e aspetterà la battuta successiva. "
                               "Altrimenti è il primo loop a dare la lunghezza che seguono gli altri." },
            { "ui.looperExport", "EXPORT WAV\nSalva il mix delle tracce che suonano in un file WAV, "
                                 "in Documenti/Bass Amp/Recordings." },
            { "ui.looperTrackStop", "STOP\nFerma questa traccia senza cancellarla. Un clic sul suo pad la rilancia, a tempo." },
            { "ui.looperTrackClear", "CLEAR\nCancella questa traccia." },
            { "ui.looperReverse", "REV\nRiproduce la traccia al contrario." },
            { "ui.looperHalf", "HALF\nRiproduce la traccia a metà velocità: un'ottava sotto." },
            { "ui.looperLevel", "LEVEL\nVolume della traccia." },
            { "ui.metronome", "METRONOMO\nUn clic per esercitarti a tempo. Non viene mai registrato da REC." },
            { "ui.metronomeOn", "ON\nAvvia o ferma il metronomo." },
            { "ui.metronomeTempo", "TEMPO\nVelocità in battiti al minuto (BPM). In una DAW, il metronomo e il looper seguono "
                                   "il tempo del brano." },
            { "ui.metronomeTap", "TAP\nBatti il tempo per regolare il metronomo." },
            { "ui.metronomeVolume", "VOL\nVolume del clic." },
        },
        {
            { "no file loaded", "nessun file caricato" },
            { "No capture loaded", "Nessuna cattura caricata" },
            { "Can't read this file", "File illeggibile" },
            { "Set audio to", "Imposta l'audio su" },
            { "Load a Neural Amp Modeler capture", "Carica una cattura Neural Amp Modeler" },
            { "Load a cabinet impulse response", "Carica una risposta all'impulso di cassa" },
            { "Saved", "Salvato:" },
            { "Export failed", "Esportazione non riuscita" },
            { "Synced to the metronome: loops start and end on a bar.", "A tempo col metronomo: i loop iniziano e finiscono su una battuta." },
            { "Free: the first loop sets the length the others follow.", "Libero: il primo loop dà la lunghezza che seguono gli altri." },
            { "MIDI learn", "MIDI learn" },
            { "Cancel MIDI learn", "Annulla MIDI learn" },
            { "Forget", "Dimentica" },
            { "MOVE A MIDI CONTROL", "MUOVI UN CONTROLLO MIDI" },
            { "Choose a backing track", "Scegli un brano" },
            { "No track loaded", "Nessun brano caricato" },
            { "Factory", "Fabbrica" },
            { "User", "Miei" },
            { "Save preset", "Salva preset" },
            { "Preset name:", "Nome del preset:" },
            { "copy", "copia" },
            { "Save", "Salva" },
            { "Cancel", "Annulla" },
            { "No audio device: Options > Audio/MIDI Settings", "Nessun dispositivo audio: Opzioni > Impostazioni audio/MIDI" },
            { "latency", "latenza" },
            { "Language", "Lingua" },
            { "available", "disponibile" },
            { "Rename...", "Rinomina..." },
            { "Delete...", "Elimina..." },
            { "Import...", "Importa..." },
            { "Export...", "Esporta..." },
            { "Open presets folder", "Apri la cartella dei preset" },
            { "Rename preset", "Rinomina il preset" },
            { "Rename", "Rinomina" },
            { "Delete preset?", "Eliminare il preset?" },
            { "Delete", "Elimina" },
            { "Import a preset", "Importa un preset" },
            { "Export this sound", "Esporta questo suono" },
            { "This file isn't a Bass Amp preset.", "Questo file non è un preset di Bass Amp." },
            { "File not found", "File non trovato" },
            { "Replace preset?", "Sostituire il preset?" },
            { "A preset with this name already exists.", "Esiste già un preset con questo nome." },
            { "Replace", "Sostituisci" },
            { "Audio input is muted to avoid feedback loop", "L'ingresso audio è disattivato per evitare il feedback" },
            { "Settings...", "Impostazioni..." },
            { "Input", "Ingresso" },
            { "Options", "Opzioni" },
            { "Audio/MIDI Settings", "Impostazioni audio/MIDI" },
            { "Audio/MIDI Settings...", "Impostazioni audio/MIDI..." },
            { "Save current state", "Salva lo stato attuale" },
            { "Save current state...", "Salva lo stato attuale..." },
            { "Load a saved state", "Carica uno stato salvato" },
            { "Load a saved state...", "Carica uno stato salvato..." },
            { "Reset to default state", "Torna allo stato predefinito" },
            { "Audio device type:", "Tipo di driver audio:" },
            { "Device:", "Dispositivo:" },
            { "Output:", "Uscita:" },
            { "Input:", "Ingresso:" },
            { "Active output channels:", "Canali di uscita attivi:" },
            { "Active input channels:", "Canali di ingresso attivi:" },
            { "Sample rate:", "Frequenza di campionamento:" },
            { "Audio buffer size:", "Dimensione del buffer audio:" },
            { "Active MIDI inputs:", "Ingressi MIDI attivi:" },
            { "MIDI Output:", "Uscita MIDI:" },
            { "Test", "Test" },
            { "Plays a test tone", "Riproduce un tono di prova" },
            { "Control Panel", "Pannello di controllo" },
            { "Opens the device's own control panel", "Apre il pannello di impostazioni del dispositivo" },
            { "Reset Device", "Reimposta" },
            { "Show advanced settings...", "Mostra impostazioni avanzate..." },
            { "No MIDI inputs available", "Nessun ingresso MIDI disponibile" },
            { "(no audio input channels found)", "(nessun canale di ingresso audio trovato)" },
            { "(no audio output channels found)", "(nessun canale di uscita audio trovato)" },
            { "none", "nessuno" },
            { "Error when trying to open audio device!", "Errore nell'apertura del dispositivo audio!" },
            { "Error whilst loading", "Errore durante il caricamento" },
            { "Error whilst saving", "Errore durante il salvataggio" },
            { "Couldn't read from the specified file!", "Impossibile leggere il file!" },
            { "Couldn't write to the specified file!", "Impossibile scrivere il file!" },
            { "OK", "OK" },
            { "Unsaved changes", "Modifiche non salvate" },
            { "Your changes to this sound will be lost.", "Le modifiche a questo suono andranno perse." },
            { "Load anyway", "Carica comunque" },
        },
    };

    return translations;
}
