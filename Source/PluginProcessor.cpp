/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <fstream>
//==============================================================================
Chorus2AudioProcessor::Chorus2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts (*this, nullptr, "Parameters", createParameters()), chorus(apvts) // pass reference to chorus and initialize chorus
#endif
{
    
}

Chorus2AudioProcessor::~Chorus2AudioProcessor()
{
    delayBuffer.clear();
}

//==============================================================================
const juce::String Chorus2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Chorus2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Chorus2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Chorus2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Chorus2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Chorus2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Chorus2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Chorus2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Chorus2AudioProcessor::getProgramName (int index)
{
    return {};
}

void Chorus2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Chorus2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    
    depthParameter = apvts.getRawParameterValue("DEPTH");
    jassert(depthParameter != nullptr);
    
    rateParameter = apvts.getRawParameterValue("RATE");
    jassert(rateParameter != nullptr);
    
    mixParameter = apvts.getRawParameterValue("MIX");
    jassert(mixParameter != nullptr);
    
    widthParameter = apvts.getRawParameterValue("WIDTH");
    jassert(widthParameter != nullptr);
    
    // Initialize delay buffer to 2 seconds of audio
    int newDelayBufferSize = static_cast<int>(sampleRate * 2.0);
    // Set size of delayBuffer outputs and delayBufferSize in samples
    delayBuffer.setSize(getTotalNumOutputChannels(), static_cast<int>(delayBufferSize));
    
    if (delayBuffer.getNumSamples() != newDelayBufferSize || delayBuffer.getNumChannels() != getTotalNumOutputChannels())
    {
        delayBufferSize = newDelayBufferSize;
        delayBuffer.setSize(getTotalNumOutputChannels(), delayBufferSize);
        delayBuffer.clear();
    }

    chorus.prepare(sampleRate);

}

void Chorus2AudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Chorus2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Chorus2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Used to calculate delay in samples
    auto sampleRate = getSampleRate();

    // Load atomic float from apvts thread safe
    // Read current value stored in feedbackParameter and return as float
    chorus.setBaseDelay(0.025f);
    chorus.setRate(rateParameter->load(std::memory_order_relaxed), sampleRate);
    chorus.setDepth(depthParameter->load(std::memory_order_relaxed));
    chorus.setMix(mixParameter->load(std::memory_order_relaxed));
    chorus.setWidth(widthParameter->load(std::memory_order_relaxed));
    
    int bufferSize = buffer.getNumSamples(); // buffer size

    // Process left channel
       if (totalNumInputChannels > 0)
       {
           auto* leftBufferData = buffer.getWritePointer(0);
           auto* leftDelayData = delayBuffer.getWritePointer(0);
           chorus.processBlock(leftBufferData, leftDelayData, bufferSize, delayBufferSize, sampleRate ,leftWritePos, true);
       }
    // Process right Channel
        if (totalNumInputChannels > 1)
        {
            auto* rightBufferData = buffer.getWritePointer(1);
            auto* rightDelayData = delayBuffer.getWritePointer(1);
            chorus.processBlock(rightBufferData, rightDelayData, bufferSize, delayBufferSize, sampleRate, rightWritePos, false);
        }
}

//==============================================================================
bool Chorus2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Chorus2AudioProcessor::createEditor()
{
    return new Chorus2AudioProcessorEditor (*this);
}

//==============================================================================
void Chorus2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Chorus2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if(xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Chorus2AudioProcessor();
}
// Returns ParameterLayout Object - List of all  parameters
// - Called in processor constructor
juce::AudioProcessorValueTreeState::ParameterLayout
    Chorus2AudioProcessor::createParameters()
{
        // Hold unique pointers to parameters, when vector destroyed, parameters are
        // automatically cleaned up.
        // RangedAudioParameter - base class for all parameter types
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
        
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>
                             ( juce::ParameterID {"FEEDBACK",1}, // ParameterID and version                                 // number for param                                        // compatibility
                              "Feedback", // Title shown to users
                              juce::NormalisableRange<float> // range( min, max )
                              ( 0.0f, 0.9f), 0.7f));         // default value
        
        
        parameters.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"DEPTH", 1}, "Depth", juce::NormalisableRange<float> (0.001f, 0.008), 0.015f));
        
        parameters.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"RATE", 1}, "Rate", juce::NormalisableRange<float> (0.1f, 5.0f), 1.0f));
        
        parameters.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"MIX", 1}, "Mix", juce::NormalisableRange<float> (0.1f, 1.0f), 0.5f));
        
        parameters.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"WIDTH", 1},
                                                                          "Width", juce::NormalisableRange<float> (1.0f, 10.0f, 1.0f), 5.0f));
        
        return { parameters.begin(), parameters.end() };
}
    
