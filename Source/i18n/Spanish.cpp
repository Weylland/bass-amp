#include "Translations.h"

const Translations& spanishTranslations()
{
    static const Translations translations {
        {
            // Pedals
            { "pedal.gate", "GATE (puerta de ruido)\nSilencia el sonido cuando no tocas, para quitar el soplido y el ruido de las pastillas. "
                            "Ponlo el primero de la cadena, sobre todo antes de un drive o un fuzz que amplifican el ruido." },
            { "pedal.comp", "COMP (compresor)\nIguala el volumen: las notas fuertes bajan, las suaves suben. "
                            "El sonido queda más parejo y más «delante». Imprescindible para el slap y para un sonido de estudio." },
            { "pedal.octaver", "OCTAVER\nAñade la octava inferior (Sub) y la superior (Up) a tu nota. "
                               "Toca notas sueltas: los acordes lo confunden. Ideal para un sonido enorme o una línea de sintetizador." },
            { "pedal.synth", "SYNTH\nConvierte tu bajo en un sintetizador: un oscilador sigue la nota que tocas a través de un filtro "
                             "que se abre con el ataque. Toca una nota limpia cada vez para que la siga." },
            { "pedal.pitch", "PITCH (transpositor)\nDesplaza tu nota de -12 a +12 semitonos, mezclada con el sonido directo. "
                             "+12 para una octava arriba, +7 para una quinta, un poco de Detune para engordar." },
            { "pedal.filter", "FILTER\nFiltro resonante ajustado a mano o barrido por un LFO (una oscilación lenta). "
                              "Paso bajo para un sonido oscuro y redondo, paso alto para uno fino, paso banda para un efecto «radio»." },
            { "pedal.envelope", "ENVELOPE (auto-wah)\nUn filtro que se abre según la fuerza de tu ataque: cuanto más fuerte tocas, "
                                "más brillante el sonido. El sonido funk por excelencia (Bootsy Collins, Mark Adams)." },
            { "pedal.wah", "WAH\nUn filtro que se barre con el pie y hace «uah». Aquí lo ajusta la perilla Pedal, o un pedal de "
                           "expresión MIDI (CC 11 por defecto). Clean conserva sonido directo para no perder graves." },
            { "pedal.fuzz", "FUZZ\nSaturación muy fuerte y cuadrada, como un pedal vintage de silicio. Blend devuelve sonido limpio "
                            "por debajo para mantener los graves firmes." },
            { "pedal.drive", "DRIVE (overdrive)\nSaturación más suave y natural que el fuzz, como un ampli que se calienta. "
                             "Blend mezcla sonido limpio para conservar la definición." },
            { "pedal.phaser", "PHASER\nAbre muescas que se desplazan por el espectro: un sonido giratorio y líquido, muy de los setenta." },
            { "pedal.flanger", "FLANGER\nComo el phaser pero más metálico: el sonido pasa por un retardo muy corto que varía. "
                               "Con mucho Regen se obtiene el efecto «avión a reacción»." },
            { "pedal.chorus", "CHORUS\nDuplica tu sonido con una copia ligeramente desafinada que se mueve, para ensancharlo. "
                              "Los graves quedan centrados y firmes; solo se procesa la parte alta del espectro." },
            { "pedal.tremolo", "TREMOLO\nHace variar el volumen al ritmo, como los amplis antiguos. Shape va de una ondulación suave "
                               "a un corte seco." },
            { "pedal.delay", "DELAY (eco)\nRepite lo que tocas tras un tiempo ajustable. TAP: marca el tempo para ajustar el eco a la canción." },
            { "pedal.reverb", "REVERB\nSimula la acústica de una sala: el sonido se prolonga y gana espacio. "
                              "En poca cantidad en el bajo, para no ahogar los graves." },

            // Gate
            { "gate_threshold", "THRESH (umbral)\nNivel por debajo del cual el gate silencia. Súbelo hasta que desaparezca el soplido "
                                "cuando no tocas, sin cortar el final de tus notas." },
            { "gate_release", "RELEASE\nTiempo que tarda el gate en cerrarse. Corto: corta seco; largo: deja que las notas se apaguen solas." },

            // Compressor
            { "comp_threshold", "THRESH (umbral)\nNivel a partir del cual actúa el compresor. Cuanto más bajo, más comprime. "
                                "Mira el medidor COMP en LEVELS: de 3 a 6 dB de reducción es un buen comienzo." },
            { "comp_ratio", "RATIO\nFuerza de la compresión por encima del umbral. 2:1 a 4:1: natural; 8:1 o más: muy parejo, "
                            "casi un limitador." },
            { "comp_attack", "ATTACK\nVelocidad de reacción. Lenta (10 ms o más): deja pasar el golpe del ataque, más pegada; "
                             "rápida: aplasta el ataque." },
            { "comp_release", "RELEASE\nTiempo para soltar la compresión tras una nota. Demasiado corto: el sonido «bombea»; demasiado largo: "
                              "las notas siguientes siguen comprimidas." },
            { "comp_makeup", "MAKEUP\nRecupera el volumen tras la compresión, para igualar el nivel con el pedal apagado." },
            { "comp_mix", "MIX\nMezcla el sonido comprimido con el directo (compresión en paralelo): se conserva el ataque natural "
                          "y se gana densidad." },

            // Octaver
            { "oct_sub", "SUB\nVolumen de la octava inferior. Para un sonido muy grave, tipo sintetizador." },
            { "oct_up", "UP\nVolumen de la octava superior, que añade un sonido brillante y hueco." },
            { "oct_dry", "DRY\nVolumen de tu sonido original en la mezcla." },

            // Envelope
            { "env_type", "TYPE\nForma del filtro: LP (paso bajo, el wah clásico), BP (paso banda, más nasal) o HP (paso alto, más fino). "
                          "Haz clic para cambiar." },
            { "env_dir", "SWEEP\nUp: el filtro se abre cuando atacas. Down: se cierra, para un «uou» invertido." },
            { "env_sens", "SENS (sensibilidad)\nCuánto abre tu ataque el filtro. Ajústalo según la fuerza con que tocas." },
            { "env_reso", "RESO (resonancia)\nRealza la frecuencia del filtro. Cuanto más alta, más marcado el efecto «cuac»." },
            { "env_mix", "MIX\nMezcla el efecto con tu sonido directo, para conservar los graves." },

            // Fuzz and drive
            { "fuzz_amount", "FUZZ\nCantidad de saturación." },
            { "fuzz_tone", "TONE\nOscuro a la izquierda, agresivo a la derecha." },
            { "fuzz_blend", "BLEND\nMezcla el fuzz con el sonido limpio. Hacia la izquierda, los graves quedan limpios." },
            { "dist_drive", "DRIVE\nCantidad de saturación, del grano ligero a la distorsión de verdad." },
            { "dist_tone", "TONE\nRecorta los agudos de la saturación. Más bajo: más redondo; más alto: más mordiente." },
            { "dist_blend", "BLEND\nMezcla la saturación con el sonido limpio, para conservar la definición de las notas." },

            // Filter
            { "filter_type", "TYPE\nLP: paso bajo (conserva los graves); BP: paso banda (conserva una zona); HP: paso alto (conserva los agudos). "
                             "12 o 24: pendiente suave o pronunciada. Haz clic para cambiar." },
            { "filter_cutoff", "CUTOFF (frecuencia de corte)\nDónde actúa el filtro. En un paso bajo, bajarlo oscurece el sonido." },
            { "filter_reso", "RESO (resonancia)\nRealza la frecuencia de corte: el sonido se vuelve más «sinte» y silbante." },
            { "filter_rate", "RATE\nVelocidad del LFO que mueve el corte. Sin efecto si Depth está a cero." },
            { "filter_depth", "DEPTH\nAmplitud del barrido del LFO. A cero, el filtro queda fijo." },

            // Wah
            { "wah_pedal", "PEDAL\nPosición del pedal: talón (grave) a punta (agudo). Se puede controlar con un pedal de expresión MIDI." },
            { "wah_reso", "RESO\nFuerza del pico del wah. Alto: muy vocal; bajo: más sutil." },
            { "wah_blend", "CLEAN\nCantidad de sonido directo bajo el wah, para no perder los graves." },

            // Synth
            { "synth_wave", "WAVE\nForma del oscilador: Square (cuadrada, hueca y redonda) o Saw (diente de sierra, rica y brillante). "
                            "Haz clic para cambiar." },
            { "synth_cutoff", "CUTOFF\nApertura del filtro en reposo. Baja: sonido oscuro; alta: sonido brillante." },
            { "synth_reso", "RESO\nResonancia del filtro, para el lado «acid» y silbante." },
            { "synth_env", "ENV\nCuánto abre tu ataque el filtro. Alto: cada nota hace «uau»." },
            { "synth_mix", "MIX\nMezcla el sintetizador con tu bajo." },

            // Pitch
            { "pitch_shift", "SHIFT\nDesplazamiento en semitonos: +12 = octava arriba, +7 = quinta, -12 = octava abajo." },
            { "pitch_detune", "DETUNE\nDesafinación fina, en centésimas de semitono. Unas pocas engordan el sonido como un chorus." },
            { "pitch_mix", "MIX\nMezcla la nota transpuesta con tu nota original." },

            // Amp
            { "amp_gain", "GAIN\nCantidad de saturación del previo. Bajo: limpio; hacia 5: grano ligero; más allá: saturado. "
                          "En NAM Capture, ajusta el nivel de entrada de la captura." },
            { "amp_bass", "BASS\nGraves: la redondez y el peso." },
            { "amp_mid", "MIDDLE\nMedios: lo que hace destacar el bajo en la mezcla. Bajarlos da un sonido vaciado, subirlos un sonido que corta." },
            { "amp_mid_freq", "FREQ\nFrecuencia sobre la que actúa Middle: baja (200 Hz) para el cuerpo, alta (1 a 3 kHz) para el ataque. "
                              "Inactivo en Tweed, cuyo circuito fija las frecuencias." },
            { "amp_treble", "TREBLE\nAgudos: la claridad y el ruido de las cuerdas." },
            { "amp_master", "MASTER\nVolumen general del ampli. Si se enciende el piloto OUT CLIP, bájalo." },
            { "amp_blend", "DI BLEND\nMezcla el sonido directo del bajo (antes del ampli) con el ampli y la pantalla, como en estudio: "
                           "el directo aporta graves nítidos, el ampli el carácter." },
            { "amp_contour", "CONTOUR\nVacía los medios hacia 380 Hz y abre graves y agudos: el sonido «sonrisa», ideal para el slap." },
            { "amp_deep", "DEEP\nAñade graves y quita un poco de 500 Hz, para un sonido más profundo." },
            { "amp_bright", "BRIGHT\nAñade agudos, para más claridad y chasquido." },
            { "amp_low_cut", "LO CUT\nCorta los subgraves a la salida del ampli. Útil para apretar el sonido o para altavoces pequeños "
                             "que saturan en los graves." },
            { "amp_high_cut", "HI CUT\nCorta los agudos a la salida del ampli, para suavizar un sonido demasiado brillante o una distorsión que chisporrotea." },

            // Amp models
            { "model.Hi-Fi", "HI-FI\nTransistor neutro y moderno, inspirado en el Markbass Little Mark. Rápido y preciso, ideal para slap y funk." },
            { "model.Tube 300", "TUBE 300\nTodo válvulas de 300 W, inspirado en el Ampeg SVT: medios-graves cálidos, agudos redondeados, el sonido rock." },
            { "model.Crossover", "CROSSOVER\nInspirado en el Darkglass B7K: los graves quedan limpios, solo satura la parte alta. El sonido metal moderno." },
            { "model.Tweed", "TWEED\nInspirado en el Fender Bassman de 1959, con su ecualizador pasivo modelado: redondo, vintage, satura pronto." },
            { "model.Flip Top", "FLIP TOP\nPequeño ampli de válvulas de estudio, inspirado en el Ampeg B-15: oscuro y redondo, el sonido Motown." },
            { "model.Solid Punch", "SOLID PUNCH\nTransistor rockero inspirado en el Gallien-Krueger 800RB: medios vaciados, mordiente a 4 kHz." },
            { "model.Tube DI", "TUBE DI\nPrevio DI inspirado en el SansAmp Bass Driver: directo y con pegada, de limpio a granulado." },
            { "model.NAM Capture", "NAM CAPTURE\nReproduce una captura de un ampli real (archivo .nam de Neural Amp Modeler), "
                                   "gratis en TONE3000. Cárgala con LOAD NAM." },

            // Cabinets
            { "cab.Off (DI)", "OFF (DI)\nSin pantalla: el sonido directo del ampli, como una salida DI." },
            { "cab.8x10", "8x10\nLa gran nevera de los conciertos de rock: graves potentes, medios presentes." },
            { "cab.4x10", "4x10\nVersátil y con pegada, con más ataque que el 8x10." },
            { "cab.1x15", "1x15\nUn solo altavoz grande: redondo, profundo, vintage, pocos agudos." },
            { "cab.2x12", "2x12\nCompacto y centrado en medios, corta bien en un grupo." },
            { "cab.2x10 Hi-Fi", "2x10 HI-FI\nCon tweeter: sonido amplio y claro, para slap y jazz moderno." },
            { "cab.IR File", "IR FILE\nReproduce tu propia respuesta de impulso (IR) de pantalla, cargada con LOAD IR." },

            // EQ
            { "eq_on", "ON\nEnciende o apaga el ecualizador. Se enciende solo en cuanto tocas un fader o un punto." },
            { "cut_low", "LOW CUT\nCorta de golpe todo lo que está por debajo de 80 Hz: el sonido se aprieta y deja sitio al bombo." },
            { "kill_low", "KILL LOW\nElimina los graves (por debajo de 250 Hz). Para un corte en directo o para aislar el ataque." },
            { "kill_mid", "KILL MID\nElimina los medios (de 250 Hz a 2,5 kHz): solo quedan el grave y el chasquido." },
            { "kill_high", "KILL HIGH\nElimina los agudos (por encima de 2,5 kHz): sonido oscuro y redondo, como un bajo con cuerdas lisas." },
            { "cut_high", "HIGH CUT\nCorta de golpe todo lo que está por encima de 5 kHz: suaviza el sonido y quita el soplido." },
            { "geq_33", "33 Hz\nSubgraves: la vibración que se siente más que se oye. Súbelos con cuidado." },
            { "geq_63", "63 Hz\nEl peso y la base del sonido." },
            { "geq_125", "125 Hz\nLa redondez. Demasiado: el sonido retumba." },
            { "geq_250", "250 Hz\nEl cuerpo. Demasiado: el sonido se embarra." },
            { "geq_500", "500 Hz\nEl cuerpo y el lado «de caja». Bajarlo despeja el sonido." },
            { "geq_1k", "1 kHz\nLa presencia: lo que hace oír el bajo en la mezcla." },
            { "geq_2k", "2 kHz\nEl ataque y el grano de las notas." },
            { "geq_4k", "4 kHz\nEl chasquido de las cuerdas y el ruido de los dedos." },
            { "geq_8k", "8 kHz\nEl brillo y el soplido." },
            { "peq_low_cut", "LOW CUT\nPunto gris de la izquierda: corta todo lo que está por debajo. Arrástralo a la derecha para apretar los graves. "
                             "Solo se mueve a izquierda y derecha: no tiene ganancia." },
            { "peq_low_freq", "LOW (shelf grave)\nSube o baja todos los graves por debajo de su frecuencia. Arrastra arriba o abajo para la ganancia, "
                              "a izquierda o derecha para la frecuencia." },
            { "peq_mid1_freq", "LO-MID\nBanda de medios-graves: cuerpo y barro. Arrastra para ajustar, rueda del ratón para el ancho (Q), "
                               "doble clic para volver a cero." },
            { "peq_mid2_freq", "HI-MID\nBanda de medios-agudos: ataque y presencia. Arrastra para ajustar, rueda del ratón para el ancho (Q), "
                               "doble clic para volver a cero." },
            { "peq_high_freq", "HIGH (shelf agudo)\nSube o baja todos los agudos por encima de su frecuencia." },
            { "peq_high_cut", "HIGH CUT\nPunto gris de la derecha: corta todo lo que está por encima. Arrástralo a la izquierda para suavizar el sonido. "
                              "Solo se mueve a izquierda y derecha: no tiene ganancia." },

            // Post-amp effects
            { "phaser_rate", "RATE\nVelocidad del movimiento." },
            { "phaser_depth", "DEPTH\nAmplitud del movimiento." },
            { "phaser_feedback", "REGEN\nRealimentación: cuanto más alta, más marcado y cantarín el efecto." },
            { "flanger_rate", "RATE\nVelocidad del barrido." },
            { "flanger_depth", "DEPTH\nAmplitud del barrido." },
            { "flanger_feedback", "REGEN\nRealimentación: cuanto más alta, más metálico el sonido." },
            { "flanger_mix", "MIX\nMezcla el efecto con el sonido directo." },
            { "chorus_rate", "RATE\nVelocidad de la ondulación. Lenta: amplia y suave; rápida: efecto vibrato." },
            { "chorus_depth", "DEPTH\nAmplitud de la desafinación." },
            { "chorus_mix", "MIX\nMezcla el efecto con el sonido directo." },
            { "tremolo_rate", "RATE\nVelocidad de las variaciones de volumen." },
            { "tremolo_depth", "DEPTH\nAmplitud de las variaciones: al máximo, el sonido se corta en cada ciclo." },
            { "tremolo_shape", "SHAPE\nForma de onda: a la izquierda una ondulación suave, a la derecha un corte seco." },
            { "delay_time", "TIME\nTiempo entre lo que tocas y el eco, en milisegundos. O marca el tempo con TAP." },
            { "delay_feedback", "REPEAT\nNúmero de repeticiones: bajo, un solo eco; alto, ecos que duran." },
            { "delay_mix", "MIX\nVolumen de los ecos." },
            { "reverb_size", "SIZE\nTamaño de la sala: de una habitación pequeña a una gran iglesia." },
            { "reverb_damping", "DAMP\nAbsorción de agudos: alto para una reverb oscura y discreta." },
            { "reverb_mix", "MIX\nCantidad de reverb." },
            { "limiter_on", "LIMIT\nLimitador de salida: impide que el sonido supere -1 dBFS, para proteger tus oídos y evitar la saturación "
                            "digital. Transparente mientras te mantengas por debajo de ese nivel." },

            // Interface
            { "ui.footswitch", "FOOTSWITCH\nEnciende o apaga el pedal: el LED se ilumina cuando está activo. "
                               "Clic derecho: MIDI learn, para controlarlo con un pad de tu controlador." },
            { "ui.tap", "TAP\nPulsa al menos dos veces al ritmo: el tiempo del delay se ajusta a tu tempo." },
            { "ui.pedalPresets", "PRESETS DEL PEDAL\nAjustes listos para este pedal. El resto de tu sonido no cambia." },
            { "ui.removePedal", "QUITAR\nApaga el pedal y lo guarda. Puedes volver a ponerlo con + ADD." },
            { "ui.addPedal", "+ ADD\nAñade un pedal a este pedalboard. Después, arrastra un pedal para cambiarlo de sitio: "
                             "el sonido sigue el orden mostrado, de izquierda a derecha." },
            { "ui.presetBox", "PRESETS\nLos sonidos de fábrica y los tuyos. Elige uno para ajustarlo todo de una vez." },
            { "ui.previousPreset", "PRESET ANTERIOR" },
            { "ui.nextPreset", "PRESET SIGUIENTE" },
            { "ui.savePreset", "SAVE\nGuarda el sonido actual como preset, en Documentos/Bass Amp/Presets." },
            { "ui.record", "REC\nGraba lo que sale de la aplicación (efectos incluidos) en WAV, en Documentos/Bass Amp/Recordings. "
                           "Haz clic de nuevo para parar." },
            { "ui.recordings", "FILES\nAbre la carpeta de grabaciones." },
            { "ui.tuner", "AFINADOR\nToca una cuerda al aire: se muestra la nota y la aguja se centra cuando la cuerda está afinada. "
                          "Mi La Re Sol (E A D G), y Si (B) grave en un 5 cuerdas." },
            { "ui.mute", "MUTE\nSilencia la salida mientras afinas." },
            { "ui.meters", "NIVELES\nIN: lo que entra (tu bajo); OUT: lo que sale; COMP: reducción del compresor. "
                           "CLIP rojo: saturación, baja la ganancia de tu interfaz (IN) o el Master (OUT). Haz clic para borrar los pilotos." },
            { "ui.eqPresets", "PRESETS DE EQ\nCurvas listas: voz, guitarra, bajo redondo, slap… Solo cambia el ecualizador." },
            { "ui.eqBasic", "BASIC\nEcualizador gráfico de 9 bandas, con faders." },
            { "ui.eqAdvanced", "ADVANCED\nEcualizador paramétrico: 6 puntos que arrastrar sobre la curva, más preciso." },
            { "ui.eqScreen", "PANTALLA DEL EQ\nLa curva muestra la corrección; el fondo, el espectro del sonido de salida. "
                             "Localiza los picos de frecuencia para corregirlos." },
            { "ui.ampPresets", "PRESETS DE AMPLI\nAjustes de ampli y pantalla listos. Los pedales y el EQ no cambian." },
            { "ui.language", "IDIOMA\nEl idioma de las burbujas de ayuda y los menús. Los nombres de los controles quedan en inglés, "
                             "como en el equipo real." },
            { "ui.fullScreen", "PANTALLA COMPLETA\nAmplía la aplicación a toda la pantalla. F11 hace lo mismo; Escape vuelve a la ventana." },
            { "ui.loadIr", "LOAD IR\nCarga una respuesta de impulso de pantalla (WAV, AIFF, FLAC) y pone la pantalla en IR File." },
            { "ui.loadNam", "LOAD NAM\nCarga una captura de ampli .nam (TONE3000 ofrece miles). "
                            "Las perillas de tono y la pantalla siguen activas después de ella." },
            { "ui.player", "PLAYER\nReproduce una canción o una pista para tocar encima." },
            { "ui.playerLoad", "LOAD\nAbre un archivo de audio (WAV, AIFF, FLAC, OGG, MP3) y lo reproduce." },
            { "ui.playerPlay", "PLAY / PAUSE" },
            { "ui.playerStop", "STOP\nPara y vuelve al principio." },
            { "ui.playerPosition", "POSITION\nArrastra para moverte por la canción." },
            { "ui.playerLoop", "LOOP\nRepite la canción en bucle." },
            { "ui.reamp", "RE-AMP\nEnvía el archivo a la cadena en lugar de tu bajo: para pasar una pista DI grabada "
                          "por los pedales y el ampli, y probar ajustes sin tocar." },
            { "ui.playerVolume", "VOL\nVolumen de la canción." },
            { "ui.looper", "LOOPER\n4 pistas de bucles para construir un tema en directo. Haz clic en un pad para grabar, "
                           "OPEN para el looper completo." },
            { "ui.loopPad", "PISTA\n1.er clic: graba. 2.º clic: el bucle suena. Después cada clic añade una capa (DUB) "
                            "o vuelve a la reproducción. Clic derecho: MIDI learn, para lanzarla con un pad o un pedal." },
            { "ui.looperOpen", "OPEN\nAbre el looper completo: ajustes de cada pista, deshacer, exportar." },
            { "ui.looperClose", "CLOSE\nCierra el looper: los bucles siguen sonando." },
            { "ui.looperStopAll", "STOP ALL\nPara todas las pistas, o las relanza si todo está parado. Vuelven a tiempo." },
            { "ui.looperClearAll", "CLEAR ALL\nBorra todas las pistas." },
            { "ui.looperUndo", "UNDO\nDeshace la última grabación o la última capa añadida." },
            { "ui.looperRedo", "REDO\nDevuelve lo que UNDO acaba de quitar." },
            { "ui.looperSync", "SYNC\nCon el metrónomo en marcha, los bucles empiezan y terminan en un compás: "
                               "lanza la grabación y espera al compás siguiente. "
                               "Si no, el primer bucle fija la duración que siguen los demás." },
            { "ui.looperExport", "EXPORT WAV\nGuarda la mezcla de las pistas que suenan en un archivo WAV, "
                                 "en Documentos/Bass Amp/Recordings." },
            { "ui.looperTrackStop", "STOP\nPara esta pista sin borrarla. Un clic en su pad la relanza, a tiempo." },
            { "ui.looperTrackClear", "CLEAR\nBorra esta pista." },
            { "ui.looperReverse", "REV\nReproduce la pista al revés." },
            { "ui.looperHalf", "HALF\nReproduce la pista a mitad de velocidad: una octava más grave." },
            { "ui.looperLevel", "LEVEL\nVolumen de la pista." },
            { "ui.metronome", "METRÓNOMO\nUn clic para practicar a tempo. Nunca lo graba REC." },
            { "ui.metronomeOn", "ON\nInicia o para el metrónomo." },
            { "ui.metronomeTempo", "TEMPO\nVelocidad en pulsos por minuto (BPM)." },
            { "ui.metronomeTap", "TAP\nMarca el tempo para ajustar el metrónomo." },
            { "ui.metronomeVolume", "VOL\nVolumen del clic." },
        },
        {
            { "no file loaded", "ningún archivo cargado" },
            { "No capture loaded", "Ninguna captura cargada" },
            { "Can't read this file", "No se puede leer el archivo" },
            { "Set audio to", "Pon el audio a" },
            { "Load a Neural Amp Modeler capture", "Cargar una captura de Neural Amp Modeler" },
            { "Load a cabinet impulse response", "Cargar una respuesta de impulso de pantalla" },
            { "Saved", "Guardado:" },
            { "Export failed", "Error al exportar" },
            { "Synced to the metronome: loops start and end on a bar.", "Sincronizado con el metrónomo: los bucles empiezan y terminan en un compás." },
            { "Free: the first loop sets the length the others follow.", "Libre: el primer bucle fija la duración que siguen los demás." },
            { "MIDI learn", "MIDI learn" },
            { "Cancel MIDI learn", "Cancelar MIDI learn" },
            { "Forget", "Olvidar" },
            { "MOVE A MIDI CONTROL", "MUEVE UN CONTROL MIDI" },
            { "Choose a backing track", "Elegir una canción" },
            { "No track loaded", "Ninguna canción cargada" },
            { "Factory", "Fábrica" },
            { "User", "Míos" },
            { "Save preset", "Guardar preset" },
            { "Preset name:", "Nombre del preset:" },
            { "copy", "copia" },
            { "Save", "Guardar" },
            { "Cancel", "Cancelar" },
            { "No audio device: Options > Audio/MIDI Settings", "Ningún dispositivo de audio: Options > Audio/MIDI Settings" },
            { "latency", "latencia" },
            { "Language", "Idioma" },
        },
    };

    return translations;
}
