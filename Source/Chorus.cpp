
#include "Chorus.h"
#include <fstream>
void Chorus::prepare(double sampleRate, int widthValue)
{

    delayLine->prepare(sampleRate);
    
    phaseSmoothing.reset(sampleRate, 0.05);
    phaseSmoothing.setCurrentAndTargetValue((float)widthValue);
    rate = 0.1f;
    // Calculate how much phase to add per sample to achieve desired frequency
    lfoIncrement = (rate * juce::MathConstants<float>::twoPi) / sampleRate;

    depth = 0.015f;
    mix = 0.5f;
    
    // Initialze depth smoothing
    currentDepth = depth;
    targetDepth = depth;
    
    lfoPhase1 = 0.0f;
    lfoPhase2 = juce::MathConstants<float>::twoPi / 5.0f;
    lfoPhase3 = juce::MathConstants<float>::twoPi * 2.0f / 5.0f;
    lfoPhase4 = juce::MathConstants<float>::pi;
    lfoPhase5 = juce::MathConstants<float>::pi + juce::MathConstants<float>::twoPi / 5.0f;
    lfoPhase6 = juce::MathConstants<float>::pi + juce::MathConstants<float>::twoPi * 2.0f / 5.0f;
    

 
}
Chorus::~Chorus()
{
    reset();
}
void Chorus::reset()
{
    delayLine->reset();
}

void Chorus::processBlock(float* bufferData, float* delayData, int bufferSize, int delayBufferSize, double sampleRate, int& writePos, bool isLeft, int widthValue)
{
 
    currentDepth += (targetDepth - currentDepth) * 0.001f;
    lfoIncrement = (rate * juce::MathConstants<float>::twoPi) / sampleRate;
    
    if (widthValue != previousWidthValue)
    {
        phaseSmoothing.setTargetValue((float)widthValue);
        previousWidthValue = widthValue;
    }

    for (int i = 0; i < bufferSize; ++i)
    {
      
        float spread = getPhase(phaseSmoothing.getNextValue());

        if (isLeft)
        {
            lfo1 = lfo(lfoPhase1);
            lfo2 = lfo(lfoPhase2);
            lfo3 = lfo(lfoPhase3);
            lfo4 = lfo(lfoPhase7);
            lfo5 = lfo(lfoPhase8);
            lfo6 = lfo(lfoPhase9);

            mod1 = modulation(currentDepth, lfo1);
            mod2 = modulation(currentDepth, lfo2);
            mod3 = modulation(currentDepth, lfo3);
            mod4 = modulation(currentDepth, lfo4);
            mod5 = modulation(currentDepth, lfo5);
            mod6 = modulation(currentDepth, lfo6);
        }
        else
        {
            
            // Advance phase
            lfo(lfoPhase4);
            lfo(lfoPhase5);
            lfo(lfoPhase6);
            lfo(lfoPhase10);
            lfo(lfoPhase11);
            lfo(lfoPhase12);
            
            lfo1 = std::sin(lfoPhase4 + spread);
            lfo2 = std::sin(lfoPhase5 + spread);
            lfo3 = std::sin(lfoPhase6 + spread);
            lfo4 = std::sin(lfoPhase10 + spread);
            lfo5 = std::sin(lfoPhase11 + spread);
            lfo6 = std::sin(lfoPhase12 + spread);
            
            mod1 = modulation(currentDepth, lfo1);
            mod2 = modulation(currentDepth, lfo2);
            mod3 = modulation(currentDepth, lfo3);
            mod4 = modulation(currentDepth, lfo4);
            mod5 = modulation(currentDepth, lfo5);
            mod6 = modulation(currentDepth, lfo6);
            
        
        }
        
        float delay1 = calculateTotalDelay(baseDelay, mod1);
        float delay2 = calculateTotalDelay(baseDelay, mod2);
        float delay3 = calculateTotalDelay(baseDelay, mod3);
        float delay4 = calculateTotalDelay(baseDelay, mod4);
        float delay5 = calculateTotalDelay(baseDelay, mod5);
        float delay6 = calculateTotalDelay(baseDelay, mod6);
    
        std::vector<float> delays = {delay1, delay2, delay3, delay4, delay5, delay6};
            
        // Process single sample
        delayLine->processSample(bufferData[i], delayData, delayBufferSize, sampleRate, delays, writePos, baseDelay, mix, isLeft);
        // Safety clip
        bufferData[i] = juce::jlimit((-1.0f), 1.0f, bufferData[i]);
    }
}

float Chorus::lfo(float& lfoPhase)
{
    float lfo = std::sin(lfoPhase);
    lfoPhase += lfoIncrement;
   
    // wrap phase, prevent overflow
    if (lfoPhase >= juce::MathConstants<float>::twoPi)
    {
        lfoPhase -= juce::MathConstants<float>::twoPi;
    }
    
    return lfo;
    
}

float Chorus::modulation(float depth, float lfo)
{
    float mod = depth * lfo;
    return mod;
}
void Chorus::setDepth(float newDepth)
{
    
    targetDepth = juce::jlimit(0.00f, 0.08f, newDepth);
    
    // Scale depth based on rate
    if (rate > 2.0f)
    {
        targetDepth *= juce::jmap(rate,2.0f, 5.0f, 1.0f, 0.5f);
    }
    
    depth = targetDepth;
}

float Chorus::calculateTotalDelay(float baseDelay, float mod)
{
    float totalDelay = baseDelay + mod;
    
    totalDelay = juce::jlimit(0.005f, 0.050f, totalDelay);
    
    return totalDelay;
}

void Chorus::setBaseDelay(float newBaseDelay)
{
    baseDelay = newBaseDelay;
    delayLine->setDelayTime(baseDelay);
}

void Chorus::setRate(float newRate, double sampleRate)
{
    rate = juce::jlimit(0.1f, 5.0f, newRate);
    lfoIncrement = (rate * juce::MathConstants<float>::twoPi) / sampleRate;
    
}

void Chorus::setMix(float newMix)
{
    mix = newMix;
}

void Chorus::setWidth(float newWidth, float phase)
{
    if (width != static_cast<int>(std::round(newWidth)))
    {
        width = static_cast<int>(std::round(newWidth));
        phaseSmoothing.setTargetValue(getPhase(width));
    }
    
}

float Chorus::getPhase(float newWidth)
{
    
    int widthInt = juce::jlimit(1, 10, static_cast<int>(std::round(newWidth)));
    return widthInt * 0.01f * juce::MathConstants<float>::pi;
}

int Chorus::getWidth()
{
    return width;
}
