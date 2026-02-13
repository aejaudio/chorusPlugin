/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Chorus2AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Chorus2AudioProcessorEditor (Chorus2AudioProcessor&);
    ~Chorus2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void setUpSlider(juce::Slider &slider, juce::Label &label, const juce::String &labelText, double min, double max, double defaultValue, const juce::String& suffix);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Chorus2AudioProcessor& processor;
    
    juce::Slider feedbackKnob;
    juce::Label feedbackLabel;
    
    juce::Slider delayTimeKnob;
    juce::Label delayTimeLabel;
    
    juce::Slider depthKnob;
    juce::Label depthLabel;
    
    juce::Slider gainKnob;
    juce::Label gainLabel;
    
    juce::Slider rateKnob;
    juce::Label rateLabel;
    
    juce::Slider mixKnob;
    juce::Label mixLabel;
    
    juce::Slider widthKnob;
    juce::Label widthLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    delayTimeAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Chorus2AudioProcessorEditor)
};
