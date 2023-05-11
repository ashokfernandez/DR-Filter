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
#define FILTER_Q_MAX 8.0f

// Skew factor for non-linear mapping of parameters 
#define HIGHPASS_CUTOFF_SKEW_FACTOR 0.5f
#define LOWPASS_CUTOFF_SKEW_FACTOR 1.5f

// Min and max frequencies for filter range
#define LOWPASS_CUTOFF_MIN 20.0f
#define LOWPASS_CUTOFF_MAX 9500.0f
#define HIGHPASS_CUTOFF_MIN 20.0f
#define HIGHPASS_CUTOFF_MAX 5000.0f


// Max and min hearing frequencies
#define MAX_FREQ 20000.0f
#define MIN_FREQ 20.0f

// Time factor used to smooth parameter changes
#define SMOOTHING_TIME_SECONDS 0.5f
// #define NORMALISED_INTERNAL 0.1f

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
    // Parameter setup
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Signal chain objects
    juce::dsp::ProcessSpec spec;
    
    // juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterProcessor;
    
    // juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableTPTFilter<float>, juce::dsp::StateVariableFilter::Parameters<float>> filterProcessor;
    juce::dsp::StateVariableTPTFilter<float> filterProcessor;
    
    // juce::dsp::ProcessorDuplicator<juce::dsp::LadderFilter<float>, juce::dsp::LadderFilter<float>::Parameters> filterProcessor;
    // juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowPassFilterProcessor, highPassFilterProcessor;
    // juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filterProcessor;

    
    void updateFrequency();
    void updateResonance();
    void updateFilterType();
    // void updateFilter();

//    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> lowPassCutoffSmoothed, highPassCutoffSmoothed, resonanceSmoothed;
//    juce::NormalisableRange<float> lowPassCutoffRange, highPassCutoffRange;

    enum FilterType
    {
        LowPass,
        HighPass, 
        Disabled
    };

    FilterType currentFilterType = FilterType::Disabled;
    
    // Todo set this up wiht a processor duplicator
    CustomWaveShaper saturationProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DRFilterAudioProcessor)
};
