/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Chorus2AudioProcessorEditor::Chorus2AudioProcessorEditor (Chorus2AudioProcessor& p)
    : AudioProcessorEditor (&p), processor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 300);

    
    delayTimeLabel.setText("Base Delay", juce::dontSendNotification);
    delayTimeLabel.setJustificationType(juce::Justification(juce::Justification::centred));
    addAndMakeVisible(delayTimeLabel);
    
    // Depth
    depthKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    depthKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
    depthKnob.setRange(0.001f, 0.010f, 0.01f);
    addAndMakeVisible(depthKnob);
    
    depthLabel.setText("Depth", juce::dontSendNotification);
    depthLabel.setJustificationType(juce::Justification(juce::Justification::centred));
    addAndMakeVisible(depthLabel);
    // Attach slider to parameter
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "DEPTH", depthKnob);
    
    // Rate
    rateKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    rateKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
    rateKnob.setRange(0.1f, 3.0f, 0.1f);
    addAndMakeVisible(rateKnob);
    
    rateLabel.setText("Rate", juce::dontSendNotification);
    rateLabel.setJustificationType(juce::Justification(juce::Justification::centred));
    addAndMakeVisible(rateLabel);
    // Attach slider to parameter
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "RATE", rateKnob);
    
    // Mix
    mixKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
    mixKnob.setRange(0.0f, 1.0f, 0.5f);
    addAndMakeVisible(mixKnob);
    
    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification(juce::Justification::centred));
    addAndMakeVisible(mixLabel);
    // Attach slider to parameter
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "MIX", mixKnob);
    
    widthKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    widthKnob.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, true, 50, 10);
    widthKnob.setRange(0, 10, 1);
    widthKnob.setNumDecimalPlacesToDisplay(0);
    addAndMakeVisible(widthKnob);
    
    widthLabel.setText("Width", juce::dontSendNotification);
    widthLabel.setJustificationType(juce::Justification(juce::Justification::centred));
    addAndMakeVisible(widthLabel);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts, "WIDTH", widthKnob);
}

Chorus2AudioProcessorEditor::~Chorus2AudioProcessorEditor()
{
    Component::setLookAndFeel (nullptr);
}

//==============================================================================
void Chorus2AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::purple);

    g.setColour (juce::Colours::black);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Chorus", getLocalBounds(), juce::Justification::centredTop, 1);
}

void Chorus2AudioProcessorEditor::resized()
{
    
    int knobSize = 100;
    depthLabel.setBounds(120, 60, knobSize, 20);
    depthKnob.setBounds(120, 85, knobSize, knobSize);
    
    rateLabel.setBounds(20, 60, knobSize, 20);
    rateKnob.setBounds(20, 85, knobSize, knobSize);
    
    mixLabel.setBounds(240, 60, knobSize, 20);
    mixKnob.setBounds(240, 85, knobSize, knobSize);
    
    widthLabel.setBounds(360, 60, knobSize, 20);
    widthKnob.setBounds(360, 85, knobSize, knobSize);
//    
//    widthLabel.setBounds(480, 60, knobSize, 20);
//    widthKnob.setBounds(480, 85, knobSize, knobSize);
}

