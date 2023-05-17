/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
DRFilterAudioProcessorEditor::DRFilterAudioProcessorEditor(DRFilterAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      resonanceLookAndFeel(juce::Colours::blue),
      driveLookAndFeel(juce::Colours::red)
{

    // Set up the knobs
    cutoffKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    cutoffKnob.setRange(0.0, 1.0, 0.01);
    cutoffKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    cutoffKnob.setLookAndFeel(&cutoffLookAndFeel);  // set the look and feel
    addAndMakeVisible(cutoffKnob);

    resonanceKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    resonanceKnob.setRange(0.0, 1.0, 0.01);
    resonanceKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    resonanceKnob.setLookAndFeel(&resonanceLookAndFeel);  // set the look and feel
    addAndMakeVisible(resonanceKnob);

    driveKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveKnob.setRange(0.0, 1.0, 0.01);
    driveKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    driveKnob.setLookAndFeel(&driveLookAndFeel);  // set the look and feel
    addAndMakeVisible(driveKnob);

    // Set up the labels
    cutoffLabel.setText("Cutoff", juce::dontSendNotification);
    cutoffLabel.attachToComponent(&cutoffKnob, false);
    cutoffLabel.setJustificationType(juce::Justification::centred);

    resonanceLabel.setText("Resonance", juce::dontSendNotification);
    resonanceLabel.attachToComponent(&resonanceKnob, false);
    resonanceLabel.setJustificationType(juce::Justification::centred);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent(&driveKnob, false);
    driveLabel.setJustificationType(juce::Justification::centred);

    // To import custom fonts they have to be included as binary data, then you import the type face 
    // and create a font from it. Then you can set the font of the label to the custom font.

    // Side effects label
    balooTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Baloo2ExtraBold_ttf, BinaryData::Baloo2ExtraBold_ttfSize);
    float fontSize = 30.0f; 
    juce::Font balooFont = juce::Font(balooTypeface).withHeight(fontSize);
    sideEffectsLabel.setFont(balooFont);
    sideEffectsLabel.setText("side effects", juce::dontSendNotification);
    addAndMakeVisible(sideEffectsLabel);

    // DR Filter label 
    righteousTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::RighteousRegular_ttf, BinaryData::RighteousRegular_ttfSize);
    fontSize = 50.0f;
    juce::Font righteousFont = juce::Font(righteousTypeface).withHeight(fontSize);
    drFilterLabel.setFont(righteousFont);
    drFilterLabel.setText("DR Filter", juce::dontSendNotification);
    addAndMakeVisible(drFilterLabel);



    // Create the AudioProcessorValueTreeState::SliderAttachment objects
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Cutoff", cutoffKnob);
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Resonance", resonanceKnob);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Drive", driveKnob);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 420);
}

DRFilterAudioProcessorEditor::~DRFilterAudioProcessorEditor()
{
    cutoffKnob.setLookAndFeel(nullptr);     // clear the look and feel
    resonanceKnob.setLookAndFeel(nullptr);  // clear the look and feel
    driveKnob.setLookAndFeel(nullptr);      // clear the look and feel
}

//==============================================================================
void DRFilterAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Create the gradient
    juce::Colour startColour = juce::Colour::fromString("FF5E5F7C");
    juce::Colour endColour = juce::Colour::fromString("FF33374B");
    juce::ColourGradient gradient(startColour, 0, 0, endColour, getWidth(), getHeight(), false);

    // Set the fill type of the Graphics object
    g.setFillType(gradient);

    // Fill the component's bounds with the gradient
    g.fillRect(getLocalBounds());

    // Create a rectangle
    juce::Rectangle<float> rect = getLocalBounds().reduced(WINDOW_PADDING_PX).toFloat();

    // Set colour and opacity
    juce::Colour rectColour = juce::Colour::fromString("FF22273B").withAlpha(0.45f);

    // Set fill colour for the rectangle
    g.setColour(rectColour);

    // Draw the rectangle with rounded corners
    g.fillRoundedRectangle(rect, 10.0f);
}




void DRFilterAudioProcessorEditor::resized()
{
    int margin = COMPONENT_MARGIN_PX; 
    int border = WINDOW_PADDING_PX; 

    // Get the label sizes
    int drFilterLabelWidth = drFilterLabel.getFont().getStringWidth(drFilterLabel.getText());
    int drFilterLabelHeight = drFilterLabel.getFont().getHeight();
    int sideEffectsLabelWidth = sideEffectsLabel.getFont().getStringWidth(sideEffectsLabel.getText());
    int sideEffectsLabelHeight = sideEffectsLabel.getFont().getHeight();

    // Place the drFilterLabel in the top right, respecting the border
    int drFilterShiftX = -5;
    int drFilterShiftY = 0;
    drFilterLabel.setBounds(getWidth() - drFilterLabelWidth - border + drFilterShiftX, border + drFilterShiftY, drFilterLabelWidth, drFilterLabelHeight);

    // Place the sideEffectsLabel in the top left, respecting the border
    int sideEffectsLabelShiftX = 7;
    int sideEffectsLabelShiftY = 3;
    sideEffectsLabel.setBounds(border + sideEffectsLabelShiftX, border + sideEffectsLabelShiftY, sideEffectsLabelWidth, sideEffectsLabelHeight);

    // Place the cutoffKnob as a main feature taking 200 x 200px on the left below the labels, respecting the border and margin
    int cutoffKnobSize = 300;
    cutoffKnob.setBounds(border, std::max(drFilterLabelHeight, sideEffectsLabelHeight) + border + margin, cutoffKnobSize, cutoffKnobSize);

    // Place Drive and resonance to the right of the cutoff knob, taking 100x 100 px stacked on top of each other, respecting the border and margin
    int knobSize = 150;
    driveKnob.setBounds(cutoffKnobSize + border + margin, std::max(drFilterLabelHeight, sideEffectsLabelHeight) + border + margin, knobSize, knobSize);
    resonanceKnob.setBounds(cutoffKnobSize + border + margin, std::max(drFilterLabelHeight, sideEffectsLabelHeight) + knobSize + border + 2 * margin, knobSize, knobSize);
}
