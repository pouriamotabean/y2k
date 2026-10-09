#include "PluginProcessor.h"
#include "PluginEditor.h"

CyberWaveAudioProcessorEditor::CyberWaveAudioProcessorEditor (CyberWaveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      keyboardComponent (p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (800, 520);

    presetSelector.addItemList({
        "--- BASS ---", "01. Neon Sub Bass", "02. Analog Saw Bass",
        "--- LEAD ---", "03. Cyber Lead", "04. Vaporwave Glide",
        "--- PLUCK ---", "05. 80s Retro Pluck", "06. Glassy Staccato",
        "--- KEY ---", "07. Chillwave Keys", "08. Vintage DX-Tines",
        "--- PIANO ---", "09. Cyber Synth Piano",
        "--- STAB ---", "10. Detuned Brass Stab", "11. Y2K Dance Stab"
    }, 1);
    presetSelector.setSelectedId(2);
    presetSelector.addListener(this);
    presetSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff1a0033));
    presetSelector.setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff00f0ff));
    presetSelector.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    addAndMakeVisible(presetSelector);

    presetLabel.setText("PRESET", juce::dontSendNotification);
    presetLabel.setFont(juce::Font("Arial", 12.0f, juce::Font::bold));
    presetLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00f0ff));
    addAndMakeVisible(presetLabel);

    bigWetKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bigWetKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    bigWetKnob.setLookAndFeel(&conceptLNF);
    bigWetKnob.setRange(0.0, 1.0, 0.01);
    bigWetKnob.setValue(0.40);
    bigWetKnob.onValueChange = [this]() {
        audioProcessor.wetDelayAmount = (float)bigWetKnob.getValue();
    };
    addAndMakeVisible(bigWetKnob);

    bigWetLabel.setText("SPACE / WET", juce::dontSendNotification);
    bigWetLabel.setFont(juce::Font("Arial", 16.0f, juce::Font::bold));
    bigWetLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff007f));
    bigWetLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bigWetLabel);

    auto setupSubKnob = [this](juce::Slider& slider, juce::Label& label, const juce::String& name) {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setLookAndFeel(&conceptLNF);
        slider.setRange(0.0, 1.0, 0.01);
        slider.setValue(0.7);
        addAndMakeVisible(slider);

        label.setText(name, juce::dontSendNotification);
        label.setFont(juce::Font("Arial", 11.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(0xff00f0ff));
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };

    setupSubKnob(cutoffKnob, cutoffLabel, "CUTOFF");
    setupSubKnob(resonanceKnob, resLabel, "RESO");
    setupSubKnob(attackKnob, attackLabel, "ATTACK");
    setupSubKnob(releaseKnob, releaseLabel, "RELEASE");

    addAndMakeVisible(keyboardComponent);
    keyboardComponent.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe6e6fa));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff120024));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff00f0ff));
}

CyberWaveAudioProcessorEditor::~CyberWaveAudioProcessorEditor()
{
    bigWetKnob.setLookAndFeel(nullptr);
    cutoffKnob.setLookAndFeel(nullptr);
    resonanceKnob.setLookAndFeel(nullptr);
    attackKnob.setLookAndFeel(nullptr);
    releaseKnob.setLookAndFeel(nullptr);
}

void CyberWaveAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient bgGrad(juce::Colour(0xff0c0019), 0, 0, juce::Colour(0xff260042), 0, (float)getHeight(), false);
    g.setGradientFill(bgGrad);
    g.fillAll();

    g.setColour(juce::Colour(0xff00f0ff).withAlpha(0.07f));
    for (int x = 0; x < getWidth(); x += 25)
        g.drawVerticalLine(x, 0.0f, (float)getHeight() - 90);
    for (int y = 0; y < getHeight() - 90; y += 25)
        g.drawHorizontalLine(y, 0.0f, (float)getWidth());

    juce::Rectangle<float> waveDisplayArea(280.0f, 20.0f, 240.0f, 80.0f);
    g.setColour(juce::Colour(0xff100021));
    g.fillRect(waveDisplayArea);
    g.setColour(juce::Colour(0xff00f0ff));
    g.drawRect(waveDisplayArea, 2.0f);

    juce::Path wavePath;
    wavePath.startNewSubPath(285.0f, 60.0f);
    for (float x = 285.0f; x < 515.0f; x += 10.0f) {
        float y = 60.0f + std::sin((x - 285.0f) * 0.1f) * 25.0f;
        wavePath.lineTo(x, y);
    }
    g.setColour(juce::Colour(0xffff007f));
    g.strokePath(wavePath, juce::PathStrokeType(2.5f));

    g.setColour(juce::Colour(0xff00f0ff));
    g.drawRect(getLocalBounds(), 3);

    g.setColour(juce::Colour(0xffff007f));
    g.setFont(juce::Font("Impact", 26.0f, juce::Font::plain));
    g.drawText("CYBERWAVE Y2K", 20, 20, 220, 30, juce::Justification::left);
}

void CyberWaveAudioProcessorEditor::resized()
{
    presetLabel.setBounds(580, 20, 180, 15);
    presetSelector.setBounds(580, 40, 180, 28);

    int bigSize = 180;
    bigWetKnob.setBounds((getWidth() - bigSize) / 2, 120, bigSize, bigSize);
    bigWetLabel.setBounds((getWidth() - 200) / 2, 305, 200, 25);

    int subSize = 65;
    cutoffKnob.setBounds(80, 150, subSize, subSize);
    cutoffLabel.setBounds(60, 220, 105, 20);

    resonanceKnob.setBounds(180, 150, subSize, subSize);
    resLabel.setBounds(160, 220, 105, 20);

    attackKnob.setBounds(550, 150, subSize, subSize);
    attackLabel.setBounds(530, 220, 105, 20);

    releaseKnob.setBounds(650, 150, subSize, subSize);
    releaseLabel.setBounds(630, 220, 105, 20);

    keyboardComponent.setBounds(10, getHeight() - 100, getWidth() - 20, 90);
}

void CyberWaveAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &presetSelector)
    {
        audioProcessor.loadPresetById(presetSelector.getSelectedId());
    }
}
