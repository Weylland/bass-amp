cmake_minimum_required(VERSION 3.22)

project(BassAmp VERSION 0.4.0)

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
    PLUGIN_MANUFACTURER_CODE Nsam
    PLUGIN_CODE Bamp
    FORMATS ${PLUGIN_FORMATS}
    AU_MAIN_TYPE kAudioUnitType_Effect
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "Bass Amp needs the audio input to hear your bass."
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(BassAmp PRIVATE
    Source/Parameters.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/dsp/Recorder.cpp
    Source/dsp/Tuner.cpp
    Source/presets/FactoryPresets.cpp
    Source/presets/PresetManager.cpp
    Source/ui/AmpLookAndFeel.cpp
    Source/ui/LevelMeters.cpp
    Source/ui/PresetBar.cpp
    Source/ui/RecorderBar.cpp
    Source/ui/Section.cpp
    Source/ui/TunerDisplay.cpp)

target_sources(BassAmp_Standalone PRIVATE Source/StandaloneApp.cpp)
target_compile_definitions(BassAmp_Standalone PRIVATE JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1)

target_compile_features(BassAmp PRIVATE cxx_std_17)

target_compile_definitions(BassAmp PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
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
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

juce_add_console_app(BassAmpTests)

target_sources(BassAmpTests PRIVATE
    Tests/DspTests.cpp
    Source/Parameters.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/dsp/Recorder.cpp
    Source/dsp/Tuner.cpp
    Source/presets/FactoryPresets.cpp
    Source/presets/PresetManager.cpp
    Source/ui/AmpLookAndFeel.cpp
    Source/ui/LevelMeters.cpp
    Source/ui/PresetBar.cpp
    Source/ui/RecorderBar.cpp
    Source/ui/Section.cpp
    Source/ui/TunerDisplay.cpp)

target_compile_definitions(BassAmpTests PRIVATE
    JucePlugin_Name="Bass Amp"
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0)

target_link_libraries(BassAmpTests
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)
