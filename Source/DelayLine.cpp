
#include "DelayLine.h"
#include <fstream>

DelayLine::~DelayLine()
{
    reset();
}
void DelayLine::prepare(double sampleRate)
{
    
    filter.reset();
    filter.prepare(sampleRate, 0.02f);
}


void DelayLine::reset()
{
    writePosition = 0;
}

void DelayLine::processSample(float& sample, float* delayData, int delayBufferSize, double sampleRate, std::vector<float> delays, int& writePos, float baseDelay, float mix)
{
    
    
    float averageDelay = (delays[0] + delays[1] + delays[2]) /3.0f;
    updateCounter++;
    if (updateCounter >= 2048)
    {
        filter.updateCoeff(averageDelay);
        updateCounter = 0;
    }
    // Store clean input
    float inputSample = sample;

    float delaySum = 0;
    for (int i = 0; i < delays.size(); i++)
    {
        float delaySamplesFloat = calculateDelayFloat(delays.at(i), sampleRate);

        // Calculate read position for linear interpolation
        float currentReadPosition = static_cast<float>(writePos) - delaySamplesFloat;
        
        // Read delayed sample with interpolation
        float delayedSample = linearInterpolation(delayData, currentReadPosition, delayBufferSize);
  
          delaySum += delayedSample;
        
    }
    delaySum = delaySum / 3.0f;
    
    // write input to delay line
    delayData[writePos] = inputSample;
    delaySum = filter.biquadFilter(delaySum) * 1.5;
//    delaySum = filter.asymmetricSaturation(delaySum, 2.0f, 1.0f);
//    delaySum *= 0.3f;
    
    // Mix dry and wet
    sample = (inputSample * (1.0 - mix)) + (delaySum * mix);
//    sample = (inputSample) + (delaySum * mix);
    
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

float DelayLine::allPassInterpolation(float* buffer, float readPosition, int bufferSize, float baseDelay)
{

    
    while (readPosition < 0.0f)
        readPosition += bufferSize;
    
    while (readPosition >= bufferSize)
    {
        readPosition -= bufferSize;
    }
    
    int index1 = static_cast<int>(readPosition);
    int index2 = (index1 + 1) % bufferSize;
    index1 = index1 % bufferSize;
    
    float fraction = readPosition - static_cast<float>(index1);
    fraction = juce::jlimit(0.0f, 1.0f, fraction);
    
    //First-Order Allpass Interpolation
    // (y(n)= eta * x(n)+x(n-1)-eta * y(n-1)
    // y(n) = current output sample
    // x(n) = current input sample
    // x(n-1) = previous input sample
    // y(n-1) = previous output sample
    // eta = allpass coefficient:
    // 1 - d
    // ------
    // 1 + d
    // d = desired delay
    float eta = ((1 - baseDelay) / (1 + baseDelay));
    float result = buffer[writePosition] * eta + buffer[writePosition - 1] - eta * buffer[index1];
    
    return result;
}
void DelayLine::setFeedback(float newFeedback)
{
    // Clamp it for stability (never goes to 1.0 or higher)
    feedback = juce::jlimit (0.0f, 0.9f, newFeedback);
}
    // Fractional Delay
    // y(n) = x(n - D)
void DelayLine::fractionalDelay(float* delayChannelData, float* bufferChannelData, int bufferSize, int delayBufferSize, float delaySamples)
{
    for (int sample = 0; sample < bufferSize; ++sample)
    {
        // Store clean input
        float inputSample = bufferChannelData[sample];
        
        // Calculate read position in float for linear interpolation
        float currentReadPosition = static_cast<float>(writePosition - delaySamples);
        
       // Use linear interpolation function to read delayed sample
       // x(n-M) = input signal from M units in the past (delayedSample)
        float delayedSample = linearInterpolation(delayChannelData, currentReadPosition, delayBufferSize);
        
        // Write to delay line
        // y(n) = x(n - D)
        // x(n) = current sample
        // D = delay in sample
        delayChannelData[writePosition] = inputSample;
        
        
        // Add delayed sample with input sample to get output and 50% dry, 50% wet
        bufferChannelData[sample] = (inputSample * 0.5f) + (delayedSample * 0.5f);
        
        //Increment write position
        writePosition = (writePosition + 1) % delayBufferSize;
    }
}

void DelayLine::setDelayTime(float newDelayTime)
{
    delayTime = juce::jlimit (0.015f, 0.025f, newDelayTime);
}

float DelayLine::getDelayTime()
{
    return delayTime;
}

float DelayLine::calculateDelayFloat(float totalDelay, double sampleRate)
{
    float delayedSamplesFloat = totalDelay * sampleRate;
    return delayedSamplesFloat;
}


