# Chorus

Stereo chorus built with JUCE

### Built With

JUCE v8.0.7

## Overview

This chorus is inspird by classic BBD chorus pedals. There is lowpass filtering to simulate BBD reconstruction using a Biquad filter. There are stereo width controls that adjust the phase offset of the right and left channels. 

## Usage

For more examples, please refer to the [Documentation](https://github.com/aejaudio/chorusPlugin1/tree/main/Chorus%20Documents)


## Features
- 6 voices, 3 for each channel 
- Stereo width control
- Custom delay line
- Adjust rate, depth, mix, and width parameters



## Additional Notes
- This plugin is built around a custom Chorus class that handles the DSP independent of the PluginProcessor. 