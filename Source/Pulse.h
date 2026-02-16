#pragma once
#include <JuceHeader.h>

class Pulse
{
public:
    Pulse(){}
    ~Pulse(){}
    void prepare(double position, double bpm, bool isPlaying);
    void setBPM(double bpm) { currentBPM = bpm; }
    void setPosition(double position) { currentPosition = position; }
    void setIsPlaying(bool isPlaying) { currentIsPlaying = isPlaying; }
    double getPosition() { return currentPosition; }
private:
    bool on = true;
    bool off = false;
    
    double currentBPM = 120.0;
    double currentPosition = 0.0;
    bool currentIsPlaying = false;
};
