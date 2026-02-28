#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

class OrbitalProcessor : public juce::AudioProcessor
{
public:
    OrbitalProcessor();
    ~OrbitalProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // Lock-free FIFO for passing audio to the visualizer on the message thread
    static constexpr int fifoCapacity = 96000;
    juce::AbstractFifo       abstractFifo { fifoCapacity };
    std::array<float, fifoCapacity> fifoData {};

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int   delayBufferLength  = 0;
    int   delayWritePosition = 0;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrbitalProcessor)
};
