
#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"

class Chorus
{
public:
    
    Chorus(juce::AudioProcessorValueTreeState& apvts) : delayLine(std::make_unique<DelayLine>(apvts)){}
    ~Chorus();
    
    void prepare(double sampleRate, int widthValue);
    void processBlock(float* bufferData, float* delayData, int bufferSize, int delayBufferSize, double sampleRate, int& writePos, bool isLeft, int widthValue);
    void reset();
    void resetDelays(double sampleRate);
    void setDepth(float newDepth);
    float modulation(float depth, float lfo);
    float getModulation();
    float calculateTotalDelay(float baseDelay, float mod);
    void setBaseDelay(float newBaseDelay);
    void setRate(float newRate, double sampleRate);
    void setMix(float newMix);
    void setWidth(float newWidth, float phase);
    float getPhase(float newWidth);
    void updateDelay(int width);
    
    void updateMod(bool isLeft, int bufferSize);
    
    int getWidth();
    
private:
    
    
    float lfo1;
    float lfo2;
    float lfo3;
    float lfo4;
    float lfo5;
    float lfo6;

    float lfoPhase1;
    float lfoPhase2;
    float lfoPhase3;
    float lfoPhase4;
    float lfoPhase5;
    float lfoPhase6;
    float lfoPhase7;
    float lfoPhase8;
    float lfoPhase9;
    float lfoPhase10;
    float lfoPhase11;
    float lfoPhase12;
    int width = 1;
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> phaseSmoothing { 0.0f };

    int previousWidthValue;
    
    // Frequency of LFO in Hz
    // Cycles per second
    float rate = 0.5f;
    float lfoIncrement;
    float mod1;
    float mod2;
    float mod3;
    float mod4;
    float mod5;
    float mod6;
    float depth;
    float baseDelay;
    float delayTime = 0.0f;
    float lfo(float& lfoPhase);
    float totalDelay1 = 0.025f;
    float totalDelay2 = 0.03f;
    float totalDelay3 = 0.015f;
    
    float mix = 0.5f;
    
    int leftWritePosition = 0;
    int rightWritePosition = 0;
    
    float targetDepth = 0.005f;
    float currentDepth = 0.005f;
    
    std::unique_ptr<DelayLine> delayLine;
    
    
    float twoPi = juce::MathConstants<float>::twoPi;
    

};
