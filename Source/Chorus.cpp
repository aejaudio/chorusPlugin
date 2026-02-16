
#include "Chorus.h"
#include <fstream>
void Chorus::prepare(double sampleRate)
{

    delayLine->prepare(sampleRate);
    rate = 0.1f;
    // Calculate how much phase to add per sample to achieve desired frequency
    lfoIncrement = (rate * juce::MathConstants<float>::twoPi) / sampleRate;
//    baseDelay = 0.015f;
    delayLine->setDelayTime(baseDelay);
//    depth = 0.015f;
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
        
        static int counter = 0;
            if (++counter % 48000 == 0)
            {
                std::cout << "BaseDelay: " << baseDelay
                          << " | Depth: " << depth
                          << " | LFO1: " << lfo1
                          << " | Mod1: " << mod1
                          << " | Delay1: " << delay1
                          << " | Delay2: " << delay2
                          << " | Delay3: " << delay3 << std::endl;
            }
            
        
        std::vector<float> delays = {delay1, delay2, delay3};
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
    float pi = juce::MathConstants<float>::pi;
    float phase = 0.0f;
    
    int widthInt = static_cast<int>(std::round(newWidth));
    switch (widthInt)
    {
        case 1:
            phase = pi * 0.01f;
            break;
            
        case 2:
            phase = pi * 0.02f;
            break;
            
        case 3:
            phase = pi * 0.03f;
            break;
            
        case 4:
            phase = pi * 0.04f;
            break;
            
        case 5:
            phase = pi * .05f;
            break;
            
        case 6:
            phase = pi * 0.06f;
            break;
            
        case 7:
            phase = pi * 0.07f;
            break;
        
        case 8:
            phase = pi * 0.08f;
            break;
            
        case 9:
            phase = pi * 0.09f;
            break;
            
        case 10:
            phase = pi * 0.1f;
            break;
            
        default:
            phase = pi * 0.05f;
            break;
        }
    
    return phase;
}




