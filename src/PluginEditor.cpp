#include "PluginEditor.h"

// --- Synthwave colour palette ---
static const juce::Colour deepSpaceTop    (0xff0a0a1a);
static const juce::Colour deepPurpleBot   (0xff1a0a2e);
static const juce::Colour neonPink        (0xffff2d95);
static const juce::Colour neonCyan        (0xff00e5ff);
static const juce::Colour hotPurple       (0xffb24dff);
static const juce::Colour chromeSilver    (0xffc0c0d0);
static const juce::Colour dimLabel        (0xff8888aa);
static const juce::Colour vizBg           (0xff05050f);

//==============================================================================
// SynthwaveLookAndFeel
//==============================================================================

SynthwaveLookAndFeel::SynthwaveLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, chromeSilver);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff0f0f1f));
    setColour(juce::PopupMenu::textColourId, chromeSilver);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, neonCyan.withAlpha(0.2f));
    setColour(juce::PopupMenu::highlightedTextColourId, neonCyan);
}

void SynthwaveLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                            juce::Slider&)
{
    const float radius  = (float)juce::jmin(width, height) * 0.4f;
    const float centreX = (float)x + (float)width  * 0.5f;
    const float centreY = (float)y + (float)height * 0.5f;
    const float angle   = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // --- Knob body: dark circle with subtle radial gradient ---
    {
        juce::ColourGradient bodyGrad(juce::Colour(0xff2a2a3a), centreX, centreY - radius * 0.3f,
                                       juce::Colour(0xff111118), centreX, centreY + radius, true);
        g.setGradientFill(bodyGrad);
        g.fillEllipse(centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);
    }

    // --- Neon pink arc (glow layer) ---
    const float arcRadius = radius + 3.0f;
    const float valueAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    {
        juce::Path glowArc;
        glowArc.addCentredArc(centreX, centreY, arcRadius, arcRadius,
                              0.0f, rotaryStartAngle, valueAngle, true);
        g.setColour(neonPink.withAlpha(0.25f));
        g.strokePath(glowArc, juce::PathStrokeType(6.0f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
    }

    // --- Neon pink arc (crisp layer) ---
    {
        juce::Path arc;
        arc.addCentredArc(centreX, centreY, arcRadius, arcRadius,
                          0.0f, rotaryStartAngle, valueAngle, true);
        g.setColour(neonPink);
        g.strokePath(arc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));
    }

    // --- Bright dot at current position ---
    {
        const float dotRadius = 4.0f;
        const float dotX = centreX + arcRadius * std::cos(angle - juce::MathConstants<float>::halfPi);
        const float dotY = centreY + arcRadius * std::sin(angle - juce::MathConstants<float>::halfPi);
        g.setColour(juce::Colours::white);
        g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f);
        g.setColour(neonPink.withAlpha(0.5f));
        g.fillEllipse(dotX - dotRadius * 2.0f, dotY - dotRadius * 2.0f, dotRadius * 4.0f, dotRadius * 4.0f);
    }
}

void SynthwaveLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                        int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                        juce::ComboBox&)
{
    // Dark fill with rounded corners
    g.setColour(juce::Colour(0xff0f0f1f));
    g.fillRoundedRectangle(0.0f, 0.0f, (float)width, (float)height, 4.0f);

    // Neon cyan border
    g.setColour(neonCyan.withAlpha(0.6f));
    g.drawRoundedRectangle(0.5f, 0.5f, (float)width - 1.0f, (float)height - 1.0f, 4.0f, 1.0f);

    // Small cyan triangle arrow on right side
    const float arrowX = (float)width - 16.0f;
    const float arrowY = (float)height * 0.5f - 3.0f;
    juce::Path arrow;
    arrow.addTriangle(arrowX, arrowY, arrowX + 8.0f, arrowY, arrowX + 4.0f, arrowY + 6.0f);
    g.setColour(neonCyan);
    g.fillPath(arrow);
}

void SynthwaveLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                             bool isSeparator, bool /*isActive*/, bool isHighlighted,
                                             bool /*isTicked*/, bool /*hasSubMenu*/,
                                             const juce::String& text, const juce::String& /*shortcutKeyText*/,
                                             const juce::Drawable* /*icon*/, const juce::Colour* /*textColour*/)
{
    if (isSeparator)
    {
        g.setColour(hotPurple.withAlpha(0.3f));
        g.fillRect(area.reduced(5, 0).withHeight(1));
        return;
    }

    if (isHighlighted)
    {
        g.setColour(neonCyan.withAlpha(0.15f));
        g.fillRect(area);
        g.setColour(neonCyan);
    }
    else
    {
        g.setColour(chromeSilver);
    }

    g.setFont(juce::FontOptions(14.0f));
    g.drawText(text, area.reduced(10, 0), juce::Justification::centredLeft);
}

//==============================================================================
// OrbitalEditor
//==============================================================================

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
    setLookAndFeel(&synthwaveLnF);
    setSize(800, 620);

    // Title
    titleLabel.setText("ORBITAL", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(36.0f));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, neonCyan);
    addAndMakeVisible(titleLabel);

    // --- Helpers ---
    auto setupKnob = [&](juce::Slider& knob, juce::Label& label, const juce::String& text)
    {
        knob.setColour(juce::Slider::rotarySliderFillColourId, neonPink);
        knob.setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
        knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        addAndMakeVisible(knob);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(11.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, dimLabel);
        addAndMakeVisible(label);
    };

    auto setupCombo = [&](juce::ComboBox& box, juce::Label& label, const juce::String& text,
                          const juce::StringArray& items)
    {
        box.addItemList(items, 1);
        box.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0f0f1f));
        box.setColour(juce::ComboBox::textColourId, chromeSilver);
        box.setColour(juce::ComboBox::outlineColourId, neonCyan.withAlpha(0.6f));
        addAndMakeVisible(box);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(11.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, dimLabel);
        addAndMakeVisible(label);
    };

    auto setupSection = [&](juce::Label& label, const juce::String& text)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(14.0f));
        label.setJustificationType(juce::Justification::centredLeft);
        label.setColour(juce::Label::textColourId, neonCyan);
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

    // Visualizer — neon pink on near-black
    visualiser.setBufferSize(512);
    visualiser.setSamplesPerBlock(256);
    visualiser.setColours(vizBg, neonPink);
    addAndMakeVisible(visualiser);

    startTimerHz(30);
}

OrbitalEditor::~OrbitalEditor()
{
    setLookAndFeel(nullptr);
}

void OrbitalEditor::paint(juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();

    // --- 1) Background gradient: deep space top → deep purple bottom ---
    {
        juce::ColourGradient bgGrad(deepSpaceTop, 0.0f, 0.0f,
                                     deepPurpleBot, 0.0f, (float)h, false);
        g.setGradientFill(bgGrad);
        g.fillRect(getLocalBounds());
    }

    // --- 2) Starfield: ~40 deterministic tiny white dots ---
    {
        juce::Random starRng(42); // fixed seed — stars don't move on repaint
        for (int i = 0; i < 40; ++i)
        {
            const float sx = starRng.nextFloat() * (float)w;
            const float sy = starRng.nextFloat() * (float)h;
            const float sa = 0.15f + starRng.nextFloat() * 0.55f;
            const float sr = 0.5f + starRng.nextFloat() * 1.0f;
            g.setColour(juce::Colours::white.withAlpha(sa));
            g.fillEllipse(sx, sy, sr, sr);
        }
    }

    // --- 3) Title glow: "ORBITAL" bloom effect ---
    // (drawn behind the Label — the Label will draw crisp text on top)
    {
        const auto titleFont = juce::Font(juce::FontOptions(38.0f).withStyle("Bold"));
        g.setFont(titleFont);

        // Bloom layer (larger, lower alpha)
        g.setColour(neonCyan.withAlpha(0.12f));
        for (int dx = -2; dx <= 2; ++dx)
            for (int dy = -2; dy <= 2; ++dy)
                g.drawText("ORBITAL", dx, 8 + dy, w, 50, juce::Justification::centred);

        // Mid glow
        g.setColour(neonCyan.withAlpha(0.25f));
        for (int dx = -1; dx <= 1; ++dx)
            for (int dy = -1; dy <= 1; ++dy)
                g.drawText("ORBITAL", dx, 8 + dy, w, 50, juce::Justification::centred);
    }

    // --- 4) Glowing section separators ---
    auto drawGlowSeparator = [&](int lineY)
    {
        const float cx = (float)w * 0.5f;
        const float halfLen = (float)w * 0.5f - 20.0f;

        // Hot purple/cyan glow at center, fading to transparent at edges
        juce::ColourGradient sepGrad(hotPurple.withAlpha(0.5f), cx - halfLen, (float)lineY,
                                      juce::Colours::transparentBlack, cx - halfLen, (float)lineY, false);
        sepGrad.addColour(0.0,  juce::Colours::transparentBlack);
        sepGrad.addColour(0.3,  hotPurple.withAlpha(0.3f));
        sepGrad.addColour(0.5,  neonCyan.withAlpha(0.5f));
        sepGrad.addColour(0.7,  hotPurple.withAlpha(0.3f));
        sepGrad.addColour(1.0,  juce::Colours::transparentBlack);
        sepGrad.point1 = { 20.0f, (float)lineY };
        sepGrad.point2 = { (float)w - 20.0f, (float)lineY };

        g.setGradientFill(sepGrad);
        g.fillRect(20.0f, (float)lineY - 0.5f, (float)w - 40.0f, 1.5f);
    };

    drawGlowSeparator(60);   // under title
    drawGlowSeparator(190);  // under OSC row
    drawGlowSeparator(316);  // under Filter/Distortion row
    drawGlowSeparator(442);  // under Envelope/Delay/Master row

    // --- 5) Synthwave horizon grid behind the visualizer area ---
    {
        const float vizLeft  = 20.0f;
        const float vizRight = (float)w - 20.0f;
        const float vizTop   = 452.0f;
        const float vizBot   = 604.0f;
        const int numLines = 6;

        for (int i = 0; i < numLines; ++i)
        {
            float t = (float)i / (float)(numLines - 1);          // 0 at top, 1 at bottom
            float ly = vizTop + t * (vizBot - vizTop);
            float alpha = 0.05f + t * 0.2f;                      // brighter near bottom

            g.setColour(neonPink.withAlpha(alpha));
            g.drawHorizontalLine((int)ly, vizLeft, vizRight);
        }
    }

    // --- 6) Section header glow (subtle glow behind the Label text) ---
    auto drawSectionGlow = [&](const juce::Label& label)
    {
        auto bounds = label.getBounds();
        g.setColour(neonCyan.withAlpha(0.08f));
        g.fillRoundedRectangle(bounds.toFloat().expanded(4.0f, 2.0f), 3.0f);
    };

    drawSectionGlow(osc1SectionLabel);
    drawSectionGlow(osc2SectionLabel);
    drawSectionGlow(filter1SectionLabel);
    drawSectionGlow(filter2SectionLabel);
    drawSectionGlow(distSectionLabel);
    drawSectionGlow(envSectionLabel);
    drawSectionGlow(delaySectionLabel);
    drawSectionGlow(masterSectionLabel);
}

void OrbitalEditor::resized()
{
    const int w = getWidth();

    titleLabel.setBounds(0, 8, w, 50);

    // =========================================================================
    // ROW 1: OSC 1 (left half) | OSC 2 (right half)   y: 66-186
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
    // ROW 2: Filter 1 | Filter 2 | Distortion   y: 196-312
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
    // ROW 3: Envelope (4 knobs) | Delay (3 knobs) | Master (1 knob)  y: 322-438
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
    // Visualizer   y: 452-604
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
