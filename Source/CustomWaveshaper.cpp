/*
  ==============================================================================

    CustomWaveshaper.cpp
    Created: 6 May 2023 3:20:06pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#include "CustomWaveShaper.h"

juce::AudioProcessorValueTreeState* CustomWaveShaper::sApvts = nullptr;

CustomWaveShaper::CustomWaveShaper(juce::AudioProcessorValueTreeState& apvts)
{
    sApvts = &apvts;
    functionToUse = saturationFunction;
}

float CustomWaveShaper::saturationFunction(float x)
{
    auto drive = sApvts->getRawParameterValue("Drive")->load();
    auto saturationAmount = juce::jmap(drive, 0.0f, 100.0f, 1.0f, 50.0f);
    float saturated = std::atan(saturationAmount * x) / std::atan(saturationAmount);
    return saturated;
}