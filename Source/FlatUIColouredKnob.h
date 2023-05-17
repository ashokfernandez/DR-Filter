/*
  ==============================================================================

    FlatUIColouredKnob.h
    Created: 14 May 2023 11:00:42pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FlatUIColouredKnob : public juce::LookAndFeel_V4
{
public:
    FlatUIColouredKnob(juce::Colour knobMarkerColour);
    ~FlatUIColouredKnob() override;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;


private:
    juce::Colour knobMarkerColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlatUIColouredKnob)
};
