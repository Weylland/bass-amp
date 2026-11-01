#include "Translations.h"

const Translations& englishTranslations()
{
    static const Translations translations {
        {
            // Pedals
            { "pedal.gate", "GATE (noise gate)\nMutes the sound when you're not playing, to remove hiss and pickup noise. "
                            "Put it first in the chain, especially before a drive or fuzz that boosts the noise." },
            { "pedal.comp", "COMP (compressor)\nEvens out the volume: loud notes are turned down, quiet ones come forward. "
                            "The sound gets steadier and more \"upfront\". A must for slap and for a studio sound." },
            { "pedal.octaver", "OCTAVER\nAdds the octave below (Sub) and the octave above (Up) to your note. "
                               "Play single notes: chords confuse it. Great for a huge sound or a synth-like line." },
            { "pedal.synth", "SYNTH\nTurns your bass into a synth: an oscillator follows the note you play through a filter "
                             "that opens on the attack. Play one clean note at a time so it can follow." },
            { "pedal.pitch", "PITCH (pitch shifter)\nShifts your note from -12 to +12 semitones, mixed with the dry sound. "
                             "+12 for an octave up, +7 for a fifth, a little Detune to thicken." },
            { "pedal.filter", "FILTER\nResonant filter set by hand or swept by an LFO (a slow oscillation). "
                              "Low-pass for a dark, round sound, high-pass for a thin one, band-pass for a \"radio\" effect." },
            { "pedal.envelope", "ENVELOPE (auto-wah)\nA filter that opens with how hard you pick: the harder you hit, the brighter "
                                "the sound. The funk sound (Bootsy Collins, Mark Adams)." },
            { "pedal.wah", "WAH\nA filter swept with your foot that goes \"wah\". Here the Pedal knob sets it, or a MIDI "
                           "expression pedal (CC 11 by default). Clean keeps some dry sound so the lows stay." },
            { "pedal.fuzz", "FUZZ\nVery strong, square distortion, like a vintage silicon pedal. Blend brings back clean sound "
                            "underneath to keep the lows solid." },
            { "pedal.drive", "DRIVE (overdrive)\nSofter, more natural distortion than fuzz, like an amp heating up. "
                             "Blend mixes in clean sound to keep definition." },
            { "pedal.phaser", "PHASER\nCarves moving notches in the spectrum: a swirling, liquid, very seventies sound." },
            { "pedal.flanger", "FLANGER\nLike the phaser but more metallic: the sound goes through a very short, changing delay. "
                               "With lots of Regen you get the \"jet plane\" effect." },
            { "pedal.chorus", "CHORUS\nDoubles your sound with a slightly detuned, moving copy to widen it. "
                              "The lows stay centred and firm; only the top of the spectrum is processed." },
            { "pedal.tremolo", "TREMOLO\nMakes the volume pulse in rhythm, like old amps. Shape goes from a smooth wave "
                               "to a hard chop." },
            { "pedal.delay", "DELAY (echo)\nRepeats what you play after a set time. TAP: tap the tempo to lock the echo to the song." },
            { "pedal.reverb", "REVERB\nSimulates a room: the sound lingers and gets space. "
                              "Use a little on bass so the lows don't get washed out." },

            // Gate
            { "gate_threshold", "THRESH (threshold)\nLevel below which the gate mutes. Raise it until the hiss disappears "
                                "when you're not playing, without cutting the end of your notes." },
            { "gate_release", "RELEASE\nHow fast the gate closes. Short: cuts clean; long: lets notes fade naturally." },

            // Compressor
            { "comp_threshold", "THRESH (threshold)\nLevel above which the compressor works. The lower, the more it compresses. "
                                "Watch the COMP meter in LEVELS: 3 to 6 dB of reduction is a good start." },
            { "comp_ratio", "RATIO\nHow hard it compresses above the threshold. 2:1 to 4:1: natural; 8:1 and more: very even, "
                            "almost a limiter." },
            { "comp_attack", "ATTACK\nHow fast it reacts. Slow (10 ms and more): lets the pluck through, punchier; "
                             "fast: squashes the attack." },
            { "comp_release", "RELEASE\nHow long it takes to let go after a note. Too short: the sound \"pumps\"; too long: "
                              "the next notes stay compressed." },
            { "comp_makeup", "MAKEUP\nBrings the volume back up after compression, to match the level with the pedal off." },
            { "comp_mix", "MIX\nBlends the compressed sound with the dry one (parallel compression): you keep the natural attack "
                          "and gain density." },

            // Octaver
            { "oct_sub", "SUB\nLevel of the octave below. For a very deep, synth-like sound." },
            { "oct_up", "UP\nLevel of the octave above, which adds a bright, hollow sound." },
            { "oct_dry", "DRY\nLevel of your original sound in the mix." },

            // Envelope
            { "env_type", "TYPE\nFilter shape: LP (low-pass, the classic wah), BP (band-pass, more nasal) or HP (high-pass, thinner). "
                          "Click to change." },
            { "env_dir", "SWEEP\nUp: the filter opens when you hit. Down: it closes, for a reversed \"wow\"." },
            { "env_sens", "SENS (sensitivity)\nHow much your attack opens the filter. Set it to how hard you play." },
            { "env_reso", "RESO (resonance)\nEmphasises the filter frequency. The higher, the stronger the \"quack\"." },
            { "env_mix", "MIX\nBlends the effect with your dry sound, to keep the lows." },

            // Fuzz and drive
            { "fuzz_amount", "FUZZ\nAmount of distortion." },
            { "fuzz_tone", "TONE\nDark to the left, aggressive to the right." },
            { "fuzz_blend", "BLEND\nBlends the fuzz with the clean sound. Toward the left, the lows stay clean." },
            { "dist_drive", "DRIVE\nAmount of distortion, from light grit to real distortion." },
            { "dist_tone", "TONE\nCuts the highs of the distortion. Lower: rounder; higher: more bite." },
            { "dist_blend", "BLEND\nBlends the distortion with the clean sound, to keep the notes defined." },

            // Filter
            { "filter_type", "TYPE\nLP: low-pass (keeps the lows); BP: band-pass (keeps a band); HP: high-pass (keeps the highs). "
                             "12 or 24: gentle or steep slope. Click to change." },
            { "filter_cutoff", "CUTOFF (cutoff frequency)\nWhere the filter works. On a low-pass, lowering it makes the sound darker." },
            { "filter_reso", "RESO (resonance)\nEmphasises the cutoff: the sound gets more \"synth\" and whistling." },
            { "filter_rate", "RATE\nSpeed of the LFO that moves the cutoff. No effect while Depth is at zero." },
            { "filter_depth", "DEPTH\nHow wide the LFO sweeps. At zero, the filter stays put." },

            // Wah
            { "wah_pedal", "PEDAL\nPedal position: heel (low) to toe (high). Can be driven by a MIDI expression pedal." },
            { "wah_reso", "RESO\nStrength of the wah peak. High: very vocal; low: more subtle." },
            { "wah_blend", "CLEAN\nAmount of dry sound kept under the wah, so the lows stay." },

            // Synth
            { "synth_wave", "WAVE\nOscillator shape: Square (hollow and round) or Saw (rich and bright). Click to change." },
            { "synth_cutoff", "CUTOFF\nFilter opening at rest. Low: dark sound; high: bright sound." },
            { "synth_reso", "RESO\nFilter resonance, for the \"acid\", whistling side." },
            { "synth_env", "ENV\nHow much your attack opens the filter. High: every note goes \"wow\"." },
            { "synth_mix", "MIX\nBlends the synth with your bass." },

            // Pitch
            { "pitch_shift", "SHIFT\nShift in semitones: +12 = octave up, +7 = fifth, -12 = octave down." },
            { "pitch_detune", "DETUNE\nFine detune, in cents. A few cents thicken the sound like a chorus." },
            { "pitch_mix", "MIX\nBlends the shifted note with your original note." },

            // Amp
            { "amp_gain", "GAIN\nHow hard the preamp is driven. Low: clean; around 5: light grit; beyond: overdriven. "
                          "On NAM Capture, sets the capture's input level." },
            { "amp_bass", "BASS\nLows: roundness and weight." },
            { "amp_mid", "MIDDLE\nMids: what makes the bass stand out in the mix. Cut them for a scooped sound, boost them to cut through." },
            { "amp_mid_freq", "FREQ\nFrequency Middle works on: low (200 Hz) for thickness, high (1 to 3 kHz) for attack. "
                              "Inactive on Tweed '59, whose circuit fixes the frequencies." },
            { "amp_treble", "TREBLE\nHighs: clarity and string noise." },
            { "amp_master", "MASTER\nOverall amp volume. If the OUT CLIP light comes on, turn it down." },
            { "amp_blend", "DI BLEND\nBlends the dry bass (before the amp) with the amp and cabinet, as in a studio: "
                           "the DI brings clean lows, the amp the character." },
            { "amp_contour", "CONTOUR\nScoops the mids around 380 Hz and opens lows and highs: the \"smiley\" sound, ideal for slap." },
            { "amp_deep", "DEEP\nAdds low end and takes out a little 500 Hz, for a deeper sound." },
            { "amp_bright", "BRIGHT\nAdds highs, for more clarity and snap." },
            { "amp_low_cut", "LO CUT\nCuts the sub-lows at the amp output. Useful to tighten the sound or for small speakers "
                             "that distort in the lows." },
            { "amp_high_cut", "HI CUT\nCuts the highs at the amp output, to soften a sound that's too bright or a fizzy distortion." },

            // Amp models
            { "model.Italian Hi-Fi", "ITALIAN HI-FI\nNeutral, modern solid state, inspired by the Markbass Little Mark. Fast and precise, ideal for slap and funk." },
            { "model.Super Valve", "SUPER VALVE\nAll-tube 300 W, inspired by the Ampeg SVT: warm low mids, rounded highs, the rock sound." },
            { "model.Nordic Drive", "NORDIC DRIVE\nInspired by the Darkglass B7K: the lows stay clean, only the top of the spectrum distorts. The modern metal sound." },
            { "model.Tweed '59", "TWEED '59\nInspired by the 1959 Fender Bassman, with its passive tone stack modelled: round, vintage, breaks up early." },
            { "model.Flip Top '60s", "FLIP TOP '60S\nSmall studio tube amp, inspired by the Ampeg B-15: dark and round, the Motown sound." },
            { "model.West Coast 800", "WEST COAST 800\nRock solid state inspired by the Gallien-Krueger 800RB: scooped mids, bite at 4 kHz." },
            { "model.Stomp DI", "STOMP DI\nDI preamp inspired by the SansAmp Bass Driver: direct and punchy, from clean to gritty." },
            { "model.NAM Capture", "NAM CAPTURE\nPlays a capture of a real amp (a Neural Amp Modeler .nam file), "
                                   "free to download from TONE3000. Load it with LOAD NAM." },

            // Cabinets
            { "cab.Off (DI)", "OFF (DI)\nNo cabinet: the amp's direct sound, like a DI out." },
            { "cab.8x10", "8x10\nThe big fridge of rock gigs: powerful lows, present mids." },
            { "cab.4x10", "4x10\nVersatile and punchy, with more attack than the 8x10." },
            { "cab.1x15", "1x15\nA single big speaker: round, deep, vintage, few highs." },
            { "cab.2x12", "2x12\nCompact and mid-focused, it cuts through a band well." },
            { "cab.2x10 Hi-Fi", "2x10 HI-FI\nWith a tweeter: wide, clear sound for slap and modern jazz." },
            { "cab.IR File", "IR FILE\nPlays your own cabinet impulse response (IR), loaded with LOAD IR." },

            // EQ
            { "eq_on", "ON\nTurns the equalizer on or off. It turns on by itself as soon as you touch a fader or a point." },
            { "cut_low", "CUT 80 HZ\nCuts everything below 80 Hz at once: the sound tightens and leaves room for the kick drum." },
            { "kill_low", "KILL LOW\nRemoves the lows (below 250 Hz). For a live drop effect or to isolate the attack." },
            { "kill_mid", "KILL MID\nRemoves the mids (250 Hz to 2.5 kHz): only the low end and the snap remain." },
            { "kill_high", "KILL HIGH\nRemoves the highs (above 2.5 kHz): dark, round sound, like a flatwound bass." },
            { "cut_high", "CUT 5 KHZ\nCuts everything above 5 kHz at once: softens the sound and removes hiss." },
            { "geq_33", "33 Hz\nSub-lows: the rumble you feel more than hear. Boost with care." },
            { "geq_63", "63 Hz\nThe weight and foundation of the sound." },
            { "geq_125", "125 Hz\nRoundness. Too much: the sound gets boomy." },
            { "geq_250", "250 Hz\nThickness. Too much: the sound gets muddy." },
            { "geq_500", "500 Hz\nBody and the \"boxy\" side. Cutting it opens the sound up." },
            { "geq_1k", "1 kHz\nPresence: what makes the bass heard in the mix." },
            { "geq_2k", "2 kHz\nThe attack and grain of the notes." },
            { "geq_4k", "4 kHz\nString snap and finger noise." },
            { "geq_8k", "8 kHz\nSparkle and hiss." },
            { "peq_low_cut", "LOW CUT\nGrey point on the left: cuts everything below it. Drag it right to tighten the lows. "
                             "It only moves left and right: it has no gain." },
            { "peq_low_freq", "LOW (low shelf)\nRaises or lowers all the lows below its frequency. Drag up or down for the gain, "
                              "left or right for the frequency." },
            { "peq_mid1_freq", "LO-MID\nLow-mid band: thickness and mud. Drag to set, mouse wheel for the width (Q), "
                               "double-click to reset." },
            { "peq_mid2_freq", "HI-MID\nHigh-mid band: attack and presence. Drag to set, mouse wheel for the width (Q), "
                               "double-click to reset." },
            { "peq_high_freq", "HIGH (high shelf)\nRaises or lowers all the highs above its frequency." },
            { "peq_high_cut", "HIGH CUT\nGrey point on the right: cuts everything above it. Drag it left to soften the sound. "
                              "It only moves left and right: it has no gain." },

            // Post-amp effects
            { "phaser_rate", "RATE\nSpeed of the movement." },
            { "phaser_depth", "DEPTH\nWidth of the movement." },
            { "phaser_feedback", "REGEN\nFeedback: the higher, the stronger and more singing the effect." },
            { "flanger_rate", "RATE\nSpeed of the sweep." },
            { "flanger_depth", "DEPTH\nWidth of the sweep." },
            { "flanger_feedback", "REGEN\nFeedback: the higher, the more metallic the sound." },
            { "flanger_mix", "MIX\nBlends the effect with the dry sound." },
            { "chorus_rate", "RATE\nSpeed of the wobble. Slow: wide and smooth; fast: vibrato effect." },
            { "chorus_depth", "DEPTH\nAmount of detune." },
            { "chorus_mix", "MIX\nBlends the effect with the dry sound." },
            { "tremolo_rate", "RATE\nSpeed of the volume changes." },
            { "tremolo_depth", "DEPTH\nSize of the changes: all the way up, the sound is cut on every cycle." },
            { "tremolo_shape", "SHAPE\nWave shape: smooth wave on the left, hard chop on the right." },
            { "delay_time", "TIME\nTime between your playing and the echo, in milliseconds. Or tap the tempo with TAP." },
            { "delay_feedback", "REPEAT\nNumber of repeats: low, a single echo; high, echoes that linger." },
            { "delay_mix", "MIX\nLevel of the echoes." },
            { "reverb_size", "SIZE\nRoom size: small room to big church." },
            { "reverb_damping", "DAMP\nHigh-frequency absorption: high for a dark, discreet reverb." },
            { "reverb_mix", "MIX\nAmount of reverb." },
            { "limiter_on", "LIMIT\nOutput limiter: keeps the sound under -1 dBFS, to protect your ears and avoid digital clipping. "
                            "Transparent as long as you stay under that level." },

            // Interface
            { "ui.footswitch", "FOOTSWITCH\nTurns the pedal on or off: the LED lights when it is on. Right-click: MIDI learn, "
                               "to switch it from a pad of your controller. A gold dot on a control means a MIDI control "
                               "drives it." },
            { "ui.tap", "TAP\nTap at least twice in rhythm: the delay time locks to your tempo." },
            { "ui.pedalPresets", "PEDAL PRESETS\nReady-made settings for this pedal. The rest of your sound doesn't change. "
                                 "Save... keeps your own settings in the list too." },
            { "ui.removePedal", "REMOVE\nTurns the pedal off and puts it away. Bring it back with + ADD." },
            { "ui.addPedal", "+ ADD\nAdds a pedal to this pedalboard. Then drag a pedal to move it: "
                             "the sound follows the order shown, left to right." },
            { "ui.presetBox", "PRESETS\nThe factory sounds and yours. Pick one to set everything at once. A gold star means "
                              "you changed the sound since loading it: save it with SAVE to keep it." },
            { "ui.previousPreset", "PREVIOUS PRESET" },
            { "ui.nextPreset", "NEXT PRESET" },
            { "ui.savePreset", "SAVE\nSaves the current sound as a preset, in Documents/Bass Amp/Presets." },
            { "ui.presetMenu", "PRESET MENU\nRename or delete the user preset loaded now, import a preset someone sent you, "
                               "export this sound to share it (a zip with its IR and capture files), or open the presets "
                               "folder." },
            { "ui.record", "REC\nRecords what comes out of the app (effects included) to WAV, in Documents/Bass Amp/Recordings. "
                           "Click again to stop." },
            { "ui.recordings", "FILES\nOpens the recordings folder." },
            { "ui.recordSplit", "SPLIT\nOff: REC records one file, the mix you hear. On: two files side by side, \"Bass\" "
                                "(your rig with its loops) and \"Player\" (the song), to balance or rework them later in a "
                                "DAW. Set it before REC." },
            { "ui.tuner", "TUNER\nPlay an open string: the note shows and the needle is centred when it is in tune. E A D G, "
                          "and low B on a five-string. Mouse wheel on the screen: reference A from 430 to 450 Hz; "
                          "double-click: back to 440 Hz." },
            { "ui.mute", "MUTE\nMutes the output while you tune." },
            { "ui.meters", "LEVELS\nIN: what comes in (your bass); OUT: what goes out; COMP: compressor reduction. Red CLIP: "
                           "overload, lower your interface gain (IN) or the Master (OUT). Click to clear the lights. Click IN "
                           "1 / IN 2 to choose the interface input your bass is plugged into." },
            { "ui.eqPresets", "EQ PRESETS\nReady-made curves: vocals, guitar, round bass, slap... Only the equalizer changes. "
                              "Save... keeps your own settings in the list too." },
            { "ui.eqBasic", "BASIC\n9-band graphic equalizer, with faders." },
            { "ui.eqAdvanced", "ADVANCED\nParametric equalizer: 6 points to drag on the curve, more precise." },
            { "ui.eqScreen", "EQ SCREEN\nThe curve shows the correction, the background the spectrum of the output. "
                             "Spot frequency bumps to fix them." },
            { "ui.ampPresets", "AMP PRESETS\nReady-made amp and cabinet settings. Pedals and EQ don't change. Save... keeps "
                               "your own settings in the list too. They keep the IR and capture files too." },
            { "ui.power", "POWER\nThe pilot lamp, as on a real amp: it stays lit while the app runs. There is nothing to set "
                          "here." },
            { "ui.language", "LANGUAGE\nThe language of the help bubbles and menus. Control names stay in English, "
                             "as on real gear." },
            { "ui.audioSettings", "AUDIO\nThe audio and MIDI settings: driver (ASIO, WASAPI...), interface, input and output "
                                  "channels, sample rate, buffer size (smaller means less latency), and MIDI inputs. Clicking "
                                  "the driver line next to it does the same." },
            { "ui.update", "NEW VERSION\nA newer version of Bass Amp is out. Click to open its download page. Bass Amp checks "
                           "once per launch by asking GitHub; nothing about you is sent." },
            { "ui.fullScreen", "FULL SCREEN\nFills the whole screen with the app. F11 does the same, Escape goes back to a window." },
            { "ui.loadIr", "LOAD IR\nLoads a cabinet impulse response (WAV, AIFF, FLAC) and sets the cabinet to IR File." },
            { "ui.loadNam", "LOAD NAM\nLoads a .nam amp capture (TONE3000 has thousands). "
                            "The tone knobs and the cabinet stay active after it." },
            { "ui.player", "PLAYER\nPlays a song or a track to play along with." },
            { "ui.playerLoad", "LOAD\nOpens an audio file (WAV, AIFF, FLAC, OGG, MP3) and plays it." },
            { "ui.playerPlay", "PLAY / PAUSE" },
            { "ui.playerStop", "STOP\nStops and goes back to the start." },
            { "ui.playerPosition", "POSITION\nDrag to move through the song." },
            { "ui.playerLoop", "LOOP\nPlays the song on repeat." },
            { "ui.reamp", "RE-AMP\nSends the file into the chain instead of your bass: to run a recorded DI track "
                          "through the pedals and amp, and try settings without playing." },
            { "ui.playerVolume", "VOL\nSong volume." },
            { "ui.looper", "LOOPER\n4 loop tracks to build a song live. Click a pad to record, OPEN for the full looper." },
            { "ui.loopPad", "TRACK\n1st click: records (with AUTO, it waits for your first note; with COUNT-IN, for a full "
                            "bar). 2nd click: the loop plays. Then each click adds a layer (DUB) or goes back to playing. "
                            "Right-click: MIDI learn, to trigger it from a pad or a footswitch." },
            { "ui.looperOpen", "OPEN\nOpens the full looper: settings for each track, undo, export." },
            { "ui.looperClose", "CLOSE\nCloses the looper: the loops keep playing." },
            { "ui.looperStopAll", "STOP ALL\nStops every track, or restarts them if all are stopped. They come back in time." },
            { "ui.looperClearAll", "CLEAR ALL\nErases every track." },
            { "ui.looperUndo", "UNDO\nUndoes the last recording or the last layer added." },
            { "ui.looperRedo", "REDO\nBrings back what UNDO just removed." },
            { "ui.looperSync", "SYNC\nWhile the metronome runs, loops start and stop on a bar: "
                               "start recording and it waits for the next bar. "
                               "Otherwise, the first loop sets the length the others follow." },
            { "ui.looperCountIn", "COUNT-IN\nWith SYNC and the metronome running, the first loop waits for a full bar of "
                                  "clicks before recording, so you know when to come in." },
            { "ui.looperAuto", "AUTO\nWithout the metronome, the first loop waits for you: after the click on its pad, "
                               "recording starts on the first note you play, so the loop has no silence at the start. Off: "
                               "recording starts on the click." },
            { "ui.looperExport", "EXPORT WAV\nSaves the mix of the playing tracks to a WAV file, "
                                 "in Documents/Bass Amp/Recordings." },
            { "ui.looperTrackStop", "STOP\nStops this track without erasing it. A click on its pad restarts it, in time." },
            { "ui.looperTrackClear", "CLEAR\nErases this track." },
            { "ui.looperTrim", "TRIM\nCuts the start or the end of the loop, for a click on the pad that came a bit early or "
                               "late. Drag the handles on the waveform while it plays. Only while this track is the only one." },
            { "ui.looperTrimReset", "RESET\nPlays the whole take again, as recorded." },
            { "ui.looperTrimDone", "DONE\nCloses the trim view. The loop keeps its new start and end." },
            { "ui.looperReverse", "REV\nPlays the track backwards." },
            { "ui.looperHalf", "HALF\nPlays the track at half speed: an octave lower." },
            { "ui.looperLevel", "LEVEL\nTrack volume." },
            { "ui.metronome", "METRONOME\nA click to practise in time. It's never recorded by REC." },
            { "ui.metronomeOn", "ON\nStarts or stops the metronome." },
            { "ui.metronomeTempo", "TEMPO\nSpeed in beats per minute (BPM). In a DAW, the metronome and the looper follow the "
                                   "song's tempo." },
            { "ui.metronomeTap", "TAP\nTap the tempo to set the metronome." },
            { "ui.metronomeVolume", "VOL\nClick volume." },
        },
        {},
    };

    return translations;
}
