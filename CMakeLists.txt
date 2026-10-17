cmake_minimum_required(VERSION 3.22)

project(BassAmp VERSION 0.1.0)

add_subdirectory(libs/JUCE)

juce_add_plugin(BassAmp
    COMPANY_NAME "Nicolas Samier"
    PRODUCT_NAME "Bass Amp"
    PLUGIN_MANUFACTURER_CODE Nsam
    PLUGIN_CODE Bamp
    FORMATS Standalone VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(BassAmp PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/ui/AmpLookAndFeel.cpp
    Source/ui/Section.cpp)

target_compile_features(BassAmp PRIVATE cxx_std_17)

target_compile_definitions(BassAmp PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(BassAmp
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
