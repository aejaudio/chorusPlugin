#Chorus Plugin - Ashley Jacobson

Multiband Chorus effect plugin built with JUCE.
- Currently no multiband implementation 

02/16/26 - 6 Voices

## Demo

Audio demo:
https://github.com/aejaudio/chorusPlugin1/blob/main/Chorus%20Documents/Chorus_021626_6Voices/Chorus_021626_6Voices.wav

Video walkthrough:

https://github.com/aejaudio/chorusPlugin1/blob/main/Chorus%20Documents/Chorus_021626_6Voices/Chorus_021626_6Voices_AudioVideo.mp4

## Current Progress

- Beginning stages of BBD chorus emulation
- Stereo width control with phase-offset LFO modulation
- Lowpass filtering to simulate BBD reconstruction using Biquad filter
- Asymmetric Saturation

## Parameters

### Rate
- **Range:** 0.1 - 5.0 Hz
- **Default:** 0.1 Hz
- **Description:** Controls the speed of the LFO modulation

### Depth
- **Range:** 0.001 - 0.008 (0-80ms)
- **Default:** 0.015 (15ms)
- **Description:** Controls how much the delay time varies

### Width
- **Range:** 1 - 10
- **Default:** 1
- **Description:** Controls stereo spread by adjusting LFO phase offset between voices


### Mix
- **Range:** 0% - 100%
- **Default:** 50%
- **Description:** Dry/wet balance

## Technical Details

### BBD Emulation
- **Base Delay:** 25ms (configurable)
- **Number of Stages:** 6
- **Filter Type:** 2nd-order Butterworth lowpass implemented as Biquad filter
- **Cutoff Calculation:** Dynamic based on delay time

### Signal Flow
```
Input → BBD Delay Stages → Lowpass Filter → Saturation → Mix → Output
```

### Architecture
- Separate filters for left/right channels
- Linear interpolation
- Coefficient smoothing


## Known Issues / Future Improvements

- [ ] Add feedback parameter
- [ ] Add voices parameter - Could be wiggle knob
- [ ] Add Multiband control
- [ ] Gain decreases when chorus engaged

## Credits

Built with JUCE Framework
Inspired by classic BBD chorus pedals
