#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout
OrbitalProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "time", "Delay Time",
        juce::NormalisableRange<float>(1.0f, 2000.0f, 0.1f, 0.5f),
        500.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "feedback", "Feedback",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f),
        0.35f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Effect Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));

    return layout;
}

OrbitalProcessor::OrbitalProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

OrbitalProcessor::~OrbitalProcessor() {}

const juce::String OrbitalProcessor::getName() const { return JucePlugin_Name; }
bool OrbitalProcessor::acceptsMidi() const { return false; }
bool OrbitalProcessor::producesMidi() const { return false; }
bool OrbitalProcessor::isMidiEffect() const { return false; }
double OrbitalProcessor::getTailLengthSeconds() const { return 2.0; }

int OrbitalProcessor::getNumPrograms() { return 1; }
int OrbitalProcessor::getCurrentProgram() { return 0; }
void OrbitalProcessor::setCurrentProgram(int) {}
const juce::String OrbitalProcessor::getProgramName(int) { return {}; }
void OrbitalProcessor::changeProgramName(int, const juce::String&) {}

void OrbitalProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    currentSampleRate = sampleRate;

    const int maxDelaySamples = static_cast<int>(2.0 * sampleRate) + 1;

    delayBufferL.assign(maxDelaySamples, 0.0f);
    delayBufferR.assign(maxDelaySamples, 0.0f);
    delayBufferLength  = maxDelaySamples;
    delayWritePosition = 0;

    abstractFifo.reset();
    fifoData.fill(0.0f);
}

void OrbitalProcessor::releaseResources() {}

void OrbitalProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const float delayTimeMs = apvts.getRawParameterValue("time")->load();
    const float feedback    = apvts.getRawParameterValue("feedback")->load();
    const float mix         = apvts.getRawParameterValue("mix")->load();

    const int delaySamples = static_cast<int>(delayTimeMs * currentSampleRate / 1000.0);
    const int numSamples   = buffer.getNumSamples();

    auto* dataL = buffer.getWritePointer(0);
    auto* dataR = buffer.getWritePointer(buffer.getNumChannels() > 1 ? 1 : 0);

    for (int i = 0; i < numSamples; ++i)
    {
        const int readPos = (delayWritePosition - delaySamples + delayBufferLength)
                            % delayBufferLength;

        const float dryL = dataL[i];
        const float dryR = dataR[i];
        const float delayedL = delayBufferL[readPos];
        const float delayedR = delayBufferR[readPos];

        delayBufferL[delayWritePosition] = dryL + delayedL * feedback;
        delayBufferR[delayWritePosition] = dryR + delayedR * feedback;

        dataL[i] = dryL * (1.0f - mix) + delayedL * mix;
        dataR[i] = dryR * (1.0f - mix) + delayedR * mix;

        delayWritePosition = (delayWritePosition + 1) % delayBufferLength;
    }

    // Push left-channel output to the lock-free FIFO for the visualizer
    int start1, size1, start2, size2;
    abstractFifo.prepareToWrite(numSamples, start1, size1, start2, size2);
    if (size1 > 0) std::copy_n(dataL,         size1, fifoData.data() + start1);
    if (size2 > 0) std::copy_n(dataL + size1, size2, fifoData.data() + start2);
    abstractFifo.finishedWrite(size1 + size2);
}

juce::AudioProcessorEditor* OrbitalProcessor::createEditor()
{
    return new OrbitalEditor(*this);
}

bool OrbitalProcessor::hasEditor() const { return true; }

void OrbitalProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OrbitalProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrbitalProcessor();
}
