#pragma once

#include "PluginProcessor.h"

class OrbitalEditor : public juce::AudioProcessorEditor
{
public:
    explicit OrbitalEditor(OrbitalProcessor&);
    ~OrbitalEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OrbitalProcessor& processorRef;
    juce::Label helloLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrbitalEditor)
};
