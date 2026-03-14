

#include "Filter.h"
#include <fstream>
Filter::Filter()
{
    b0 = 1.0f;
    b1 = 0.0f;
    b2 = 0.0f;
    a1 = 0.0f;
    a2 = 0.0f;
    b0_smooth = b0;
    b1_smooth = b1;
    b2_smooth = b2;
    a1_smooth = a1;
    a2_smooth = a2;
}

void Filter::prepare(double currentSampleRate, float delayTime)
{   
    
    sampleRate = currentSampleRate;
    currentStages = delayTime / stageTime;
    
    cutoff = baseCutOff / std::sqrt(currentStages/ numStages);
    
    calculate(cutoff);

}

void Filter::reset()
{
    x1 = x2 = y1 = y2 = 0.0f;
}
// Biquad Filter
// y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
// x[n] = current input sample
// y[n] = current output sample
// x[n-1], x[n-2] = previous input samples
// y[n-1], y[n-2] = previous output samples
// b0, b1, b2 = feedforward coefficients
// a1, a2 = feedback coefficients

float Filter::biquadFilter(float inputSample)
{

  
    
    b0_smooth = b0_smooth * smoothing + b0 * (1.0f - smoothing);
    b1_smooth = b1_smooth * smoothing + b1 * (1.0f - smoothing);
    b2_smooth = b2_smooth * smoothing + b2 * (1.0f - smoothing);
    a1_smooth = a1_smooth * smoothing + a1 * (1.0f - smoothing);
    a2_smooth = a2_smooth * smoothing + a2 * (1.0f - smoothing);
    
    float output = ((b0_smooth * inputSample) + (b1_smooth * (x1)) + (b2_smooth * (x2)) - (a1_smooth * y1) - (a2_smooth * y2));
    
    output = std::clamp(output, -2.0f, 2.0f);
    
    if (std::abs(output) < 1e-15f)
    {
        output = 0.0f;
    }
    // Update state for next sample
    x2 = x1;
    x1 = inputSample;
    y2 = y1;
    y1 = output;


    return output;
}


void Filter::updateCoeff(float delayTime)
{
    
    currentStages = delayTime / stageTime;
    cutoff = baseCutOff / (std::sqrt(currentStages) / numStages);

    if (cutoff < 20.0f)
    {
        cutoff = 20.0f;
    }
    if (cutoff > sampleRate * 0.45f)
    {
        cutoff = sampleRate * 0.45f;
    }
    
    calculate(cutoff);
    
}

float Filter::asymmetricSaturation(float input, float driveAdd, float driveSubtract)
{
    if (input > 0.0f)
    {
        float output = saturate(input, driveAdd);
        return output;
    }
    else
    {
        float output = saturate(input, driveSubtract);
        return output;
    }
    
    
}
float Filter::saturate(float input, float drive)
{
    float output = std::tanh(input * drive) / drive;
    scaled = input * drive;
    saturated = tanh(scaled);
    output = saturated / drive;
    return output;
}

void Filter::calculate(float cutoff)
{
    w0 = 2 * pi * cutoff / sampleRate;
    alpha = std::sin(w0) / (2.0f * Q);
    b0 = (1- std::cos(w0)) / 2;
    b1 = 1 - std::cos(w0);
    b2 = (1 - std::cos(w0)) / 2;
    a0 = 1 + alpha;
    a1 = -2 * std::cos(w0);
    a2 = 1 - alpha;
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}
