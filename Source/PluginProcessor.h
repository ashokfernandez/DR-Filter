/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
// #include "CustomWaveShaper.h"
#include "Distortion.h"

#include "Config.h"

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
