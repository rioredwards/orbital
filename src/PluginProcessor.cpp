#include "PluginProcessor.h"
#include "PluginEditor.h"

OrbitalProcessor::OrbitalProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

OrbitalProcessor::~OrbitalProcessor() {}

const juce::String OrbitalProcessor::getName() const { return JucePlugin_Name; }
bool OrbitalProcessor::acceptsMidi() const { return false; }
bool OrbitalProcessor::producesMidi() const { return false; }
bool OrbitalProcessor::isMidiEffect() const { return false; }
double OrbitalProcessor::getTailLengthSeconds() const { return 0.0; }

int OrbitalProcessor::getNumPrograms() { return 1; }
int OrbitalProcessor::getCurrentProgram() { return 0; }
void OrbitalProcessor::setCurrentProgram(int) {}
const juce::String OrbitalProcessor::getProgramName(int) { return {}; }
void OrbitalProcessor::changeProgramName(int, const juce::String&) {}

void OrbitalProcessor::prepareToPlay(double, int) {}
void OrbitalProcessor::releaseResources() {}

void OrbitalProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* OrbitalProcessor::createEditor()
{
    return new OrbitalEditor(*this);
}

bool OrbitalProcessor::hasEditor() const { return true; }

void OrbitalProcessor::getStateInformation(juce::MemoryBlock&) {}
void OrbitalProcessor::setStateInformation(const void*, int) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrbitalProcessor();
}
