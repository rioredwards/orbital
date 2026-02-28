#pragma once

#include "PluginProcessor.h"
#include <juce_audio_utils/juce_audio_utils.h>

//==============================================================================
class SynthwaveLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SynthwaveLookAndFeel();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override;

    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox&) override;

    void drawPopupMenuItem(juce::Graphics&, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;
};

//==============================================================================
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
    SynthwaveLookAndFeel synthwaveLnF;

    // Title
    juce::Label titleLabel;

    // --- OSC 1 ---
    juce::ComboBox osc1WaveBox;
    juce::Slider   osc1LevelKnob  { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::ComboBox osc1OctaveBox;
    juce::Label    osc1WaveLabel, osc1LevelLabel, osc1OctaveLabel;

    // --- OSC 2 ---
    juce::ComboBox osc2WaveBox;
    juce::Slider   osc2LevelKnob  { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::ComboBox osc2OctaveBox;
    juce::Label    osc2WaveLabel, osc2LevelLabel, osc2OctaveLabel;

    // --- Filter 1 ---
    juce::Slider filter1CutoffKnob { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider filter1ResoKnob   { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label  filter1CutoffLabel, filter1ResoLabel;

    // --- Filter 2 ---
    juce::Slider filter2CutoffKnob { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider filter2ResoKnob   { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label  filter2CutoffLabel, filter2ResoLabel;

    // --- Distortion ---
    juce::Slider distDriveKnob { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider distMixKnob   { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label  distDriveLabel, distMixLabel;

    // --- Envelope ---
    juce::Slider attackKnob  { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider decayKnob   { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider sustainKnob { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider releaseKnob { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label  attackLabel, decayLabel, sustainLabel, releaseLabel;

    // --- Delay ---
    juce::Slider delayTimeKnob     { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider delayFeedbackKnob { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Slider delayMixKnob      { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label  delayTimeLabel, delayFeedbackLabel, delayMixLabel;

    // --- Master ---
    juce::Slider masterGainKnob { juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow };
    juce::Label  masterGainLabel;

    // Section labels
    juce::Label osc1SectionLabel, osc2SectionLabel;
    juce::Label filter1SectionLabel, filter2SectionLabel, distSectionLabel;
    juce::Label envSectionLabel, delaySectionLabel, masterSectionLabel;

    // --- APVTS Attachments (declared AFTER controls) ---
    juce::AudioProcessorValueTreeState::ComboBoxAttachment osc1WaveAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment   osc1LevelAttach;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment osc1OctaveAttach;

    juce::AudioProcessorValueTreeState::ComboBoxAttachment osc2WaveAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment   osc2LevelAttach;
    juce::AudioProcessorValueTreeState::ComboBoxAttachment osc2OctaveAttach;

    juce::AudioProcessorValueTreeState::SliderAttachment filter1CutoffAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment filter1ResoAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment filter2CutoffAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment filter2ResoAttach;

    juce::AudioProcessorValueTreeState::SliderAttachment distDriveAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment distMixAttach;

    juce::AudioProcessorValueTreeState::SliderAttachment attackAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment decayAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment sustainAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment releaseAttach;

    juce::AudioProcessorValueTreeState::SliderAttachment delayTimeAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment delayFeedbackAttach;
    juce::AudioProcessorValueTreeState::SliderAttachment delayMixAttach;

    juce::AudioProcessorValueTreeState::SliderAttachment masterGainAttach;

    // Visualizer
    juce::AudioVisualiserComponent visualiser { 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrbitalEditor)
};
