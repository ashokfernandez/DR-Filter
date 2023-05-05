/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpectrogramComponent.h"

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


    // Labels for the GUI components
    juce::Label cutoffLabel;
    juce::Label resonanceLabel;
    juce::Label driveLabel;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DRFilterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DRFilterAudioProcessorEditor)
};
