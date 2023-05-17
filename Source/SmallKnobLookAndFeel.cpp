/*
  ==============================================================================

    SmallKnobLookAndFeel.cpp
    Created: 17 May 2023 3:32:06pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#include "SmallKnobLookAndFeel.h"

SmallKnobLookAndFeel::SmallKnobLookAndFeel(juce::Colour knobAccentColour)
    : knobAccentColour(knobAccentColour)
{
}

SmallKnobLookAndFeel::~SmallKnobLookAndFeel()
{
}

void SmallKnobLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    // Draw background layer
    auto knobBackground = juce::Drawable::createFromImageData(BinaryData::SmallKnobStaticBG_svg,
                                                               BinaryData::SmallKnobStaticBG_svgSize);
    if (knobBackground)
    {
        knobBackground->drawWithin(g, juce::Rectangle<float>(x, y, width, height),
                                   juce::RectanglePlacement::centred, 1.0f);
    }

    // Calculate scale ratio for shrinking the rotating layer on top of the background 
    // comes from the size of the two layers in the Figma designs
    float scaleRatio = 266.0f / 488.0f;
    int rotWidth = int(width * scaleRatio);
    int rotHeight = int(height * scaleRatio);
    int offsetX = (width - rotWidth) / 2;
    int offsetY = (height - rotHeight) / 2;

    // Calculate rotation for the rotating layer
    float customRange = 3 * juce::MathConstants<float>::pi / 2; // range from -3*pi/4 to 3*pi/4 is 1.5*pi
    float customAngle = (sliderPos - 0.5f) * customRange; // sliderPos - 0.5f ranges from -0.5 to 0.5, so this gives a range from -3*pi/4 to 3*pi/4

    // Create a DropShadow
    juce::DropShadow dropShadow;
    dropShadow.colour = juce::Colours::black.withAlpha(0.5f);
    dropShadow.radius = 5;
    dropShadow.offset = { 0, 3 };

    // Create a DropShadowEffect
    juce::DropShadowEffect shadowEffect;
    shadowEffect.setShadowProperties(dropShadow);



    // Draw rotating layer
    auto knobRotating = juce::Drawable::createFromImageData(BinaryData::SmallKnobRotatable_svg,
                                                             BinaryData::SmallKnobRotatable_svgSize);
    if (knobRotating)
    {
        knobRotating->setTransformToFit(juce::Rectangle<float>(x + offsetX, y + offsetY, rotWidth, rotHeight), juce::RectanglePlacement::centred);
        knobRotating->setTransform(knobRotating->getTransform().rotated(customAngle, width * 0.5f, height * 0.5f));


        // Apply DropShadowEffect and draw
        shadowEffect.applyEffect(*knobRotating, g, 1.0f, 1.0f);
        // knobRotating->draw(g, 1.0f);
    }
}
