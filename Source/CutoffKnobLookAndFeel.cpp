/*
  ==============================================================================

    CutoffKnobLookAndFeel.cpp
    Created: 17 May 2023 7:38:07pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#include "CutoffKnobLookAndFeel.h"

CutoffKnobLookAndFeel::CutoffKnobLookAndFeel()
{
}

CutoffKnobLookAndFeel::~CutoffKnobLookAndFeel()
{
}

void CutoffKnobLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    // DRAW THE BACKGROUND

    // Determine the operating mode of the filter - bypass, lowpass or highpass 
    // Filte is bypassed 5 points either side of halfway, replace this with preprocessor #define FILTER_DEAD_ZONE 
    auto filterDeadZone = 5.0f;
    auto currentSliderValue = slider.getValue();
    std::unique_ptr<juce::Drawable> knobBackground;
    
    if (currentSliderValue < -filterDeadZone) {
        knobBackground = juce::Drawable::createFromImageData(BinaryData::CutoffKnobLowpassBG_svg,
                                                                   BinaryData::CutoffKnobLowpassBG_svgSize);
    } else if (currentSliderValue > filterDeadZone) {
        knobBackground = juce::Drawable::createFromImageData(BinaryData::CutoffKnobHighpassBG_svg,
                                                                   BinaryData::CutoffKnobHighpassBG_svgSize);
    } else {
        knobBackground = juce::Drawable::createFromImageData(BinaryData::CutoffKnobBypassBG_svg,
                                                                   BinaryData::CutoffKnobBypassBG_svgSize);
    }

    if (knobBackground)
    {
        knobBackground->drawWithin(g, juce::Rectangle<float>(x, y, width, height),
                                   juce::RectanglePlacement::centred, 1.0f);
    }

    // Calculate scale ratio for shrinking the rotating layer on top of the background 
    // comes from the size of the two layers in the Figma designs
    float rotationLayerWidth = 266.0f;
    float backgroundLayerWidth = 488.0f;
    float scaleRatio = rotationLayerWidth / backgroundLayerWidth;

    int rotWidth = int(width * scaleRatio);
    int rotHeight = int(height * scaleRatio);
    int offsetX = (width - rotWidth) / 2;
    int offsetY = (height - rotHeight) / 2;

    // Calculate rotation for the rotating layer
    float knobRange = 3 * juce::MathConstants<float>::pi / 2; // range from -3*pi/4 to 3*pi/4 is 1.5*pi
    float customAngle = (sliderPos - 0.5f) * knobRange; // sliderPos - 0.5f ranges from -0.5 to 0.5, so this gives a range from -3*pi/4 to 3*pi/4

    
    
    // DRAW THE INDICATOR DOTS bAROUND THE PERMIETER OF THE DIAL

    // Draw indicator dots
    int numOfDots = 33; // variable to hold the number of dots
    float dotDiameter = 10; // diameter of the dots
    float radius = width - rotWidth - dotDiameter - 11; // Tuned by eye to fit the dots in the right place
    float angleStep; // calculate the angular step for each dot
    
    // In the special case of three dots, we need to ensure it's placed inbetween the two range indicator dots
    if (numOfDots == 3) {
        angleStep = knobRange / 2;
    } else {
        angleStep = knobRange / (numOfDots - 2);
    }
    
    // Indicator dots around the perimeter of the dial 
    for (int i = 0; i < numOfDots; ++i)
    {
        // Calculate angle for each dot
        float dotAngle;
        if (i == 0) {
            dotAngle = -3 * juce::MathConstants<float>::pi / 4;
        } else if (i == numOfDots - 1) {
            dotAngle = (3 * juce::MathConstants<float>::pi / 4) - 0.00001; // Take a tiny bit off so the final dot lights up
        } else {
            dotAngle = -3 * juce::MathConstants<float>::pi / 4 + (i * angleStep);
        }

        // Adjust the angle due to the coordinate system of JUCE
        float adjustedDotAngle = dotAngle - juce::MathConstants<float>::pi / 2;

        // Calculate dot position
        int dotX = x + width / 2 + radius * std::cos(adjustedDotAngle) - dotDiameter / 2;
        int dotY = y + height / 2 + radius * std::sin(adjustedDotAngle) - dotDiameter / 2;

        // Background dot
        juce::Colour backgroundColor = juce::Colour::fromString("ff25293B");
        g.setColour(backgroundColor);
        g.fillEllipse(dotX, dotY, dotDiameter, dotDiameter);
        g.setColour(backgroundColor.darker(0.25f));
        g.drawEllipse(dotX, dotY, dotDiameter, dotDiameter, 1.0f);
        
        // Draw the active dot style if the dot is behind the current angle of the knob
        if (dotAngle < customAngle)
        {
            juce::Colour glowColor = juce::Colours::yellow.withAlpha(0.3f);
            juce::Colour innerGlowColor = juce::Colour::fromString("fff8f4ff").withAlpha(0.3f);

            // Draw the inner part of the glowing dot
            g.setColour(juce::Colours::yellow.withAlpha(0.3f));
            g.fillEllipse(dotX + 1, dotY + 1, dotDiameter - 2, dotDiameter - 2);
            
            // Draw glow effect around inner part
            float glowSize = 0.9f * dotDiameter;
            g.setColour(glowColor);
            g.drawEllipse(dotX - (glowSize - dotDiameter) / 2, dotY - (glowSize - dotDiameter) / 2, glowSize, glowSize, 1.0f);
            g.setColour(innerGlowColor);
            g.drawEllipse(dotX + 1, dotY + 1, dotDiameter - 2, dotDiameter - 2, 1.0f);
        }
    }



    // DRAW THE INNER ROTATABLE DIAL, WITH A DROP SHADOW

    // Create a DropShadow
    juce::DropShadow dropShadow;
    dropShadow.colour = juce::Colours::black.withAlpha(0.8f); 
    dropShadow.radius = 10;
    dropShadow.offset = { 0, 7 }; // offset in x and y direction

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

        // Render Drawable into an Image
        juce::Image image(juce::Image::ARGB, knobRotating->getBounds().getWidth(), knobRotating->getBounds().getHeight(), true);
        juce::Graphics graphics(image);
        knobRotating->draw(graphics, 1.0f);

        // Apply DropShadowEffect
        shadowEffect.applyEffect(image, g, 1.0f, 1.0f);
    }

}
