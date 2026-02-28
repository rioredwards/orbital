#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

static constexpr float twoPi = juce::MathConstants<float>::twoPi;

// ============================================================================
// Parameter Layout
// ============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
OrbitalProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Oscillator 1
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc1_waveform", "OSC1 Waveform",
        juce::StringArray { "SAW", "SQR", "SIN", "TRI" }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc1_level", "OSC1 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc1_octave", "OSC1 Octave",
        juce::StringArray { "-2", "-1", "0", "+1", "+2" }, 2));

    // Oscillator 2
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc2_waveform", "OSC2 Waveform",
        juce::StringArray { "SAW", "SQR", "SIN", "TRI" }, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc2_level", "OSC2 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc2_octave", "OSC2 Octave",
        juce::StringArray { "-2", "-1", "0", "+1", "+2" }, 2));

    // Filter 1
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter1_cutoff", "Filter 1 Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f), 8000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter1_resonance", "Filter 1 Resonance",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 0.707f));

    // Filter 2
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter2_cutoff", "Filter 2 Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filter2_resonance", "Filter 2 Resonance",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f), 0.707f));

    // Distortion
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "dist_drive", "Distortion Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "dist_mix", "Distortion Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // Envelope
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "env_attack", "Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.01f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "env_decay", "Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.4f), 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "env_sustain", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "env_release", "Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.4f), 0.5f));

    // Delay
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "delay_time", "Delay Time",
        juce::NormalisableRange<float>(1.0f, 2000.0f, 0.1f, 0.5f), 500.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "delay_feedback", "Delay Feedback",
        juce::NormalisableRange<float>(0.0f, 0.95f, 0.01f), 0.35f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "delay_mix", "Delay Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));

    // Master
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "master_gain", "Master Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    return layout;
}

// ============================================================================
// OrbitalVoice
// ============================================================================

OrbitalVoice::OrbitalVoice(juce::AudioProcessorValueTreeState& state)
    : apvts(state)
{
    filter1.setType(juce::dsp::StateVariableTPTFilter<float>::Type::lowpass);
    filter2.setType(juce::dsp::StateVariableTPTFilter<float>::Type::lowpass);
}

bool OrbitalVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<OrbitalSound*>(sound) != nullptr;
}

void OrbitalVoice::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    voiceSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = 1;
    spec.numChannels = 1;

    filter1.prepare(spec);
    filter2.prepare(spec);
    filter1.reset();
    filter2.reset();

    envelope.setSampleRate(sampleRate);
}

void OrbitalVoice::startNote(int midiNoteNumber, float velocity,
                              juce::SynthesiserSound*, int /*currentPitchWheelPosition*/)
{
    noteVelocity = velocity;

    const float freq = static_cast<float>(
        juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));

    // Octave offsets: choice 0=-2, 1=-1, 2=0, 3=+1, 4=+2
    const int oct1 = static_cast<int>(apvts.getRawParameterValue("osc1_octave")->load()) - 2;
    const int oct2 = static_cast<int>(apvts.getRawParameterValue("osc2_octave")->load()) - 2;

    const float freq1 = freq * std::pow(2.0f, static_cast<float>(oct1));
    const float freq2 = freq * std::pow(2.0f, static_cast<float>(oct2));

    osc1PhaseInc = freq1 / static_cast<float>(voiceSampleRate);
    osc2PhaseInc = freq2 / static_cast<float>(voiceSampleRate);

    osc1Phase = 0.0f;
    osc2Phase = 0.0f;

    // Set ADSR parameters
    juce::ADSR::Parameters adsrParams;
    adsrParams.attack  = apvts.getRawParameterValue("env_attack")->load();
    adsrParams.decay   = apvts.getRawParameterValue("env_decay")->load();
    adsrParams.sustain = apvts.getRawParameterValue("env_sustain")->load();
    adsrParams.release = apvts.getRawParameterValue("env_release")->load();
    envelope.setParameters(adsrParams);

    envelope.noteOn();

    filter1.reset();
    filter2.reset();
}

void OrbitalVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        envelope.noteOff();
    }
    else
    {
        envelope.reset();
        clearCurrentNote();
    }
}

void OrbitalVoice::pitchWheelMoved(int) {}
void OrbitalVoice::controllerMoved(int, int) {}

float OrbitalVoice::generateOscSample(int oscIndex, float phase)
{
    const int waveform = static_cast<int>(
        apvts.getRawParameterValue(oscIndex == 0 ? "osc1_waveform" : "osc2_waveform")->load());

    switch (waveform)
    {
        case 0: // SAW: -1 to +1 ramp
            return 2.0f * phase - 1.0f;
        case 1: // SQR
            return phase < 0.5f ? 1.0f : -1.0f;
        case 2: // SIN
            return std::sin(twoPi * phase);
        case 3: // TRI
            return 4.0f * std::abs(phase - 0.5f) - 1.0f;
        default:
            return 0.0f;
    }
}

float OrbitalVoice::advancePhase(float phase, float increment)
{
    phase += increment;
    if (phase >= 1.0f)
        phase -= 1.0f;
    return phase;
}

void OrbitalVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                    int startSample, int numSamples)
{
    if (!isVoiceActive())
        return;

    const float osc1Level = apvts.getRawParameterValue("osc1_level")->load();
    const float osc2Level = apvts.getRawParameterValue("osc2_level")->load();

    const float cutoff1 = apvts.getRawParameterValue("filter1_cutoff")->load();
    const float reso1   = apvts.getRawParameterValue("filter1_resonance")->load();
    const float cutoff2 = apvts.getRawParameterValue("filter2_cutoff")->load();
    const float reso2   = apvts.getRawParameterValue("filter2_resonance")->load();

    filter1.setCutoffFrequency(cutoff1);
    filter1.setResonance(reso1);
    filter2.setCutoffFrequency(cutoff2);
    filter2.setResonance(reso2);

    // Update ADSR in case parameters changed while note is held
    juce::ADSR::Parameters adsrParams;
    adsrParams.attack  = apvts.getRawParameterValue("env_attack")->load();
    adsrParams.decay   = apvts.getRawParameterValue("env_decay")->load();
    adsrParams.sustain = apvts.getRawParameterValue("env_sustain")->load();
    adsrParams.release = apvts.getRawParameterValue("env_release")->load();
    envelope.setParameters(adsrParams);

    for (int i = 0; i < numSamples; ++i)
    {
        // Generate oscillators
        float sample = generateOscSample(0, osc1Phase) * osc1Level
                     + generateOscSample(1, osc2Phase) * osc2Level;

        osc1Phase = advancePhase(osc1Phase, osc1PhaseInc);
        osc2Phase = advancePhase(osc2Phase, osc2PhaseInc);

        // Per-voice filters
        sample = filter1.processSample(0, sample);
        sample = filter2.processSample(0, sample);

        // ADSR envelope
        const float envValue = envelope.getNextSample();
        sample *= envValue * noteVelocity;

        // If envelope has finished, clear the voice
        if (!envelope.isActive())
        {
            clearCurrentNote();
            break;
        }

        // Add to all output channels
        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample(ch, startSample + i, sample);
    }
}

// ============================================================================
// OrbitalProcessor
// ============================================================================

OrbitalProcessor::OrbitalProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Add voices and sound to synthesiser
    for (int i = 0; i < 12; ++i)
        synth.addVoice(new OrbitalVoice(apvts));

    synth.addSound(new OrbitalSound());
}

OrbitalProcessor::~OrbitalProcessor() {}

const juce::String OrbitalProcessor::getName() const { return JucePlugin_Name; }
bool OrbitalProcessor::acceptsMidi() const { return true; }
bool OrbitalProcessor::producesMidi() const { return false; }
bool OrbitalProcessor::isMidiEffect() const { return false; }
double OrbitalProcessor::getTailLengthSeconds() const { return 2.0; }

int OrbitalProcessor::getNumPrograms() { return 1; }
int OrbitalProcessor::getCurrentProgram() { return 0; }
void OrbitalProcessor::setCurrentProgram(int) {}
const juce::String OrbitalProcessor::getProgramName(int) { return {}; }
void OrbitalProcessor::changeProgramName(int, const juce::String&) {}

void OrbitalProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    synth.setCurrentPlaybackSampleRate(sampleRate);

    // Prepare each voice
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<OrbitalVoice*>(synth.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);
    }

    // Delay buffers
    const int maxDelaySamples = static_cast<int>(2.0 * sampleRate) + 1;
    delayBufferL.assign(maxDelaySamples, 0.0f);
    delayBufferR.assign(maxDelaySamples, 0.0f);
    delayBufferLength = maxDelaySamples;
    delayWritePosition = 0;

    abstractFifo.reset();
    fifoData.fill(0.0f);
}

void OrbitalProcessor::releaseResources() {}

void OrbitalProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();

    // Clear buffer — synth generates audio, no input
    buffer.clear();

    // Render synth voices (osc → filter → ADSR → sum)
    synth.renderNextBlock(buffer, midiMessages, 0, numSamples);

    // Read effect parameters
    const float distDrive = apvts.getRawParameterValue("dist_drive")->load();
    const float distMix   = apvts.getRawParameterValue("dist_mix")->load();
    const float delayTimeMs = apvts.getRawParameterValue("delay_time")->load();
    const float delayFeedback = apvts.getRawParameterValue("delay_feedback")->load();
    const float delayMix  = apvts.getRawParameterValue("delay_mix")->load();
    const float masterGain = apvts.getRawParameterValue("master_gain")->load();

    auto* dataL = buffer.getWritePointer(0);
    auto* dataR = buffer.getWritePointer(buffer.getNumChannels() > 1 ? 1 : 0);

    const int delaySamples = static_cast<int>(delayTimeMs * currentSampleRate / 1000.0);

    // Distortion gain: drive 0→1x, drive 1→21x
    const float driveGain = 1.0f + distDrive * 20.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        // --- Distortion (tanh waveshaper) ---
        if (distDrive > 0.001f)
        {
            const float dryL = dataL[i];
            const float dryR = dataR[i];
            const float wetL = std::tanh(dryL * driveGain);
            const float wetR = std::tanh(dryR * driveGain);
            dataL[i] = dryL * (1.0f - distMix) + wetL * distMix;
            dataR[i] = dryR * (1.0f - distMix) + wetR * distMix;
        }

        // --- Delay ---
        const int readPos = (delayWritePosition - delaySamples + delayBufferLength)
                            % delayBufferLength;

        const float dryL = dataL[i];
        const float dryR = dataR[i];
        const float delayedL = delayBufferL[readPos];
        const float delayedR = delayBufferR[readPos];

        delayBufferL[delayWritePosition] = dryL + delayedL * delayFeedback;
        delayBufferR[delayWritePosition] = dryR + delayedR * delayFeedback;

        dataL[i] = dryL * (1.0f - delayMix) + delayedL * delayMix;
        dataR[i] = dryR * (1.0f - delayMix) + delayedR * delayMix;

        delayWritePosition = (delayWritePosition + 1) % delayBufferLength;

        // --- Master gain ---
        dataL[i] *= masterGain;
        dataR[i] *= masterGain;
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
