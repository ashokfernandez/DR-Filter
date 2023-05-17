/*
  ==============================================================================

    CutoffKnobLookAndFeel.h
    Created: 17 May 2023 7:38:07pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class CutoffKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CutoffKnobLookAndFeel();
    ~CutoffKnobLookAndFeel() override;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;


private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CutoffKnobLookAndFeel)
};

