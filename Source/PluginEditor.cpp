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

    resonanceKnob.setSliderStyle(juce::Slider::Rotary);
    resonanceKnob.setRange(0.0, 1.0, 0.01);
    resonanceKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    resonanceKnob.setLookAndFeel(&resonanceLookAndFeel);  // set the look and feel
    addAndMakeVisible(resonanceKnob);

    driveKnob.setSliderStyle(juce::Slider::Rotary);
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

    // Add and make the spectrogram component visible
    addAndMakeVisible(spectrogram);

    // Create the AudioProcessorValueTreeState::SliderAttachment objects
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Cutoff", cutoffKnob);
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Resonance", resonanceKnob);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Drive", driveKnob);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);
}

DRFilterAudioProcessorEditor::~DRFilterAudioProcessorEditor()
{
    cutoffKnob.setLookAndFeel(nullptr);     // clear the look and feel
    resonanceKnob.setLookAndFeel(nullptr);  // clear the look and feel
    driveKnob.setLookAndFeel(nullptr);      // clear the look and feel
}

//==============================================================================
void DRFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("DRFilter", getLocalBounds(), juce::Justification::centred, 1);
}

void DRFilterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // Set the positions of the knobs and labels
    int spectrogramHeight = 100;
    int knobWidth = 100;
    int knobHeight = 100;
    int margin = 10;
    
    spectrogram.setBounds(margin, margin, getWidth() - 2 * margin, spectrogramHeight);

    
    cutoffLabel.setBounds(margin, margin, knobWidth, knobHeight / 2);
    resonanceLabel.setBounds(cutoffLabel.getRight() + margin, margin, knobWidth, knobHeight / 2);
    driveLabel.setBounds(resonanceLabel.getRight() + margin, margin, knobWidth, knobHeight / 2);

    cutoffKnob.setBounds(margin, getHeight() / 2 - knobHeight / 2, knobWidth, knobHeight);
    resonanceKnob.setBounds(cutoffKnob.getX() + cutoffKnob.getWidth() + margin, getHeight() / 2 - knobHeight / 2, knobWidth, knobHeight);
    driveKnob.setBounds(resonanceKnob.getX() + resonanceKnob.getWidth() + margin, getHeight() / 2 - knobHeight / 2, knobWidth, knobHeight);

    // TODO: Set the position and size of the spectrogram component

}

