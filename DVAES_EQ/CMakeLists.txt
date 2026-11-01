cmake_minimum_required(VERSION 3.22)
project(DVAES_EQ VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.10 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(DVAES_EQ
    COMPANY_NAME "DVAES Digital"
    BUNDLE_ID "digital.dvaes.eq"
    PLUGIN_MANUFACTURER_CODE Dvae
    PLUGIN_CODE DvEq
    FORMATS VST3
    PRODUCT_NAME "DVAES EQ"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)
juce_generate_juce_header(DVAES_EQ)
target_sources(DVAES_EQ PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(DVAES_EQ PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(DVAES_EQ PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
