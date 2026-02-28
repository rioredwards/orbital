#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>

class OrbitalProcessor;

// Trivial SynthesiserSound — accepts all notes and channels
class OrbitalSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

// Per-voice: 2 oscillators, 2 filters, ADSR envelope
class OrbitalVoice : public juce::SynthesiserVoice
{
public:
    explicit OrbitalVoice(juce::AudioProcessorValueTreeState& state);

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound*, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock);

private:
    float generateOscSample(int oscIndex, float phase);
    float advancePhase(float phase, float increment);

    juce::AudioProcessorValueTreeState& apvts;

    // Oscillator state
    float osc1Phase = 0.0f;
    float osc2Phase = 0.0f;
    float osc1PhaseInc = 0.0f;
    float osc2PhaseInc = 0.0f;

    // Filters (per-voice)
    juce::dsp::StateVariableTPTFilter<float> filter1;
    juce::dsp::StateVariableTPTFilter<float> filter2;

    // Envelope
    juce::ADSR envelope;

    float noteVelocity = 0.0f;
    double voiceSampleRate = 44100.0;
};

// Main processor with synthesiser, shared effects
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

    // Lock-free FIFO for passing audio to the visualizer
    static constexpr int fifoCapacity = 96000;
    juce::AbstractFifo abstractFifo { fifoCapacity };
    std::array<float, fifoCapacity> fifoData {};

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::Synthesiser synth;

    // Delay effect (shared, post-voice)
    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int delayBufferLength = 0;
    int delayWritePosition = 0;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrbitalProcessor)
};
