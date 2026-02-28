#include "PluginEditor.h"

OrbitalEditor::OrbitalEditor(OrbitalProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    helloLabel.setText("Hello World", juce::dontSendNotification);
    helloLabel.setFont(juce::FontOptions(32.0f));
    helloLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(helloLabel);

    setSize(400, 300);
}

OrbitalEditor::~OrbitalEditor() {}

void OrbitalEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void OrbitalEditor::resized()
{
    helloLabel.setBounds(getLocalBounds());
}
