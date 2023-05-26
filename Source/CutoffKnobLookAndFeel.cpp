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
    // DRAW THE BACKGROUND SVG
    std::unique_ptr<juce::Drawable> knobBackground;
    FilterMode currentFilterMode;

    // Determine the operating mode of the filter - bypass, lowpass or highpass 
    // Filte is bypassed 5 points either side of halfway, replace this with preprocessor #define FILTER_DEAD_ZONE 
    auto currentSliderValue = slider.getValue();
    if (currentSliderValue < -FILTER_DEAD_ZONE) {
        currentFilterMode = FilterMode::lowpass;
        knobBackground = juce::Drawable::createFromImageData(BinaryData::CutoffKnobLowpassBG_svg,
                                                                   BinaryData::CutoffKnobLowpassBG_svgSize);
    } else if (currentSliderValue > FILTER_DEAD_ZONE) {
        currentFilterMode = FilterMode::highpass;
        knobBackground = juce::Drawable::createFromImageData(BinaryData::CutoffKnobHighpassBG_svg,
                                                                   BinaryData::CutoffKnobHighpassBG_svgSize);
    } else {
        currentFilterMode = FilterMode::bypass;
        knobBackground = juce::Drawable::createFromImageData(BinaryData::CutoffKnobBypassBG_svg,
                                                                   BinaryData::CutoffKnobBypassBG_svgSize);
    }

    if (knobBackground)
    {
        knobBackground->drawWithin(g, juce::Rectangle<float>(x, y, width, height),
                                   juce::RectanglePlacement::centred, 1.0f);
    }
    // END SVG
    
    
    // // DRAW THE BACKGROUND PNG
    // juce::Image knobBackground;
    // FilterMode currentFilterMode;

    // // Determine the operating mode of the filter - bypass, lowpass, or highpass
    // // Filter is bypassed 5 points either side of halfway, replace this with preprocessor #define FILTER_DEAD_ZONE 
    // auto currentSliderValue = slider.getValue();
    // if (currentSliderValue < -FILTER_DEAD_ZONE) {
    //     currentFilterMode = FilterMode::lowpass;
    //     knobBackground = juce::ImageCache::getFromMemory(BinaryData::CutoffKnobLowpassBG_png,
    //                                                     BinaryData::CutoffKnobLowpassBG_pngSize);
    // } else if (currentSliderValue > FILTER_DEAD_ZONE) {
    //     currentFilterMode = FilterMode::highpass;
    //     knobBackground = juce::ImageCache::getFromMemory(BinaryData::CutoffKnobHighpassBG_png,
    //                                                     BinaryData::CutoffKnobHighpassBG_pngSize);
    // } else {
    //     currentFilterMode = FilterMode::bypass;
    //     knobBackground = juce::ImageCache::getFromMemory(BinaryData::CutoffKnobBypassBG_png,
    //                                                     BinaryData::CutoffKnobBypassBG_pngSize);
    // }

    // if (!knobBackground.isNull())
    // {
    //     // Scale the image to fit the desired dimensions
    //     float scaleX = width / (float)knobBackground.getWidth();
    //     float scaleY = height / (float)knobBackground.getHeight();
    //     float scale = std::min(scaleX, scaleY);
        
    //     juce::Rectangle<float> targetBounds(x, y, knobBackground.getWidth() * scale, knobBackground.getHeight() * scale);
    //     targetBounds = targetBounds.withCentre(juce::Point<float>(x + width / 2.0f, y + height / 2.0f));
    //     g.drawImage(knobBackground, targetBounds);
    // }

    // /// END PNG


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
    float rotationAngle = (sliderPos - 0.5f) * knobRange; // sliderPos - 0.5f ranges from -0.5 to 0.5, so this gives a range from -3*pi/4 to 3*pi/4
    
    // DRAW THE INDICATOR BARS AROUND THE PERIMETER OF THE DIAL
    int numOfBarsPerSide = 25; 
    float barWidth = 5.0f;
    float barHeight = 25.0f;
    float barCornerRadius = 2.0f;
    float radius = width - rotWidth - barHeight + 13; // Tuned by eye to fit the dots in the right place
    // float angleStep = knobRange / (numOfBarsPerSide - 2); 

    // The start and end of the angles for different filter modes
    auto drawBars = [&](float startAngle, float endAngle, FilterMode mode) {
        float angleStep = (endAngle - startAngle) / (numOfBarsPerSide - 1);

        for (int i = 0; i < numOfBarsPerSide; ++i) {
            float barAngle = startAngle + i * angleStep;
            float adjustedDotAngle = barAngle - juce::MathConstants<float>::pi / 2;

            int offsetY = -10;
            int offsetX = 0;
            int dotX = offsetX + x + width / 2 + radius * std::cos(adjustedDotAngle) - barWidth / 2;
            int dotY = offsetY + y + height / 2 + radius * std::sin(adjustedDotAngle) - barWidth / 2;

            // Draw the background bar
            juce::Path roundedRectangle;
            roundedRectangle.addRoundedRectangle(dotX, dotY, barWidth, barHeight, barCornerRadius);
            juce::AffineTransform rotation = juce::AffineTransform::rotation(barAngle, dotX + barWidth / 2, dotY + barHeight / 2);
            g.setColour(juce::Colour::fromString("ff25293B"));
            g.fillPath(roundedRectangle, rotation);
            
            // Draw the active bar overlay if the bar is between the center and the current angle
            // Add a tiny bit on to ensure the last bar lights up
            bool isActive = (mode == FilterMode::lowpass && barAngle > rotationAngle - 0.1f) ||
                            (mode == FilterMode::highpass && barAngle < rotationAngle + 0.1f) ||
                            (mode == FilterMode::bypass);

            if (isActive) {
                // Define the glowing color.
                juce::Colour activeColor; 
                switch (mode) {
                    case FilterMode::bypass:
                        if (i % 8 == 0)
                            // activeColor = juce::Colours::white.withAlpha(0.4f);
                            activeColor = juce::Colours::white.withAlpha(0.6f);
                        else
                            activeColor = juce::Colour::fromString("ffB4181D").withAlpha(0.6f);
                            
                        break;
                    case FilterMode::lowpass:
                        activeColor = juce::Colour::fromString("ff0BA6D8");
                        break;
                    case FilterMode::highpass:
                        activeColor = juce::Colour::fromString("ff34D846");
                        break;
                }

                // Draw the active style of the bar
                juce::Path activeRoundedRectangle;
                activeRoundedRectangle.addRoundedRectangle(dotX+1, dotY+1, barWidth-2, barHeight-2, barCornerRadius);
                g.setColour(activeColor);
                g.fillPath(activeRoundedRectangle, rotation);
            }
        }
    };

    // The start and end of the angles for different filter modes
    float lowpassStartAngle = -3 * juce::MathConstants<float>::pi / 4;
    float lowpassEndAngle = -juce::MathConstants<float>::pi / 7;

    float highpassStartAngle = juce::MathConstants<float>::pi / 7;
    float highpassEndAngle = 3 * juce::MathConstants<float>::pi / 4;

    drawBars(lowpassStartAngle, lowpassEndAngle, currentFilterMode);
    drawBars(highpassStartAngle, highpassEndAngle, currentFilterMode);

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
        knobRotating->setTransform(knobRotating->getTransform().rotated(rotationAngle, width * 0.5f, height * 0.5f));

        // Render Drawable into an Image
        juce::Image image(juce::Image::ARGB, knobRotating->getBounds().getWidth(), knobRotating->getBounds().getHeight(), true);
        juce::Graphics graphics(image);
        knobRotating->draw(graphics, 1.0f);

        // Apply DropShadowEffect
        shadowEffect.applyEffect(image, g, 1.0f, 1.0f);
    }

}
