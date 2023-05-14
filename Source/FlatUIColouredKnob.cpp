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
    const float radius = juce::jmin(width / 2.0f, height / 2.0f);
    const float centerX = x + width * 0.5f;
    const float centerY = y + height * 0.5f;
    const float markerLength = radius * 0.45f;  // 45% of the radius
    const float markerStartDistance = radius;

    // Define start and end angles in radians (-135 degrees to 135 degrees)
    const float rotaryStartAngleInRad = juce::degreesToRadians(-225.0f);
    const float rotaryEndAngleInRad = juce::degreesToRadians(45.0f);

    // Map sliderPos (0 to 1) to rotaryStartAngle to rotaryEndAngle (-135 to 135 degrees in radians)
    const float rotation = juce::jmap(sliderPos, 0.0f, 1.0f, rotaryStartAngleInRad, rotaryEndAngleInRad);

    // Knob Background
    juce::Path knobBackground;
    knobBackground.addEllipse(centerX - radius, centerY - radius, 2 * radius, 2 * radius);
    g.setColour(juce::Colour(81, 81, 81));
    g.fillPath(knobBackground);

    // Knob Marker
    juce::Path knobMarker;
    knobMarker.startNewSubPath(centerX + markerStartDistance * std::cos(rotation),
                               centerY + markerStartDistance * std::sin(rotation));
    knobMarker.lineTo(centerX + (markerStartDistance - markerLength) * std::cos(rotation),
                      centerY + (markerStartDistance - markerLength) * std::sin(rotation));
    g.setColour(juce::Colours::red); // set your preferred color
    g.strokePath(knobMarker, juce::PathStrokeType(2.0f)); // the width of the line is 2.0f
}
