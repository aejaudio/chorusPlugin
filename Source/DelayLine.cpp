
#include "DelayLine.h"
#include <fstream>

DelayLine::~DelayLine()
{
    reset();
}
void DelayLine::prepare(double sampleRate)
{
    
    filterL.reset();
    filterL.prepare(sampleRate, 0.02f);
    filterR.reset();
    filterR.prepare(sampleRate, 0.02f);
}


void DelayLine::reset()
{
    writePosition = 0;
}

void DelayLine::processSample(float& sample, float* delayData, int delayBufferSize, double sampleRate, std::vector<float> delays, int& writePos, float baseDelay, float mix, bool isLeft)
{
    float averageDelay = 0;
    for(int i = 0; i < delays.size(); i++)
    {
        averageDelay += delays[i];
    }
    
    averageDelay /= 6.0f;
    

    if (isLeft)
    {
        processFilter(averageDelay, filterL);
    }
    else
    {
        processFilter(averageDelay, filterR);
    }
    // Store clean input
    float inputSample = sample;

    float delaySum = 0;
    for (int i = 0; i < delays.size(); i++)
    {
        float delaySamplesFloat = calculateDelayFloat(delays[i], sampleRate);

        // Calculate read position for linear interpolation
        float currentReadPosition = static_cast<float>(writePos) - delaySamplesFloat;
        
        // Read delayed sample with interpolation
        float delayedSample = linearInterpolation(delayData, currentReadPosition, delayBufferSize);
  
        delaySum += delayedSample;
        
    }
    delaySum = delaySum / 6.0f;
    
    // write input to delay line
    delayData[writePos] = inputSample;
    
    if (isLeft)
    {
        delaySum = filterL.biquadFilter(delaySum);
        delaySum = filterL.asymmetricSaturation(delaySum, 2.0f, 1.0f);
    }
    else
    {
        delaySum = filterR.biquadFilter(delaySum);
        delaySum = filterR.asymmetricSaturation(delaySum, 2.0f, 1.0f);
    }
    
    delaySum *= 2.0f;
    sample = (inputSample * (1.0 - mix)) + (delaySum * mix);
    
    // Increment write position
    writePos = (writePos + 1) % delayBufferSize;
    

}
float DelayLine::linearInterpolation(float* buffer, float readPosition, int bufferSize)
{
    // Handle negative read positions
    if (readPosition < 0.0f)
        readPosition += bufferSize;
    
    // Edge case if position gets way out of range
    if (readPosition >= bufferSize)
    {
        readPosition = fmod(readPosition, static_cast<float>(bufferSize));
    }
    // Get int indices
    int index1 = static_cast<int>(readPosition);
    // Use modulo to wrap to begininng if index doesn't exist
    int index2 = (index1 + 1) % bufferSize;
    
    index1 = index1 % bufferSize;
    
    // Get fraction: if readPosition is 20.7f then (readPosition - index1) would be .7 (20.7-20 = .7)
    float fraction = readPosition - static_cast<float>(index1);
    
    fraction = juce::jlimit(0.0f, 1.0f, fraction);
    
    // Interpolate
    // y = (1 - t)y₀ + ty₁
    // y₀ = start value
    // y₁ = end value
    // t ∈ [0, 1] = interpolation parameter (fraction)
    // y = interpolated result
    float result = buffer[index1] * (1.0f - fraction) + buffer[index2] * fraction;
    return result;
}

float DelayLine::calculateDelayFloat(float totalDelay, double sampleRate)
{
    float delayedSamplesFloat = totalDelay * sampleRate;
    return delayedSamplesFloat;
}

void DelayLine::processFilter(float averageDelay, Filter filter)
{
    updateCounter++;
    if (updateCounter >= 512)
    {
        filter.updateCoeff(averageDelay);
        updateCounter = 0;
    }
}
void DelayLine::setDelayTime(float newDelayTime)
{
    delayTime = juce::jlimit (0.015f, 0.025f, newDelayTime);
}
