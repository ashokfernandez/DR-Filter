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
      cutoffLookAndFeel(juce::Colours::green),    // initialize with your desired colours
      resonanceLookAndFeel(juce::Colours::blue),
      driveLookAndFeel(juce::Colours::red)
{

    // Set up the knobs
    cutoffKnob.setSliderStyle(juce::Slider::Rotary);
    cutoffKnob.setRange(0.0, 1.0, 0.01);
    cutoffKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    cutoffKnob.setLookAndFeel(&cutoffLookAndFeel);  // set the look and feel
    addAndMakeVisible(cutoffKnob);

    resonanceKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    resonanceKnob.setRange(0.0, 1.0, 0.01);
    resonanceKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    resonanceKnob.setLookAndFeel(&resonanceLookAndFeel);  // set the look and feel
    addAndMakeVisible(resonanceKnob);

    driveKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
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

    // Side effects label
     // Create a Typeface from the font data.
    balooTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Baloo2ExtraBold_ttf, BinaryData::Baloo2ExtraBold_ttfSize);

    // Create a Font that uses this Typeface.
    float fontSize = 30.0f; // Adjust the font size as needed
    juce::Font balooFont = juce::Font(balooTypeface).withHeight(fontSize);

    // Set the font of the Label.
    sideEffectsLabel.setFont(balooFont);
    
    sideEffectsLabel.setText("side effects", juce::dontSendNotification);
    // Don't forget to add the label to the editor and set its size and position
    addAndMakeVisible(sideEffectsLabel);



    // Create the AudioProcessorValueTreeState::SliderAttachment objects
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Cutoff", cutoffKnob);
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Resonance", resonanceKnob);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Drive", driveKnob);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(600, 400);
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
    juce::Colour startColour = juce::Colour::fromString("#FF535353");
    juce::Colour endColour = juce::Colour::fromString("#FF302C28");
    juce::ColourGradient gradient = juce::ColourGradient::vertical(startColour, 0, endColour, (float)getHeight());

    // Create a FillType object and set its gradient
    juce::FillType fillType;
    fillType.setGradient(gradient);

    // Set the fill type of the Graphics object
    g.setFillType(fillType);

    // Fill the component's bounds with the gradient
    g.fillRect(getLocalBounds());
}



void DRFilterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // Set the positions of the knobs and labels
    int knobWidth = 150;
    int knobHeight = 150;
    int margin = 10;
    
    

    
    cutoffLabel.setBounds(margin, margin, knobWidth, knobHeight / 2);
    resonanceLabel.setBounds(cutoffLabel.getRight() + margin, margin, knobWidth, knobHeight / 2);
    driveLabel.setBounds(resonanceLabel.getRight() + margin, margin, knobWidth, knobHeight / 2);

    cutoffKnob.setBounds(margin, getHeight() / 2 - knobHeight / 2, knobWidth, knobHeight);
    resonanceKnob.setBounds(cutoffKnob.getX() + cutoffKnob.getWidth() + margin, getHeight() / 2 - knobHeight / 2, knobWidth, knobHeight);
    driveKnob.setBounds(resonanceKnob.getX() + resonanceKnob.getWidth() + margin, getHeight() / 2 - knobHeight / 2, knobWidth, knobHeight);

     sideEffectsLabel.setBounds(getWidth() - 109, getHeight() - 31, 109, 31);

    // TODO: Set the position and size of the spectrogram component

}

