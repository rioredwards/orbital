cmake_minimum_required(VERSION 3.22)
project(Orbital VERSION 0.1.0)

add_subdirectory(JUCE)

juce_add_plugin(Orbital
    COMPANY_NAME "Orbital"
    PLUGIN_MANUFACTURER_CODE Orbt
    PLUGIN_CODE Orb1
    FORMATS VST3 Standalone AU
    PRODUCT_NAME "Orbital"
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(Orbital PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
)

target_compile_features(Orbital PRIVATE cxx_std_17)

target_link_libraries(Orbital
    PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_devices
        juce::juce_audio_utils
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(Orbital
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)
