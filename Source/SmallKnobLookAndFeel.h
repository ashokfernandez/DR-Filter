/*
  ==============================================================================

    SmallKnobLookAndFeel.h
    Created: 17 May 2023 3:32:06pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SmallKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SmallKnobLookAndFeel(juce::Colour knobAccentColour);
    ~SmallKnobLookAndFeel() override;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;


private:
    juce::Colour knobAccentColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SmallKnobLookAndFeel)
};
