#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class Y2KConceptLookAndFeel : public juce::LookAndFeel_V4
{
public:
    Y2KConceptLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff00f0ff));
        setColour(juce::Slider::thumbColourId, juce::Colour(0xffff007f));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto radius = (float)juce::jmin(width, height) / 2.0f - 6.0f;
        auto centreX = (float)x + (float)width  * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(juce::Colour(0xff18002e));
        g.fillEllipse(rx, ry, radius * 2.0f, radius * 2.0f);

        g.setColour(juce::Colour(0xff00f0ff));
        g.drawEllipse(rx, ry, radius * 2.0f, radius * 2.0f, 2.0f);

        juce::Path arc;
        arc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(juce::Colour(0xffff007f));
        g.strokePath(arc, juce::PathStrokeType(4.0f));

        juce::Path p;
        p.addRectangle(-2.0f, -radius + 4.0f, 4.0f, radius * 0.6f);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }
};

class CyberWaveAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::ComboBox::Listener
{
public:
    CyberWaveAudioProcessorEditor (CyberWaveAudioProcessor&);
    ~CyberWaveAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;

private:
    CyberWaveAudioProcessor& audioProcessor;

    juce::Slider bigWetKnob;
    juce::Label bigWetLabel;

    juce::Slider cutoffKnob, resonanceKnob, attackKnob, releaseKnob;
    juce::Label cutoffLabel, resLabel, attackLabel, releaseLabel;

    juce::ComboBox presetSelector;
    juce::Label presetLabel;

    juce::MidiKeyboardComponent keyboardComponent;

    Y2KConceptLookAndFeel conceptLNF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CyberWaveAudioProcessorEditor)
};
