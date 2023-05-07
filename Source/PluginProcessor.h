/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomWaveShaper.h"

// How many values on either side of zero the filter will stay disabled for
#define FILTER_DEAD_ZONE 5.0f

// Minimum and maximum Q values for the filter, starting at 0.707 (butterworth) up to spikey
#define FILTER_Q_MIN 0.707f
#define FILTER_Q_MAX 5.0f

//==============================================================================
/**
*/
class DRFilterAudioProcessor  : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    DRFilterAudioProcessor();
    ~DRFilterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;

    

private:
    
    void updateFilterCoefficients();

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    
    // Declare the state variable filter for stereo audio processing
    // juce::dsp::StateVariableTPTFilter<float> stateVariableTPTFilter;
    // juce::dsp::IIR::Filter<float> filterProcessor;
    
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterProcessor;
    // filterProcessor

    // Declare the saturation processor object
    CustomWaveShaper saturationProcessor;

    // Declare the ProcessSpec object
    juce::dsp::ProcessSpec spec;

  

    

    //==============================================================================


    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DRFilterAudioProcessor)
};
