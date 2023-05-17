/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "SpectrogramComponent.h"
#include "FlatUIColouredKnob.h"
#include "SmallKnobLookAndFeel.h"


//==============================================================================
/**
*/
class DRFilterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DRFilterAudioProcessorEditor (DRFilterAudioProcessor&);
    ~DRFilterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;


private:
      // GUI components
    juce::Slider cutoffKnob;
    juce::Slider resonanceKnob;
    juce::Slider driveKnob;
    SpectrogramComponent spectrogram;

    // FlatUIColouredKnob instances
    FlatUIColouredKnob cutoffLookAndFeel;
    FlatUIColouredKnob resonanceLookAndFeel;
    FlatUIColouredKnob driveLookAndFeel;

    // Labels for the GUI components
    juce::Label cutoffLabel;
    juce::Label resonanceLabel;
    juce::Label driveLabel;
    juce::Label sideEffectsLabel;

    // Typeface and Font for custom font
    juce::Typeface::Ptr balooTypeface;
    juce::Font balooFont;

    // Attachment objects for linking the parameters to the GUI components
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DRFilterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DRFilterAudioProcessorEditor)
};
