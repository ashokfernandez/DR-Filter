/*
  ==============================================================================

    CustomWaveshaper.h
    Created: 6 May 2023 3:20:06pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CustomWaveShaper : public juce::dsp::WaveShaper<float>
{
public:
    CustomWaveShaper(juce::AudioProcessorValueTreeState& apvts);

private:
    static juce::AudioProcessorValueTreeState* sApvts;
    static float saturationFunction(float x);
};