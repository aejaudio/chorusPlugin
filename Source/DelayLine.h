
#pragma once
#include <JuceHeader.h>

class Chorus;
class DelayLine
{
public:
    
    DelayLine(juce::AudioProcessorValueTreeState& state) : apvts (state)
    {
    }

    ~DelayLine();
    void prepare(double sampleRate, int size);
    void processBlock(float* bufferData, float* delayData, int bufferSize, int delayBufferSize, int sampleRate, float totalDelay);
    void setDelayTime(float newDelayTime);
    void reset();
    void setFeedback(float newFeedback);
    float getFeedback() const noexcept { return feedback;}
    int getDelayBufferSize();
    int delayGetNumSamples();
    float getDelayTime();
    void calculateTotalDelay();
    void processSample(float& sample, float* delayData, int delayBufferSize, double sampleRate, std::vector<float> delays, int& writePos, float baseDelay, float mix);
    float calculateDelayFloat(float totalDelay, double sampleRate);
    
private:
    
    juce::AudioProcessorValueTreeState& apvts;
    void fillBuffer(juce::AudioBuffer<float> &buffer, juce::AudioBuffer<float> &delayBuffer, int channel, float* channelData, int bufferSize, int delayBufferSize);
    void readFromBuffer(juce::AudioBuffer<float> &buffer, juce::AudioBuffer<float> &delayBuffer,int channel, float* channelData, int bufferSize, int delayBufferSize);
    
    void fractionalDelay(float* delayChannelData, float* bufferChannelData, int bufferSize, int delayBufferSize, float delaySamples);
    
    // Linear Interpolation
    // y = (1 - t)y₀ + ty₁
    float linearInterpolation(float* buffer, float readPosition, int bufferSize);
    
    float allPassInterpolation(float* buffer, float readPosition, int bufferSize, float baseDelay);
    
    int writePosition { 0 };
    float feedback = { 0.0f }; // g
    float delaySamples { 0.0f };
    int readPosition { 0 };
    float delayTime = { 0.015f };
    float totalDelay = 0.0f;
    

};
