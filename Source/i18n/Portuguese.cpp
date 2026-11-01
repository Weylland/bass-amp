#include "Translations.h"

const Translations& portugueseTranslations()
{
    static const Translations translations {
        {
            // Pedals
            { "pedal.gate", "GATE (Noise Gate)\nCorta o som quando você não está tocando, para eliminar o chiado e o zumbido dos captadores. "
                            "Coloque no início da cadeia, principalmente antes de um drive ou fuzz, que amplificam o ruído." },
            { "pedal.comp", "COMP (Compressor)\nEquilibra o volume: abaixa as notas fortes e destaca as fracas. "
                            "O som fica mais regular e presente. Indispensável para slap e para um som de estúdio." },
            { "pedal.octaver", "OCTAVER\nAdiciona à sua nota a oitava abaixo (Sub) e a oitava acima (Up). "
                               "Toque uma nota de cada vez: acordes o confundem. Ideal para um som enorme ou uma linha de synth." },
            { "pedal.synth", "SYNTH\nTransforma o baixo em sintetizador: um oscilador segue sua nota através de um filtro "
                             "que abre com o ataque. Toque uma nota limpa de cada vez para ele acompanhar." },
            { "pedal.pitch", "PITCH (Pitch Shifter)\nMove sua nota de -12 a +12 semitons, misturada ao som direto. "
                             "+12 para uma oitava acima, +7 para uma quinta, um pouco de detune para engrossar." },
            { "pedal.filter", "FILTER\nFiltro ressonante, ajustado à mão ou movido por um LFO (uma oscilação lenta). "
                              "Passa-baixa para um som escuro e redondo, passa-alta para um som fino, passa-banda para um efeito «rádio»." },
            { "pedal.envelope", "ENVELOPE (Auto-Wah)\nUm filtro que abre com o seu ataque: quanto mais forte você toca, mais "
                                "brilhante o som. O som funk por excelência (Bootsy Collins, Mark Adams)." },
            { "pedal.wah", "WAH\nUm filtro que se move com o pé e faz «wah». Aqui quem ajusta é o botão Pedal, ou um pedal "
                           "de expressão MIDI (CC 11 por padrão). Clean mantém o som direto para não perder os graves." },
            { "pedal.fuzz", "FUZZ\nDistorção muito forte e quadrada, como um pedal de silício vintage. Blend mistura som limpo "
                            "por baixo para manter os graves firmes." },
            { "pedal.drive", "DRIVE (Overdrive)\nDistorção mais suave e natural que o fuzz, como um amplificador esquentando. "
                             "Blend mistura som limpo para mais definição." },
            { "pedal.phaser", "PHASER\nCria vales que se movem no espectro: um som giratório, líquido, típico dos anos setenta." },
            { "pedal.flanger", "FLANGER\nComo o phaser, porém mais metálico: o som passa por um atraso muito curto que varia. "
                               "Com bastante regen você tem o efeito «avião a jato»." },
            { "pedal.chorus", "CHORUS\nDobra seu som com uma cópia levemente desafinada e em movimento, para deixá-lo mais largo. "
                              "Os graves ficam no centro e firmes; só a parte alta do espectro é processada." },
            { "pedal.tremolo", "TREMOLO\nFaz o volume variar no ritmo, como os amplificadores antigos. Shape vai de uma ondulação suave "
                               "a um corte seco." },
            { "pedal.delay", "DELAY (Eco)\nRepete o que você toca depois de um tempo ajustável. TAP: bata o tempo para ajustar o eco à música." },
            { "pedal.reverb", "REVERB\nSimula uma sala: o som se prolonga e ganha espaço. "
                              "Use com moderação no baixo para não embolar os graves." },

            // Gate
            { "gate_threshold", "THRESH (Limiar)\nNível abaixo do qual o gate corta o som. Aumente até o chiado sumir nas pausas, "
                                "sem cortar o final das notas." },
            { "gate_release", "RELEASE\nVelocidade com que o gate fecha. Curto: corta seco; longo: deixa as notas morrerem naturalmente." },

            // Compressor
            { "comp_threshold", "THRESH (Limiar)\nNível a partir do qual o compressor trabalha. Quanto mais baixo, mais ele comprime. "
                                "Veja o medidor COMP em LEVELS: 3 a 6 dB de redução é um bom começo." },
            { "comp_ratio", "RATIO\nForça da compressão acima do limiar. De 2:1 a 4:1: natural; a partir de 8:1: muito uniforme, "
                            "quase um limitador." },
            { "comp_attack", "ATTACK\nVelocidade de reação. Lento (a partir de 10 ms): deixa passar o ataque da nota, mais punch; "
                             "rápido: achata o ataque." },
            { "comp_release", "RELEASE\nTempo para a compressão soltar depois de uma nota. Curto demais: o som «bombeia»; longo demais: "
                              "as notas seguintes continuam comprimidas." },
            { "comp_makeup", "MAKEUP\nAumenta o volume depois da compressão, para voltar ao nível do pedal desligado." },
            { "comp_mix", "MIX\nMistura o som comprimido com o som direto (compressão paralela): mantém o ataque natural "
                          "e ganha densidade." },

            // Octaver
            { "oct_sub", "SUB\nVolume da oitava abaixo. Para um som muito grave, tipo synth." },
            { "oct_up", "UP\nVolume da oitava acima, que adiciona um som brilhante e oco." },
            { "oct_dry", "DRY\nVolume do seu som original na mistura." },

            // Envelope
            { "env_type", "TYPE\nForma do filtro: LP (passa-baixa, o wah clássico), BP (passa-banda, mais nasal) ou HP (passa-alta, mais fino). "
                          "Clique para mudar." },
            { "env_dir", "SWEEP\nUp: o filtro abre com o ataque. Down: ele fecha, para um «wouw» invertido." },
            { "env_sens", "SENS (Sensibilidade)\nO quanto o seu ataque abre o filtro. Ajuste conforme a força com que você toca." },
            { "env_reso", "RESO (Ressonância)\nRealça a frequência do filtro. Quanto mais alta, mais marcado o «quack»." },
            { "env_mix", "MIX\nMistura o efeito com o som direto para manter os graves." },

            // Fuzz and drive
            { "fuzz_amount", "FUZZ\nQuantidade de distorção." },
            { "fuzz_tone", "TONE\nÀ esquerda escuro, à direita agressivo." },
            { "fuzz_blend", "BLEND\nMistura o fuzz com o som limpo. Para a esquerda, os graves ficam limpos." },
            { "dist_drive", "DRIVE\nQuantidade de distorção, de um leve crunch até a saturação de verdade." },
            { "dist_tone", "TONE\nTira agudos da distorção. Mais baixo: mais redondo; mais alto: mais agressivo." },
            { "dist_blend", "BLEND\nMistura a distorção com o som limpo para manter as notas definidas." },

            // Filter
            { "filter_type", "TYPE\nLP: passa-baixa (mantém os graves); BP: passa-banda (mantém uma faixa); HP: passa-alta (mantém os agudos). "
                             "12 ou 24: inclinação suave ou forte. Clique para mudar." },
            { "filter_cutoff", "CUTOFF (Frequência de corte)\nOnde o filtro atua. Em passa-baixa, abaixar escurece o som." },
            { "filter_reso", "RESO (Ressonância)\nRealça a frequência de corte: o som fica mais «synth» e assobiado." },
            { "filter_rate", "RATE\nVelocidade do LFO que move a frequência de corte. Sem efeito se Depth estiver em zero." },
            { "filter_depth", "DEPTH\nAmplitude do movimento do LFO. Em zero, o filtro fica parado." },

            // Wah
            { "wah_pedal", "PEDAL\nPosição do pedal: calcanhar (grave) até a ponta (agudo). Controlável por um pedal de expressão MIDI." },
            { "wah_reso", "RESO\nIntensidade do pico do wah. Alta: bem vocal; baixa: mais discreto." },
            { "wah_blend", "CLEAN\nParte de som direto sob o wah, para não perder os graves." },

            // Synth
            { "synth_wave", "WAVE\nForma do oscilador: Square (quadrada, oca e redonda) ou Saw (dente de serra, rica e brilhante). "
                            "Clique para mudar." },
            { "synth_cutoff", "CUTOFF\nAbertura do filtro em repouso. Baixa: som escuro; alta: som brilhante." },
            { "synth_reso", "RESO\nRessonância do filtro, para o caráter «acid» e o assobio." },
            { "synth_env", "ENV\nO quanto o seu ataque abre o filtro. Alto: cada nota faz «wow»." },
            { "synth_mix", "MIX\nMistura o synth com o seu baixo." },

            // Pitch
            { "pitch_shift", "SHIFT\nDeslocamento em semitons: +12 = uma oitava acima, +7 = uma quinta, -12 = uma oitava abaixo." },
            { "pitch_detune", "DETUNE\nDesafinação fina em cents. Alguns cents engrossam o som, como um chorus." },
            { "pitch_mix", "MIX\nMistura a nota deslocada com a sua nota original." },

            // Amp
            { "amp_gain", "GAIN\nO quanto o pré-amplificador é exigido. Baixo: limpo; perto de 5: leve crunch; acima: saturado. "
                          "Em NAM Capture, o nível de entrada da captura." },
            { "amp_bass", "BASS\nGraves: redondeza e peso." },
            { "amp_mid", "MIDDLE\nMédios: o que faz o baixo aparecer na mixagem. Abaixar dá um som cavado, aumentar um som que corta." },
            { "amp_mid_freq", "FREQ\nFrequência em que o Middle atua: grave (200 Hz) para o corpo, alta (1 a 3 kHz) para o ataque. "
                              "Inativo no Tweed '59, cujo circuito fixa as frequências." },
            { "amp_treble", "TREBLE\nAgudos: clareza e ruídos das cordas." },
            { "amp_master", "MASTER\nVolume geral do amplificador. Se a luz OUT CLIP acender, abaixe." },
            { "amp_blend", "DI BLEND\nMistura o som direto do baixo (antes do amp) com o amp e a caixa, como no estúdio: "
                           "o DI traz graves limpos, o amp traz o caráter." },
            { "amp_contour", "CONTOUR\nCava os médios em torno de 380 Hz e abre graves e agudos: o som «sorriso», ideal para slap." },
            { "amp_deep", "DEEP\nAdiciona graves e tira um pouco de 500 Hz, para um som mais profundo." },
            { "amp_bright", "BRIGHT\nAdiciona agudos, para mais clareza e brilho." },
            { "amp_low_cut", "LO CUT\nCorta os subgraves na saída do amp. Útil para enxugar o som, ou para caixas pequenas "
                             "que saturam nos graves." },
            { "amp_high_cut", "HI CUT\nCorta os agudos na saída do amp, para suavizar um som brilhante demais ou uma distorção que arranha." },

            // Amp models
            { "model.Italian Hi-Fi", "ITALIAN HI-FI\nAmp transistorizado neutro e moderno, inspirado no Markbass Little Mark. Rápido e preciso, ideal para slap e funk." },
            { "model.Super Valve", "SUPER VALVE\nValvulado de 300 W, inspirado no Ampeg SVT: médios-graves quentes, agudos redondos, o som do rock." },
            { "model.Nordic Drive", "NORDIC DRIVE\nInspirado no Darkglass B7K: os graves ficam limpos, só a parte alta satura. O som do metal moderno." },
            { "model.Tweed '59", "TWEED '59\nInspirado no Fender Bassman de 1959, com os controles de tonalidade passivos modelados: redondo, vintage, satura cedo." },
            { "model.Flip Top '60s", "FLIP TOP '60S\nPequeno amp valvulado de estúdio, inspirado no Ampeg B-15: escuro e redondo, o som da Motown." },
            { "model.West Coast 800", "WEST COAST 800\nAmp transistorizado de rock, inspirado no Gallien-Krueger 800RB: médios cavados, brilho em 4 kHz." },
            { "model.Stomp DI", "STOMP DI\nPré-amplificador DI, inspirado no SansAmp Bass Driver: direto e encorpado, de limpo a crunch." },
            { "model.NAM Capture", "NAM CAPTURE\nToca uma captura de um amplificador real (um arquivo .nam do Neural Amp Modeler), "
                                   "gratuitas no TONE3000. Carregue com LOAD NAM." },

            // Cabinets
            { "cab.Off (DI)", "OFF (DI)\nSem caixa: o som direto do amp, como uma saída DI." },
            { "cab.8x10", "8x10\nA grande geladeira dos shows de rock: graves potentes, médios presentes." },
            { "cab.4x10", "4x10\nVersátil e encorpada, com mais ataque que a 8x10." },
            { "cab.1x15", "1x15\nUm único alto-falante grande: redondo, profundo, vintage, poucos agudos." },
            { "cab.2x12", "2x12\nCompacta e centrada nos médios, aparece bem numa banda." },
            { "cab.2x10 Hi-Fi", "2x10 HI-FI\nCom tweeter: som amplo e claro para slap e jazz moderno." },
            { "cab.IR File", "IR FILE\nToca a sua própria resposta ao impulso (IR) de caixa, carregada com LOAD IR." },

            // EQ
            { "eq_on", "ON\nLiga ou desliga o equalizador. Ele liga sozinho assim que você mexe num fader ou num ponto." },
            { "cut_low", "CUT 80 HZ\nCorta tudo abaixo de 80 Hz de uma vez: o som fica mais enxuto e deixa espaço para o bumbo." },
            { "kill_low", "KILL LOW\nRemove os graves (abaixo de 250 Hz). Para um drop ao vivo ou para isolar o ataque." },
            { "kill_mid", "KILL MID\nRemove os médios (de 250 Hz a 2,5 kHz): sobram só os graves e o estalo." },
            { "kill_high", "KILL HIGH\nRemove os agudos (acima de 2,5 kHz): som escuro e redondo, como um baixo com cordas flatwound." },
            { "cut_high", "CUT 5 KHZ\nCorta tudo acima de 5 kHz de uma vez: suaviza o som e remove o chiado." },
            { "geq_33", "33 Hz\nSubgraves: o ronco que se sente mais do que se ouve. Aumente com cuidado." },
            { "geq_63", "63 Hz\nPeso e base do som." },
            { "geq_125", "125 Hz\nRedondeza. Demais: o som retumba." },
            { "geq_250", "250 Hz\nCorpo. Demais: o som embola." },
            { "geq_500", "500 Hz\nCorpo e lado «papelão». Abaixar abre o som." },
            { "geq_1k", "1 kHz\nPresença: o que faz o baixo ser ouvido na mixagem." },
            { "geq_2k", "2 kHz\nAtaque e textura das notas." },
            { "geq_4k", "4 kHz\nEstalo das cordas e ruídos dos dedos." },
            { "geq_8k", "8 kHz\nBrilho e chiado." },
            { "peq_low_cut", "LOW CUT\nPonto cinza à esquerda: corta tudo abaixo dele. Arraste para a direita para enxugar os graves. "
                             "Ele só se move para a esquerda e para a direita: não tem ganho." },
            { "peq_low_freq", "LOW (Low Shelf)\nAumenta ou abaixa todos os graves abaixo da sua frequência. Arraste para cima ou para baixo para o ganho, "
                              "para a esquerda ou a direita para a frequência." },
            { "peq_mid1_freq", "LO-MID\nBanda de médios-graves: corpo e embolamento. Arraste para ajustar, roda do mouse para a largura (Q), "
                               "duplo clique para zerar." },
            { "peq_mid2_freq", "HI-MID\nBanda de médios-agudos: ataque e presença. Arraste para ajustar, roda do mouse para a largura (Q), "
                               "duplo clique para zerar." },
            { "peq_high_freq", "HIGH (High Shelf)\nAumenta ou abaixa todos os agudos acima da sua frequência." },
            { "peq_high_cut", "HIGH CUT\nPonto cinza à direita: corta tudo acima dele. Arraste para a esquerda para suavizar o som. "
                              "Ele só se move para a esquerda e para a direita: não tem ganho." },

            // Post-amp effects
            { "phaser_rate", "RATE\nVelocidade do movimento." },
            { "phaser_depth", "DEPTH\nAmplitude do movimento." },
            { "phaser_feedback", "REGEN\nRealimentação: quanto mais alta, mais marcado e cantado o efeito." },
            { "flanger_rate", "RATE\nVelocidade do movimento." },
            { "flanger_depth", "DEPTH\nAmplitude do movimento." },
            { "flanger_feedback", "REGEN\nRealimentação: quanto mais alta, mais metálico o som." },
            { "flanger_mix", "MIX\nMistura o efeito com o som direto." },
            { "chorus_rate", "RATE\nVelocidade da ondulação. Lenta: amplo e suave; rápida: efeito vibrato." },
            { "chorus_depth", "DEPTH\nQuantidade de desafinação." },
            { "chorus_mix", "MIX\nMistura o efeito com o som direto." },
            { "tremolo_rate", "RATE\nVelocidade das variações de volume." },
            { "tremolo_depth", "DEPTH\nForça das variações: no máximo, o som é cortado a cada ciclo." },
            { "tremolo_shape", "SHAPE\nForma da onda: à esquerda ondulação suave, à direita corte seco." },
            { "delay_time", "TIME\nTempo entre o que você toca e o eco, em milissegundos. Ou bata o tempo com TAP." },
            { "delay_feedback", "REPEAT\nNúmero de repetições: baixo, um único eco; alto, ecos que duram bastante." },
            { "delay_mix", "MIX\nVolume dos ecos." },
            { "reverb_size", "SIZE\nTamanho da sala: do quarto pequeno à grande igreja." },
            { "reverb_damping", "DAMP\nAbafamento dos agudos: alto para uma reverb escura e discreta." },
            { "reverb_mix", "MIX\nQuantidade de reverb." },
            { "limiter_on", "LIMIT\nLimitador de saída: mantém o som abaixo de -1 dBFS para proteger seus ouvidos e evitar a saturação digital. "
                            "Transparente enquanto você ficar abaixo desse nível." },

            // Interface
            { "ui.footswitch", "FOOTSWITCH\nLiga ou desliga o pedal: o LED acende quando ele está ativo. Clique direito: MIDI "
                               "learn, para acionar com um pad do seu controlador. Um ponto dourado num controle indica que um "
                               "controle MIDI o comanda." },
            { "ui.tap", "TAP\nBata pelo menos duas vezes no ritmo: o tempo do delay se ajusta ao seu andamento." },
            { "ui.pedalPresets", "PRESETS DO PEDAL\nAjustes prontos para este pedal. O resto do seu som não muda. Salvar... "
                                 "adiciona seus próprios ajustes à lista." },
            { "ui.removePedal", "REMOVER\nDesliga o pedal e o guarda. Traga de volta com + ADD." },
            { "ui.addPedal", "+ ADD\nAdiciona um pedal a este pedalboard. Depois arraste um pedal para movê-lo: "
                             "o som segue a ordem mostrada, da esquerda para a direita." },
            { "ui.presetBox", "PRESETS\nOs sons de fábrica e os seus. Escolha um para ajustar tudo de uma vez. Uma estrela "
                              "dourada indica que você mudou o som desde que o carregou: salve com SAVE para mantê-lo." },
            { "ui.previousPreset", "PRESET ANTERIOR" },
            { "ui.nextPreset", "PRÓXIMO PRESET" },
            { "ui.savePreset", "SAVE\nSalva o som atual como preset, em Documentos/Bass Amp/Presets." },
            { "ui.presetMenu", "MENU DE PRESETS\nRenomeie ou exclua o preset de usuário carregado, importe um preset que te "
                               "enviaram, exporte este som para compartilhar (um zip com seus arquivos de IR e captura) ou "
                               "abra a pasta de presets." },
            { "ui.record", "REC\nGrava o que sai do app (com os efeitos), em WAV, em Documentos/Bass Amp/Recordings. "
                           "Clique de novo para parar." },
            { "ui.recordings", "FILES\nAbre a pasta das gravações." },
            { "ui.recordSplit", "SPLIT\nDesligado: REC grava um só arquivo, a mixagem que você ouve. Ligado: dois arquivos "
                                "lado a lado, \"Bass\" (seu som com os loops) e \"Player\" (a música), para equilibrá-los ou "
                                "retrabalhá-los depois numa DAW. Ajuste antes do REC." },
            { "ui.tuner", "AFINADOR\nToque uma corda solta: a nota aparece e a agulha fica no centro quando está afinada. E A "
                          "D G, e B grave para um baixo de cinco cordas. Roda do mouse na tela: Lá de referência de 430 a 450 "
                          "Hz; duplo clique: volta para 440 Hz." },
            { "ui.mute", "MUTE\nCorta a saída enquanto você afina." },
            { "ui.meters", "NÍVEIS\nIN: o que entra (seu baixo); OUT: o que sai; COMP: redução do compressor. CLIP vermelho: "
                           "saturação, abaixe o ganho da interface (IN) ou o master (OUT). Clique para apagar as luzes. Clique "
                           "em IN 1 / IN 2 para escolher a entrada da interface onde o baixo está ligado." },
            { "ui.eqPresets", "PRESETS DE EQ\nCurvas prontas: voz, guitarra, baixo redondo, slap… Só o equalizador muda. "
                              "Salvar... adiciona seus próprios ajustes à lista." },
            { "ui.eqBasic", "BASIC\nEqualizador gráfico de 9 bandas com faders." },
            { "ui.eqAdvanced", "ADVANCED\nEqualizador paramétrico: 6 pontos para arrastar na curva, mais preciso." },
            { "ui.eqScreen", "TELA DO EQ\nA curva mostra a correção, o fundo mostra o espectro da saída. "
                             "Identifique os picos de frequência para corrigi-los." },
            { "ui.ampPresets", "PRESETS DE AMP\nAjustes prontos de amp e caixa. Pedais e EQ não mudam. Salvar... adiciona seus "
                               "próprios ajustes à lista. Também guardam o IR e a captura." },
            { "ui.power", "POWER\nA luz de energia, como num amp de verdade: fica acesa enquanto o app está rodando. Não há "
                          "nada para ajustar aqui." },
            { "ui.language", "IDIOMA\nO idioma dos balões de ajuda e dos menus. Os nomes dos controles ficam em inglês, "
                             "como no equipamento de verdade." },
            { "ui.audioSettings", "ÁUDIO\nAs configurações de áudio e MIDI: driver (ASIO, WASAPI…), interface, canais de "
                                  "entrada e saída, taxa de amostragem, tamanho do buffer (menor = menos latência) e entradas "
                                  "MIDI. Clicar na linha do driver ao lado faz o mesmo." },
            { "ui.update", "NOVA VERSÃO\nSaiu uma versão mais recente do Bass Amp. Clique para abrir a página de download. O "
                           "Bass Amp verifica uma vez por abertura consultando o GitHub; nada sobre você é enviado." },
            { "ui.fullScreen", "TELA CHEIA\nAmplia o app para a tela inteira. F11 faz o mesmo, Esc volta para a janela." },
            { "ui.loadIr", "LOAD IR\nCarrega uma resposta ao impulso de caixa (WAV, AIFF, FLAC) e coloca a caixa em IR File." },
            { "ui.loadNam", "LOAD NAM\nCarrega uma captura de amp .nam (o TONE3000 oferece milhares). "
                            "Os controles de tonalidade e a caixa continuam ativos depois." },
            { "ui.player", "PLAYER\nToca uma música ou base para você tocar junto." },
            { "ui.playerLoad", "LOAD\nAbre um arquivo de áudio (WAV, AIFF, FLAC, OGG, MP3) e o toca." },
            { "ui.playerPlay", "PLAY / PAUSA" },
            { "ui.playerStop", "STOP\nPara e volta ao início." },
            { "ui.playerPosition", "POSIÇÃO\nArraste para se mover na música." },
            { "ui.playerLoop", "LOOP\nToca a música em loop." },
            { "ui.reamp", "RE-AMP\nEnvia o arquivo para a cadeia no lugar do baixo: para passar uma faixa DI gravada "
                          "pelos pedais e pelo amp, e testar ajustes sem tocar." },
            { "ui.playerVolume", "VOL\nVolume da música." },
            { "ui.looper", "LOOPER\n4 faixas em loop para construir uma música ao vivo. Clique num pad para gravar, "
                           "OPEN para o looper completo." },
            { "ui.loopPad", "FAIXA\n1º clique: grava (com AUTO, espera a tua primeira nota; com COUNT-IN, um compasso "
                            "inteiro). 2º clique: o loop começa. Depois cada clique adiciona uma camada (DUB) ou volta à "
                            "reprodução. Clique direito: MIDI learn, para disparar por um pad ou footswitch." },
            { "ui.looperOpen", "OPEN\nAbre o looper completo: ajustes de cada faixa, desfazer, exportar." },
            { "ui.looperClose", "CLOSE\nFecha o looper: os loops continuam tocando." },
            { "ui.looperStopAll", "STOP ALL\nPara todas as faixas, ou as reinicia se todas estiverem paradas. Elas voltam no tempo." },
            { "ui.looperClearAll", "CLEAR ALL\nApaga todas as faixas." },
            { "ui.looperUndo", "UNDO\nDesfaz a última gravação ou a última camada adicionada." },
            { "ui.looperRedo", "REDO\nRestaura o que o UNDO acabou de tirar." },
            { "ui.looperSync", "SYNC\nQuando o metrônomo está tocando, os loops começam e terminam num compasso: "
                               "inicie a gravação e ela espera o próximo compasso. "
                               "Senão, o primeiro loop define a duração que os outros seguem." },
            { "ui.looperCountIn", "COUNT-IN\nCom SYNC e o metrônomo ligado, o primeiro loop espera um compasso inteiro de "
                                  "cliques antes de gravar, para você saber quando entrar." },
            { "ui.looperAuto", "AUTO\nSem metrônomo, o primeiro loop espera por você: depois do clique no pad, a gravação "
                               "começa na primeira nota que você toca, sem silêncio no início do loop. Desligado: a gravação "
                               "começa no clique." },
            { "ui.looperExport", "EXPORT WAV\nSalva a mixagem das faixas que estão tocando num arquivo WAV, "
                                 "em Documentos/Bass Amp/Recordings." },
            { "ui.looperTrackStop", "STOP\nPara esta faixa sem apagá-la. Um clique no seu pad a reinicia, no tempo." },
            { "ui.looperTrackClear", "CLEAR\nApaga esta faixa." },
            { "ui.looperTrim", "TRIM\nCorta o início ou o fim do loop, para um clique no pad que chegou um pouco cedo ou "
                               "tarde. Arrasta as alças na forma de onda enquanto toca. Só enquanto esta faixa for a única." },
            { "ui.looperTrimReset", "RESET\nVolta a tocar a gravação inteira, tal como foi gravada." },
            { "ui.looperTrimDone", "DONE\nFecha a vista de corte. O loop mantém o novo início e o novo fim." },
            { "ui.looperReverse", "REV\nToca a faixa ao contrário." },
            { "ui.looperHalf", "HALF\nToca a faixa na metade da velocidade: uma oitava abaixo." },
            { "ui.looperLevel", "LEVEL\nVolume da faixa." },
            { "ui.metronome", "METRÔNOMO\nUm clique para praticar no tempo. Ele nunca é gravado pelo REC." },
            { "ui.metronomeOn", "ON\nInicia ou para o metrônomo." },
            { "ui.metronomeTempo", "TEMPO\nVelocidade em batidas por minuto (BPM). Numa DAW, o metrônomo e o looper seguem o "
                                   "tempo da música." },
            { "ui.metronomeTap", "TAP\nBata o tempo para ajustar o metrônomo." },
            { "ui.metronomeVolume", "VOL\nVolume do clique." },
        },
        {
            { "no file loaded", "nenhum arquivo carregado" },
            { "No capture loaded", "Nenhuma captura carregada" },
            { "Can't read this file", "Arquivo ilegível" },
            { "Set audio to", "Ajuste o áudio para" },
            { "Load a Neural Amp Modeler capture", "Carregar uma captura Neural Amp Modeler" },
            { "Load a cabinet impulse response", "Carregar uma resposta ao impulso de caixa" },
            { "Saved", "Salvo:" },
            { "Export failed", "Falha na exportação" },
            { "Synced to the metronome: loops start and end on a bar.", "No tempo do metrônomo: os loops começam e terminam num compasso." },
            { "Free: the first loop sets the length the others follow.", "Livre: o primeiro loop define a duração que os outros seguem." },
            { "MIDI learn", "MIDI learn" },
            { "Cancel MIDI learn", "Cancelar MIDI learn" },
            { "Forget", "Esquecer" },
            { "MOVE A MIDI CONTROL", "MEXA UM CONTROLE MIDI" },
            { "Choose a backing track", "Escolher uma música" },
            { "No track loaded", "Nenhuma música carregada" },
            { "Factory", "Fábrica" },
            { "User", "Meus" },
            { "Save preset", "Salvar preset" },
            { "Preset name:", "Nome do preset:" },
            { "copy", "cópia" },
            { "Save", "Salvar" },
            { "Cancel", "Cancelar" },
            { "No audio device: Options > Audio/MIDI Settings", "Nenhum dispositivo de áudio: Opções > Configurações de áudio/MIDI" },
            { "latency", "latência" },
            { "Language", "Idioma" },
            { "Save...", "Salvar..." },
            { "Drag the handles to cut the start or the end of the loop. The whole take is kept.", "Arrasta as alças para cortar o início ou o fim do loop. A gravação inteira é mantida." },
            { "Synced to the metronome: a bar of count-in, then loops start and end on a bar.", "Sincronizado com o metrônomo: um compasso de contagem, depois os loops começam e terminam num compasso." },
            { "Free: recording starts on your first note; the first loop sets the length the others follow.", "Livre: a gravação começa na sua primeira nota; o primeiro loop define a duração que os outros seguem." },
            { "available", "disponível" },
            { "Rename...", "Renomear..." },
            { "Delete...", "Excluir..." },
            { "Import...", "Importar..." },
            { "Export...", "Exportar..." },
            { "Open presets folder", "Abrir a pasta de presets" },
            { "Rename preset", "Renomear o preset" },
            { "Rename", "Renomear" },
            { "Delete preset?", "Excluir o preset?" },
            { "Delete", "Excluir" },
            { "Import a preset", "Importar um preset" },
            { "Export this sound", "Exportar este som" },
            { "This file isn't a Bass Amp preset.", "Este arquivo não é um preset do Bass Amp." },
            { "File not found", "Arquivo não encontrado" },
            { "Replace preset?", "Substituir o preset?" },
            { "A preset with this name already exists.", "Já existe um preset com este nome." },
            { "Replace", "Substituir" },
            { "Audio input is muted to avoid feedback loop", "A entrada de áudio está silenciada para evitar microfonia" },
            { "Settings...", "Configurações..." },
            { "Input", "Entrada" },
            { "Options", "Opções" },
            { "Audio/MIDI Settings", "Configurações de áudio/MIDI" },
            { "Audio/MIDI Settings...", "Configurações de áudio/MIDI..." },
            { "Save current state", "Salvar o estado atual" },
            { "Save current state...", "Salvar o estado atual..." },
            { "Load a saved state", "Carregar um estado salvo" },
            { "Load a saved state...", "Carregar um estado salvo..." },
            { "Reset to default state", "Voltar ao estado padrão" },
            { "Audio device type:", "Tipo de driver de áudio:" },
            { "Device:", "Dispositivo:" },
            { "Output:", "Saída:" },
            { "Input:", "Entrada:" },
            { "Active output channels:", "Canais de saída ativos:" },
            { "Active input channels:", "Canais de entrada ativos:" },
            { "Sample rate:", "Taxa de amostragem:" },
            { "Audio buffer size:", "Tamanho do buffer de áudio:" },
            { "Active MIDI inputs:", "Entradas MIDI ativas:" },
            { "MIDI Output:", "Saída MIDI:" },
            { "Test", "Teste" },
            { "Plays a test tone", "Toca um som de teste" },
            { "Control Panel", "Painel de controle" },
            { "Opens the device's own control panel", "Abre o painel de configurações do dispositivo" },
            { "Reset Device", "Reiniciar" },
            { "Show advanced settings...", "Mostrar configurações avançadas..." },
            { "No MIDI inputs available", "Nenhuma entrada MIDI disponível" },
            { "(no audio input channels found)", "(nenhum canal de entrada de áudio encontrado)" },
            { "(no audio output channels found)", "(nenhum canal de saída de áudio encontrado)" },
            { "none", "nenhum" },
            { "Error when trying to open audio device!", "Erro ao abrir o dispositivo de áudio!" },
            { "Error whilst loading", "Erro ao carregar" },
            { "Error whilst saving", "Erro ao salvar" },
            { "Couldn't read from the specified file!", "Não foi possível ler o arquivo!" },
            { "Couldn't write to the specified file!", "Não foi possível gravar o arquivo!" },
            { "OK", "OK" },
            { "Unsaved changes", "Alterações não salvas" },
            { "Your changes to this sound will be lost.", "Suas alterações neste som serão perdidas." },
            { "Load anyway", "Carregar mesmo assim" },
        },
    };

    return translations;
}
