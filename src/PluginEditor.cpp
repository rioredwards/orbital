#include "PluginEditor.h"

OrbitalEditor::OrbitalEditor(OrbitalProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      timeAttachment     (p.apvts, "time",     timeKnob),
      feedbackAttachment (p.apvts, "feedback", feedbackKnob),
      mixAttachment      (p.apvts, "mix",      mixKnob)
{
    setSize(500, 380);

    // Title
    titleLabel.setText("ORBITAL", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(36.0f));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    // Helper: configure one knob + its label
    auto setupKnob = [&](juce::Slider& knob, juce::Label& label,
                         const juce::String& text)
    {
        knob.setColour(juce::Slider::rotarySliderFillColourId,
                       juce::Colour(0xff7b68ee));
        knob.setColour(juce::Slider::thumbColourId,
                       juce::Colour(0xff7b68ee));
        addAndMakeVisible(knob);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(13.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colour(0xffaaaaaa));
        addAndMakeVisible(label);
    };

    setupKnob(timeKnob,     timeLabel,     "TIME");
    setupKnob(feedbackKnob, feedbackLabel, "FEEDBACK");
    setupKnob(mixKnob,      mixLabel,      "MIX");

    // Visualizer
    visualiser.setBufferSize(512);
    visualiser.setSamplesPerBlock(256);
    visualiser.setColours(juce::Colour(0xff0d0d1a), juce::Colour(0xff7b68ee));
    addAndMakeVisible(visualiser);

    // 30 fps timer to drain the audio FIFO
    startTimerHz(30);
}

OrbitalEditor::~OrbitalEditor() {}

void OrbitalEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));

    // Accent separator line under title
    g.setColour(juce::Colour(0xff7b68ee).withAlpha(0.35f));
    g.drawHorizontalLine(65, 24.0f, static_cast<float>(getWidth() - 24));
}

void OrbitalEditor::resized()
{
    const int w = getWidth();

    titleLabel.setBounds(0, 0, w, 68);

    // Divide horizontally into 3 equal columns for the knobs
    const int colW    = w / 3;
    const int labelH  = 20;
    const int knobH   = 160;
    const int topPad  = 20;
    const int labelY  = 68 + topPad;
    const int knobY   = labelY + labelH + 4;

    auto placeKnob = [&](juce::Slider& knob, juce::Label& label, int col)
    {
        const int cx = col * colW + colW / 2;
        label.setBounds(cx - 60, labelY, 120, labelH);
        knob .setBounds(cx - 75, knobY, 150, knobH);
    };

    placeKnob(timeKnob,     timeLabel,     0);
    placeKnob(feedbackKnob, feedbackLabel, 1);
    placeKnob(mixKnob,      mixLabel,      2);

    visualiser.setBounds(20, 295, w - 40, 70);
}

void OrbitalEditor::timerCallback()
{
    const int available = processorRef.abstractFifo.getNumReady();
    if (available <= 0)
        return;

    int start1, size1, start2, size2;
    processorRef.abstractFifo.prepareToRead(available, start1, size1, start2, size2);

    const float* ptr1 = processorRef.fifoData.data() + start1;
    const float* ptr2 = processorRef.fifoData.data() + start2;

    if (size1 > 0) visualiser.pushBuffer(&ptr1, 1, size1);
    if (size2 > 0) visualiser.pushBuffer(&ptr2, 1, size2);

    processorRef.abstractFifo.finishedRead(size1 + size2);
}
