/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"
#include "Chorus.h"

//==============================================================================
/**
*/
class Chorus2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Chorus2AudioProcessor();
    ~Chorus2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Chorus2AudioProcessor)

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize;
    
    Chorus chorus;
    
    // Can be accessed by multiple threads simultaneously (Audio and GUI)
    std::atomic<float>* feedbackParameter = nullptr;
    std::atomic<float>* delayTimeParameter = nullptr;
    std::atomic<float>* depthParameter = nullptr;
    std::atomic<float>* rateParameter = nullptr;
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* widthParameter = nullptr;
    
    int leftWritePos = 0;
    int rightWritePos = 0;

};
