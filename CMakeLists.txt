cmake_minimum_required(VERSION 3.22)

project(BassAmp VERSION 0.6.1)

# Static MSVC runtime: the app and plugin run without the Visual C++ redistributable.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(libs/JUCE)

set(PLUGIN_FORMATS Standalone VST3)

if(APPLE)
    list(APPEND PLUGIN_FORMATS AU)
endif()

juce_add_plugin(BassAmp
    COMPANY_NAME "Nicolas Samier"
    PRODUCT_NAME "Bass Amp"
    BUNDLE_ID com.nicolassamier.bassamp
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
    PLUGIN_MANUFACTURER_CODE Nsam
    PLUGIN_CODE Bamp
    FORMATS ${PLUGIN_FORMATS}
    AU_MAIN_TYPE kAudioUnitType_Effect
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "Bass Amp needs the audio input to hear your bass."
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

# Neural Amp Modeler core (MIT) with Eigen (MPL2), for .nam amp captures.
set(NAM_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/NeuralAmpModelerCore")
file(GLOB NAM_SOURCES "${NAM_DIR}/NAM/*.cpp" "${NAM_DIR}/NAM/*/*.cpp")
add_library(nam OBJECT ${NAM_SOURCES})
target_compile_features(nam PUBLIC cxx_std_17)
target_include_directories(nam SYSTEM PUBLIC "${NAM_DIR}" "${NAM_DIR}/Dependencies/eigen" "${NAM_DIR}/Dependencies/nlohmann")
target_compile_definitions(nam PUBLIC NAM_SAMPLE_FLOAT NAM_ENABLE_A2_FAST)
set_target_properties(nam PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    target_compile_definitions(nam PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(nam PRIVATE /W0 /bigobj)
else()
    target_compile_options(nam PRIVATE -w)
endif()

set(BASS_AMP_SOURCES
    Source/PedalChain.cpp
    Source/MidiLearn.cpp
    Source/Parameters.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/dsp/BackingTrack.cpp
    Source/dsp/NamModel.cpp
    Source/dsp/Recorder.cpp
    Source/dsp/Tuner.cpp
    Source/presets/FactoryPresets.cpp
    Source/presets/PresetManager.cpp
    Source/ui/AmpHead.cpp
    Source/ui/AmpLookAndFeel.cpp
    Source/ui/EqDisplay.cpp
    Source/ui/EqPanel.cpp
    Source/ui/LevelMeters.cpp
    Source/ui/Help.cpp
    Source/ui/Logo.cpp
    Source/ui/MainPanel.cpp
    Source/ui/Pedal.cpp
    Source/ui/PlayerUnit.cpp
    Source/ui/LooperUnit.cpp
    Source/ui/MetronomeUnit.cpp
    Source/ui/PedalBoard.cpp
    Source/ui/PresetBar.cpp
    Source/ui/RecorderBar.cpp
    Source/ui/TunerDisplay.cpp)

target_sources(BassAmp PRIVATE ${BASS_AMP_SOURCES})

target_sources(BassAmp_Standalone PRIVATE Source/StandaloneApp.cpp)
target_compile_definitions(BassAmp_Standalone PRIVATE JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1)

target_compile_features(BassAmp PRIVATE cxx_std_17)

# The help texts are French, in UTF-8.
if(MSVC)
    target_compile_options(BassAmp PRIVATE /utf-8)
endif()

target_compile_definitions(BassAmp PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_MP3AUDIOFORMAT=1
    JUCE_VST3_CAN_REPLACE_VST2=0)

# ASIO is a Windows driver model; macOS uses Core Audio.
if(WIN32)
    set(ASIO_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/asiosdk/common")

    if(EXISTS "${ASIO_SDK_DIR}/iasiodrv.h")
        target_include_directories(BassAmp PRIVATE "${ASIO_SDK_DIR}")
        target_compile_definitions(BassAmp PUBLIC JUCE_ASIO=1)
    else()
        message(WARNING "ASIO SDK not found in libs/asiosdk: building without ASIO support")
    endif()
endif()

target_link_libraries(BassAmp
    PRIVATE
        nam
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

juce_add_console_app(BassAmpTests)

target_sources(BassAmpTests PRIVATE Tests/DspTests.cpp ${BASS_AMP_SOURCES})

if(MSVC)
    target_compile_options(BassAmpTests PRIVATE /utf-8)
endif()

target_compile_definitions(BassAmpTests PRIVATE
    JucePlugin_Name="Bass Amp"
    JucePlugin_VersionString="${PROJECT_VERSION}"
    BASS_AMP_NAM_MODELS="${NAM_DIR}/example_models"
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_MP3AUDIOFORMAT=1)

target_link_libraries(BassAmpTests
    PRIVATE
        nam
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)
