/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
// #include "CustomWaveShaper.h"
#include "Distortion.h"

// Max and min hearing frequencies
#define MAX_FREQ 20000.0f
#define MIN_FREQ 20.0f

// How many values on either side of zero the filter will stay disabled for
#define FILTER_DEAD_ZONE 5.0f

// Min and max frequencies for filter range
#define LOWPASS_CUTOFF_MIN 20.0f
#define LOWPASS_CUTOFF_MAX 9500.0f
#define HIGHPASS_CUTOFF_MIN 20.0f
#define HIGHPASS_CUTOFF_MAX 5000.0f

// The cutoff control needs to map to a cutoff frequency in Hz, but this isn't a linear relationship
// The skew midpoint is the frequency the cutoff should should point to when the cutoff knob is at halfway
// The midpoint will determine how skewed the mapping is
#define HIGHPASS_CUTOFF_SKEW_MIDPOINT 500.0f
#define LOWPASS_CUTOFF_SKEW_MIDPOINT 1000.0f

// Step size inside the normalised cutoff value range
#define NORMALISED_INTERVAL 0.1f

// Minimum and maximum Q values for the filter, starting at 0.707 (butterworth) up to spikey
#define FILTER_Q_MIN 0.707f
#define FILTER_Q_MAX 8.0f

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
    void reset() noexcept;

    juce::AudioProcessorValueTreeState apvts;

    

private:
    // Parameter setup
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::NormalisableRange<float> lowPassCutoffRange, highPassCutoffRange;

    // Signal chain objects
    juce::dsp::ProcessSpec spec;
    // juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableTPTFilter<float>, juce::dsp::StateVariableFilter::Parameters<float>> filterProcessor;
    juce::dsp::StateVariableTPTFilter<float> filterProcessor;
    
    // Methods to update DSP when the parameters change
    void updateFrequency();
    void updateResonance();
    void updateFilterType();
    void updateDrive();

    // Track the current filter type so we know when to disable the filter during processing
    enum FilterType
    {
        LowPass,
        HighPass, 
        Disabled
    };

    FilterType currentFilterType = FilterType::Disabled;
    
    // Todo set this up wiht a processor duplicator
    // CustomWaveShaper saturationProcessor;
    // juce::dsp::WaveShaper<float> saturationProcessor; 
    Distortion distortionProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DRFilterAudioProcessor)
};
