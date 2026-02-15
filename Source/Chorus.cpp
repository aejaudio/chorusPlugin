
#include "Chorus.h"
#include <fstream>
void Chorus::prepare(double sampleRate)
{

    delayLine->prepare(sampleRate);
    // Calculate how much phase to add per sample to achieve desired frequency
    lfoIncrement = (rate * juce::MathConstants<float>::twoPi) / sampleRate;
    baseDelay = 0.025f;
    depth = 0.005f;
    mix = 0.5f;
    
    // Initialze depth smoothing
    currentDepth = depth;
    targetDepth = depth;
    
    lfoPhase1 = 0.0f;
    lfoPhase2 = juce::MathConstants<float>::twoPi / 3.0f;
    lfoPhase3 = juce::MathConstants<float>::twoPi * 2.0f / 3.0f;
    lfoPhase4 = juce::MathConstants<float>::pi;
    lfoPhase5 = juce::MathConstants<float>::pi + juce::MathConstants<float>::twoPi / 3.0f;
    lfoPhase6 = juce::MathConstants<float>::pi + juce::MathConstants<float>::twoPi * 2.0f / 3.0f;
    
 
}
Chorus::~Chorus()
{
    reset();
}
void Chorus::reset()
{
    delayLine->reset();
}
void Chorus::processBlock(float* bufferData, float* delayData, int bufferSize, int delayBufferSize, double sampleRate, int& writePos, bool isLeft)
{
 
    currentDepth += (targetDepth - currentDepth) * 0.001f;
    lfoIncrement = (rate * juce::MathConstants<float>::twoPi) / sampleRate;
    for (int i = 0; i < bufferSize; ++i)
    {
        
        
        if (isLeft)
        {
            lfo1 = lfo(lfoPhase1);
            lfo2 = lfo(lfoPhase2);
            lfo3 = lfo(lfoPhase3);
            mod1 = modulation(currentDepth, lfo1);
            mod2 = modulation(currentDepth, lfo2);
            mod3 = modulation(currentDepth, lfo3);
        }
        else
        {
            lfo1 = lfo(lfoPhase4);
            lfo2 = lfo(lfoPhase5);
            lfo3 = lfo(lfoPhase6);
            mod1 = modulation(currentDepth, lfo1);
            mod2 = modulation(currentDepth, lfo2);
            mod3 = modulation(currentDepth, lfo3);
        }
    
        float delay1 = calculateTotalDelay(baseDelay, mod1);
        float delay2 = calculateTotalDelay(baseDelay, mod2);
        float delay3 = calculateTotalDelay(baseDelay, mod3);
        
        std::vector<float> delays = {delay1, delay2, delay3};
        // Process single sample
        delayLine->processSample(bufferData[i], delayData, delayBufferSize, sampleRate, delays, writePos, baseDelay, mix);
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
    
    targetDepth = juce::jlimit(0.001f, 0.010f, newDepth);
    
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
    
    totalDelay = juce::jlimit(0.001f, 0.050f, totalDelay);
    
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

void Chorus::setWidth(float newWidth)
{
    if (width == static_cast<int>(std::round(newWidth)))
        return;
    
    width = static_cast<int>(std::round(newWidth));
    float phase = setPhase(width);
    float pi = juce::MathConstants<float>::pi;
    
    
    // Distribute phase across voices
    lfoPhase1 = 0.0f;
    lfoPhase2 = phase / 3.0f;
    lfoPhase3 = (phase * 2.0f) / 3.0f;
    
    
    // Right channel opposite phases
    lfoPhase4 = pi;
    lfoPhase5 = pi + phase / 3.0f;
    lfoPhase6 = pi + (phase * 2.0f) / 3.0f;
}

float Chorus::setPhase(float newWidth)
{
    float pi = juce::MathConstants<float>::twoPi;
    float phase = 0.0f;
    
    int widthInt = static_cast<int>(std::round(newWidth));
    switch (widthInt)
    {
        case 1:
            phase = pi * 0.25f;
            break;
            
        case 2:
            phase = pi * 0.5f;
            break;
            
        case 3:
            phase = pi * 0.75f;
            break;
            
        case 4:
            phase = pi * 1.0f;
            break;
            
        case 5:
            phase = pi * 1.5f;
            break;
            
        case 6:
            phase = pi * 2.0f;
            break;
            
        case 7:
            phase = pi * 2.5f;
            break;
        
        case 8:
            phase = pi * 3.0f;
            break;
            
        case 9:
            phase = pi * 3.5f;
            break;
            
        case 10:
            phase = pi * 4.0f;
            break;
            
        default:
            phase = pi * 0.5f;
            break;
        }
    
    return phase;
}




