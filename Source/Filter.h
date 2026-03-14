

#pragma once
#include <JuceHeader.h>
#include <cmath>

class Filter
{
public:
    
    Filter();
    ~Filter(){};
    void prepare(double currentSampleRate, float currentDelayTime);
    float biquadFilter(float inputSample);
    void updateCoeff(float delayTime);
    void reset();
    
    float asymmetricSaturation(float input, float driveAdd, float driveSubtract);
    float saturate(float input, float drive);
    void calculate(float cutoff);
    
private:
    
    double sampleRate;
    
    float numStages = 1024;
    float maxDelay = 0.04f;
    float stageTime = maxDelay / numStages;
    
    // As delay time modulates
    float currentStages;
    float baseCutOff = 12000.0f;
    float cutoff;
    float pi = juce::MathConstants<float>::pi;
    float w0;
    
    float alpha;;
    
    float b0;
    float b1;
    float b2;
    float a0;
    float a1;
    float a2;
    
    
    
    float b0_smooth;
    float b1_smooth;
    float b2_smooth;
    float a1_smooth;
    float a2_smooth;
    float smoothing = 0.999f;
    
    float x1 = 0.0f;
    float x2 = 0.0f;
    float y1 = 0.0f;
    float y2 = 0.0f;
    const float Q = 0.5f;
    
    float drive = 3.0f;
    float scaled = 0.0f;
    float saturated = 0.0f;

    
    float delayTime;

    

//
//    double b0 = 0.00002808162654240159;
//    double b1 = 0.00324260184959550063;
//    double b2 = 0.01229358405255225224;
//    
//    double a1 = 1.00000000000000000000;
//    double a2 = 0.27002485476511250972;

};
