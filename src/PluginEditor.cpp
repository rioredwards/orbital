#include "PluginEditor.h"

static const juce::Colour bgColour       (0xff1a1a2e);
static const juce::Colour accentColour   (0xff7b68ee);
static const juce::Colour labelColour    (0xffaaaaaa);
static const juce::Colour sectionColour  (0xff7b68ee);

OrbitalEditor::OrbitalEditor(OrbitalProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      // APVTS attachments — must match declaration order in header
      osc1WaveAttach    (p.apvts, "osc1_waveform",  osc1WaveBox),
      osc1LevelAttach   (p.apvts, "osc1_level",     osc1LevelKnob),
      osc1OctaveAttach  (p.apvts, "osc1_octave",    osc1OctaveBox),
      osc2WaveAttach    (p.apvts, "osc2_waveform",  osc2WaveBox),
      osc2LevelAttach   (p.apvts, "osc2_level",     osc2LevelKnob),
      osc2OctaveAttach  (p.apvts, "osc2_octave",    osc2OctaveBox),
      filter1CutoffAttach (p.apvts, "filter1_cutoff",    filter1CutoffKnob),
      filter1ResoAttach   (p.apvts, "filter1_resonance", filter1ResoKnob),
      filter2CutoffAttach (p.apvts, "filter2_cutoff",    filter2CutoffKnob),
      filter2ResoAttach   (p.apvts, "filter2_resonance", filter2ResoKnob),
      distDriveAttach   (p.apvts, "dist_drive", distDriveKnob),
      distMixAttach     (p.apvts, "dist_mix",   distMixKnob),
      attackAttach      (p.apvts, "env_attack",  attackKnob),
      decayAttach       (p.apvts, "env_decay",   decayKnob),
      sustainAttach     (p.apvts, "env_sustain", sustainKnob),
      releaseAttach     (p.apvts, "env_release", releaseKnob),
      delayTimeAttach     (p.apvts, "delay_time",     delayTimeKnob),
      delayFeedbackAttach (p.apvts, "delay_feedback", delayFeedbackKnob),
      delayMixAttach      (p.apvts, "delay_mix",      delayMixKnob),
      masterGainAttach  (p.apvts, "master_gain", masterGainKnob)
{
    setSize(800, 620);

    // Title
    titleLabel.setText("ORBITAL", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(36.0f));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    // --- Helpers ---
    auto setupKnob = [&](juce::Slider& knob, juce::Label& label, const juce::String& text)
    {
        knob.setColour(juce::Slider::rotarySliderFillColourId, accentColour);
        knob.setColour(juce::Slider::thumbColourId, accentColour);
        knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        addAndMakeVisible(knob);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(11.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, labelColour);
        addAndMakeVisible(label);
    };

    auto setupCombo = [&](juce::ComboBox& box, juce::Label& label, const juce::String& text,
                          const juce::StringArray& items)
    {
        box.addItemList(items, 1);
        box.setColour(juce::ComboBox::backgroundColourId, bgColour.brighter(0.15f));
        box.setColour(juce::ComboBox::textColourId, juce::Colours::white);
        box.setColour(juce::ComboBox::outlineColourId, accentColour.withAlpha(0.4f));
        addAndMakeVisible(box);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(11.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, labelColour);
        addAndMakeVisible(label);
    };

    auto setupSection = [&](juce::Label& label, const juce::String& text)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(14.0f));
        label.setJustificationType(juce::Justification::centredLeft);
        label.setColour(juce::Label::textColourId, sectionColour);
        addAndMakeVisible(label);
    };

    // --- OSC 1 ---
    setupSection(osc1SectionLabel, "OSC 1");
    setupCombo(osc1WaveBox, osc1WaveLabel, "WAVE", { "SAW", "SQR", "SIN", "TRI" });
    setupKnob(osc1LevelKnob, osc1LevelLabel, "LEVEL");
    setupCombo(osc1OctaveBox, osc1OctaveLabel, "OCT", { "-2", "-1", "0", "+1", "+2" });

    // --- OSC 2 ---
    setupSection(osc2SectionLabel, "OSC 2");
    setupCombo(osc2WaveBox, osc2WaveLabel, "WAVE", { "SAW", "SQR", "SIN", "TRI" });
    setupKnob(osc2LevelKnob, osc2LevelLabel, "LEVEL");
    setupCombo(osc2OctaveBox, osc2OctaveLabel, "OCT", { "-2", "-1", "0", "+1", "+2" });

    // --- Filter 1 ---
    setupSection(filter1SectionLabel, "FILTER 1");
    setupKnob(filter1CutoffKnob, filter1CutoffLabel, "CUTOFF");
    setupKnob(filter1ResoKnob, filter1ResoLabel, "RES");

    // --- Filter 2 ---
    setupSection(filter2SectionLabel, "FILTER 2");
    setupKnob(filter2CutoffKnob, filter2CutoffLabel, "CUTOFF");
    setupKnob(filter2ResoKnob, filter2ResoLabel, "RES");

    // --- Distortion ---
    setupSection(distSectionLabel, "DISTORTION");
    setupKnob(distDriveKnob, distDriveLabel, "DRIVE");
    setupKnob(distMixKnob, distMixLabel, "MIX");

    // --- Envelope ---
    setupSection(envSectionLabel, "ENVELOPE");
    setupKnob(attackKnob, attackLabel, "A");
    setupKnob(decayKnob, decayLabel, "D");
    setupKnob(sustainKnob, sustainLabel, "S");
    setupKnob(releaseKnob, releaseLabel, "R");

    // --- Delay ---
    setupSection(delaySectionLabel, "DELAY");
    setupKnob(delayTimeKnob, delayTimeLabel, "TIME");
    setupKnob(delayFeedbackKnob, delayFeedbackLabel, "FB");
    setupKnob(delayMixKnob, delayMixLabel, "MIX");

    // --- Master ---
    setupSection(masterSectionLabel, "MASTER");
    setupKnob(masterGainKnob, masterGainLabel, "GAIN");

    // Visualizer
    visualiser.setBufferSize(512);
    visualiser.setSamplesPerBlock(256);
    visualiser.setColours(juce::Colour(0xff0d0d1a), accentColour);
    addAndMakeVisible(visualiser);

    startTimerHz(30);
}

OrbitalEditor::~OrbitalEditor() {}

void OrbitalEditor::paint(juce::Graphics& g)
{
    g.fillAll(bgColour);

    const float lineAlpha = 0.35f;
    g.setColour(accentColour.withAlpha(lineAlpha));

    // Separator under title
    g.drawHorizontalLine(60, 20.0f, static_cast<float>(getWidth() - 20));
    // Separator under OSC row
    g.drawHorizontalLine(190, 20.0f, static_cast<float>(getWidth() - 20));
    // Separator under Filter/Distortion row
    g.drawHorizontalLine(316, 20.0f, static_cast<float>(getWidth() - 20));
    // Separator under Envelope/Delay/Master row
    g.drawHorizontalLine(442, 20.0f, static_cast<float>(getWidth() - 20));
}

void OrbitalEditor::resized()
{
    const int w = getWidth();

    titleLabel.setBounds(0, 8, w, 50);

    // =========================================================================
    // ROW 1: OSC 1 (left half) | OSC 2 (right half)   y: 66–186
    // =========================================================================
    const int oscY = 66;
    const int halfW = w / 2;
    const int knobSize = 70;
    const int labelH = 16;
    const int comboW = 70;
    const int comboH = 24;

    auto layoutOsc = [&](int xOffset, juce::Label& sectionLbl,
                         juce::ComboBox& waveBox, juce::Label& waveLbl,
                         juce::Slider& levelKnob, juce::Label& levelLbl,
                         juce::ComboBox& octBox, juce::Label& octLbl)
    {
        sectionLbl.setBounds(xOffset + 10, oscY, 80, 20);

        const int itemY = oscY + 24;
        const int spacing = (halfW - 20) / 3;

        // Wave combo
        int cx = xOffset + 10 + spacing / 2;
        waveLbl.setBounds(cx - comboW / 2, itemY, comboW, labelH);
        waveBox.setBounds(cx - comboW / 2, itemY + labelH + 2, comboW, comboH);

        // Level knob
        cx = xOffset + 10 + spacing + spacing / 2;
        levelLbl.setBounds(cx - 35, itemY, 70, labelH);
        levelKnob.setBounds(cx - knobSize / 2, itemY + labelH + 2, knobSize, knobSize);

        // Octave combo
        cx = xOffset + 10 + spacing * 2 + spacing / 2;
        octLbl.setBounds(cx - comboW / 2, itemY, comboW, labelH);
        octBox.setBounds(cx - comboW / 2, itemY + labelH + 2, comboW, comboH);
    };

    layoutOsc(0, osc1SectionLabel, osc1WaveBox, osc1WaveLabel,
              osc1LevelKnob, osc1LevelLabel, osc1OctaveBox, osc1OctaveLabel);
    layoutOsc(halfW, osc2SectionLabel, osc2WaveBox, osc2WaveLabel,
              osc2LevelKnob, osc2LevelLabel, osc2OctaveBox, osc2OctaveLabel);

    // =========================================================================
    // ROW 2: Filter 1 | Filter 2 | Distortion   y: 196–312
    // =========================================================================
    const int row2Y = 196;
    const int thirdW = w / 3;

    auto layoutFilterSection = [&](int xOffset, int sectionW, juce::Label& sectionLbl,
                                   juce::Slider& knob1, juce::Label& lbl1,
                                   juce::Slider& knob2, juce::Label& lbl2)
    {
        sectionLbl.setBounds(xOffset + 10, row2Y, sectionW - 20, 20);
        const int itemY = row2Y + 24;
        const int spacing = sectionW / 2;

        int cx = xOffset + spacing / 2;
        lbl1.setBounds(cx - 35, itemY, 70, labelH);
        knob1.setBounds(cx - knobSize / 2, itemY + labelH + 2, knobSize, knobSize);

        cx = xOffset + spacing + spacing / 2;
        lbl2.setBounds(cx - 35, itemY, 70, labelH);
        knob2.setBounds(cx - knobSize / 2, itemY + labelH + 2, knobSize, knobSize);
    };

    layoutFilterSection(0, thirdW, filter1SectionLabel,
                        filter1CutoffKnob, filter1CutoffLabel,
                        filter1ResoKnob, filter1ResoLabel);
    layoutFilterSection(thirdW, thirdW, filter2SectionLabel,
                        filter2CutoffKnob, filter2CutoffLabel,
                        filter2ResoKnob, filter2ResoLabel);
    layoutFilterSection(thirdW * 2, w - thirdW * 2, distSectionLabel,
                        distDriveKnob, distDriveLabel,
                        distMixKnob, distMixLabel);

    // =========================================================================
    // ROW 3: Envelope (4 knobs) | Delay (3 knobs) | Master (1 knob)  y: 322–438
    // =========================================================================
    const int row3Y = 322;

    // Envelope — takes ~half width (4 knobs)
    const int envW = w * 4 / 10;
    envSectionLabel.setBounds(10, row3Y, envW - 20, 20);
    {
        const int itemY = row3Y + 24;
        const int spacing = envW / 4;
        juce::Slider* knobs[] = { &attackKnob, &decayKnob, &sustainKnob, &releaseKnob };
        juce::Label*  labels[] = { &attackLabel, &decayLabel, &sustainLabel, &releaseLabel };
        for (int i = 0; i < 4; ++i)
        {
            int cx = spacing / 2 + spacing * i;
            labels[i]->setBounds(cx - 35, itemY, 70, labelH);
            knobs[i]->setBounds(cx - knobSize / 2, itemY + labelH + 2, knobSize, knobSize);
        }
    }

    // Delay — 3 knobs
    const int delayX = envW;
    const int delayW = w * 4 / 10;
    delaySectionLabel.setBounds(delayX + 10, row3Y, delayW - 20, 20);
    {
        const int itemY = row3Y + 24;
        const int spacing = delayW / 3;
        juce::Slider* knobs[] = { &delayTimeKnob, &delayFeedbackKnob, &delayMixKnob };
        juce::Label*  labels[] = { &delayTimeLabel, &delayFeedbackLabel, &delayMixLabel };
        for (int i = 0; i < 3; ++i)
        {
            int cx = delayX + spacing / 2 + spacing * i;
            labels[i]->setBounds(cx - 35, itemY, 70, labelH);
            knobs[i]->setBounds(cx - knobSize / 2, itemY + labelH + 2, knobSize, knobSize);
        }
    }

    // Master — 1 knob
    const int masterX = envW + delayW;
    const int masterW = w - masterX;
    masterSectionLabel.setBounds(masterX + 10, row3Y, masterW - 20, 20);
    {
        const int itemY = row3Y + 24;
        int cx = masterX + masterW / 2;
        masterGainLabel.setBounds(cx - 35, itemY, 70, labelH);
        masterGainKnob.setBounds(cx - knobSize / 2, itemY + labelH + 2, knobSize, knobSize);
    }

    // =========================================================================
    // Visualizer   y: 452–604
    // =========================================================================
    visualiser.setBounds(20, 452, w - 40, 152);
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
