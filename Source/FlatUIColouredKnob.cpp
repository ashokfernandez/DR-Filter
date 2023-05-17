/*
  ==============================================================================

    FlatUIColouredKnob.cpp
    Created: 14 May 2023 11:00:42pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#include "FlatUIColouredKnob.h"

FlatUIColouredKnob::FlatUIColouredKnob(juce::Colour knobMarkerColour)
    : knobMarkerColour(knobMarkerColour)
{
}

FlatUIColouredKnob::~FlatUIColouredKnob()
{
}

void FlatUIColouredKnob::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                                          juce::Slider& slider)
{
    // Create Drawable from SVG
    auto svgData = BinaryData::FlatKnobRotatable_svg;
    auto svgSize = BinaryData::FlatKnobRotatable_svgSize;
    auto svgDrawable = juce::Drawable::createFromImageData(svgData, svgSize);

    if (svgDrawable == nullptr) return;  // Make sure the SVG loaded correctly

    const float centerX = x + width * 0.5f;
    const float centerY = y + height * 0.5f;
    const float radius = juce::jmin(width / 2.0f, height / 2.0f);

    const float rotaryStartAngleInRad = juce::degreesToRadians(rotaryStartAngle);
    const float rotaryEndAngleInRad = juce::degreesToRadians(rotaryEndAngle);

    const float rotation = juce::jmap(sliderPos, 0.0f, 1.0f, rotaryStartAngleInRad, rotaryEndAngleInRad);

    // Adjust these values to create a smaller rectangle for the SVG
    float svgMarginX = width * 0.2f;
    float svgMarginY = height * 0.2f;

    // The SVG will fit within this smaller rectangle
    juce::Rectangle<float> svgRect(x + svgMarginX, y + svgMarginY, width - 2 * svgMarginX, height - 2 * svgMarginY);

    // Rotate the SVG and scale it to fit the smaller rectangle
    juce::AffineTransform transform;
    transform = transform.rotated(rotation, centerX, centerY);
    transform = transform.scaled(svgRect.getWidth() / svgDrawable->getDrawableBounds().getWidth(),
                                 svgRect.getHeight() / svgDrawable->getDrawableBounds().getHeight());
    svgDrawable->setTransform(transform);
    
    // Draw the SVG
    svgDrawable->draw(g, 1.0f);
}

    
    
    // const float radius = juce::jmin(width / 2.0f, height / 2.0f);
    // const float centerX = x + width * 0.5f;
    // const float centerY = y + height * 0.5f;
    // const float markerLength = radius * 0.70f;
    // const float markerStartDistance = radius;
    // const float lineThickness = 5.0f;

    // const float rotaryStartAngleInRad = juce::degreesToRadians(-225.0f);
    // const float rotaryEndAngleInRad = juce::degreesToRadians(45.0f);

    // const float rotation = juce::jmap(sliderPos, 0.0f, 1.0f, rotaryStartAngleInRad, rotaryEndAngleInRad);

    // // Knob Background
    // juce::Path knobBackground;
    // knobBackground.addEllipse(centerX - radius, centerY - radius, 2 * radius, 2 * radius);
    // g.setColour(juce::Colour(81, 81, 81));
    // g.fillPath(knobBackground);

    // // Knob Marker
    // juce::Path knobMarker;
    // knobMarker.startNewSubPath(centerX + markerStartDistance * std::cos(rotation),
    //                            centerY + markerStartDistance * std::sin(rotation));
    // knobMarker.lineTo(centerX + (markerStartDistance - markerLength) * std::cos(rotation),
    //                   centerY + (markerStartDistance - markerLength) * std::sin(rotation));
    // g.setColour(knobMarkerColour);
    // g.strokePath(knobMarker, juce::PathStrokeType(lineThickness));

    // // Start and End Range Markers
    // const float markerSize = lineThickness;  // Size of the square marker
    // const float markerRadius = radius + markerSize * 0.9f;  // The distance from the center to the marker
    // g.setColour(knobMarkerColour);

    // // Start marker
    // float startX = centerX + markerRadius * std::cos(rotaryStartAngleInRad) - markerSize / 2;
    // float startY = centerY + markerRadius * std::sin(rotaryStartAngleInRad) - markerSize / 2;
    // g.saveState();
    // g.addTransform(juce::AffineTransform::rotation(rotaryStartAngleInRad, startX + markerSize / 2, startY + markerSize / 2));
    // g.fillRect(startX, startY, markerSize, markerSize);
    // g.restoreState();

    // // End marker
    // float endX = centerX + markerRadius * std::cos(rotaryEndAngleInRad) - markerSize / 2;
    // float endY = centerY + markerRadius * std::sin(rotaryEndAngleInRad) - markerSize / 2;
    // g.saveState();
    // g.addTransform(juce::AffineTransform::rotation(rotaryEndAngleInRad, endX + markerSize / 2, endY + markerSize / 2));
    // g.fillRect(endX, endY, markerSize, markerSize);
    // g.restoreState();
// }
