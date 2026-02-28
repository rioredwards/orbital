#pragma once

#include "PluginProcessor.h"
#include <juce_audio_utils/juce_audio_utils.h>

class OrbitalEditor : public juce::AudioProcessorEditor,
                      private juce::Timer
{
public:
    explicit OrbitalEditor(OrbitalProcessor&);
    ~OrbitalEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    OrbitalProcessor& processorRef;

    juce::Label titleLabel;

    juce::Slider timeKnob     { juce::Slider::RotaryVerticalDrag,
                                juce::Slider::TextBoxBelow };
    juce::Slider feedbackKnob { juce::Slider::RotaryVerticalDrag,
                                juce::Slider::TextBoxBelow };
    juce::Slider mixKnob      { juce::Slider::RotaryVerticalDrag,
                                juce::Slider::TextBoxBelow };

    juce::Label timeLabel, feedbackLabel, mixLabel;

    // Attachments bind sliders to APVTS parameters bidirectionally.
    // Must be declared AFTER the sliders (initialization order matters).
    juce::AudioProcessorValueTreeState::SliderAttachment timeAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment feedbackAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment mixAttachment;

    // 1 channel (mono left-channel feed from processor)
    juce::AudioVisualiserComponent visualiser { 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrbitalEditor)
};
