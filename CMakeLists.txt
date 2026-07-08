cmake_minimum_required(VERSION 3.22)

project(EFFIGY VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version")
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Universal binary")
endif()

# ---------------------------------------------------------------------------
# JUCE via FetchContent
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ---------------------------------------------------------------------------
# Embed factory IRs as BinaryData
# ---------------------------------------------------------------------------
file(GLOB EFFIGY_IR_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/IRs/*.wav")

juce_add_binary_data(EffigyResources
    SOURCES ${EFFIGY_IR_FILES}
)

# ---------------------------------------------------------------------------
# Plugin target
# ---------------------------------------------------------------------------
juce_add_plugin(EFFIGY
    COMPANY_NAME              "Effigy Audio"
    BUNDLE_ID                 com.effigyaudio.effigy
    PLUGIN_MANUFACTURER_CODE  Efgy
    PLUGIN_CODE               Efg1
    FORMATS                   VST3 AU
    PRODUCT_NAME              "EFFIGY"
    IS_SYNTH                  FALSE
    NEEDS_MIDI_INPUT          FALSE
    NEEDS_MIDI_OUTPUT         FALSE
    COPY_PLUGIN_AFTER_BUILD   FALSE
    VST3_CATEGORIES           "Fx" "Distortion"
    AU_MAIN_TYPE              kAudioUnitType_Effect
)

juce_generate_juce_header(EFFIGY)

target_sources(EFFIGY PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Parameters.cpp
    Source/Presets.cpp
    Source/dsp/InputConditioning.cpp
    Source/dsp/GateTightener.cpp
    Source/dsp/Drive.cpp
    Source/dsp/FeelEngine.cpp
    Source/dsp/ToneStack.cpp
    Source/dsp/CabSection.cpp
    Source/dsp/Produce.cpp
    Source/dsp/FactoryIRs.cpp
    Source/gui/LookAndFeel.cpp
)

target_compile_definitions(EFFIGY PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(EFFIGY PRIVATE
    EffigyResources
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_audio_processors
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
