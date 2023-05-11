#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// CONSTRUCTOR/DESTRUCTOR
//==============================================================================

DRFilterAudioProcessor::DRFilterAudioProcessor()
    : AudioProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()),
      saturationProcessor(apvts)
    //   lowPassCutoffRange(MIN_FREQ, MAX_FREQ, NORMALISED_INTERNAL, LOWPASS_CUTOFF_SKEW_FACTOR),
    //   highPassCutoffRange(MIN_FREQ, MAX_FREQ, NORMALISED_INTERNAL, HIGHPASS_CUTOFF_SKEW_FACTOR)
    // lowPassCutoffRange(
    //         5.0f, 100.0f,
    //         [](float start, float end, float normalizedValue) {
    //             return start + (end - start) * normalizedValue;
    //         },
    //         [](float start, float end, float mappedValue) {
    //             return (mappedValue - start) / (end - start);
    //         },
    //         [](float start, float end, float mappedValue) {
    //             float k = std::log10(20000.0f / 20.0f) / std::log10(100.0f / 5.0f);
    //             return 20.0f * std::pow((mappedValue / 5.0f), k);
    //         }),
    //       highPassCutoffRange(
    //         5.0f, 100.0f,
    //         [](float start, float end, float normalizedValue) {
    //             return start + (end - start) * normalizedValue;
    //         },
    //         [](float start, float end, float mappedValue) {
    //             return (mappedValue - start) / (end - start);
    //         },
    //         [](float start, float end, float mappedValue) {
    //             float k = std::log(6000.0f / 20.0f) / std::log(50.0f / 5.0f);
    //             return 20.0f * std::pow((mappedValue / 5.0f), k);
    //         })

{
    // Add parameter listeners    
    apvts.addParameterListener("Cutoff", this);
    apvts.addParameterListener("Resonance", this);
    apvts.addParameterListener("Drive", this);
}

DRFilterAudioProcessor::~DRFilterAudioProcessor()
{
    // Remove parameter listeners
    apvts.removeParameterListener("Cutoff", this);
    apvts.removeParameterListener("Resonance", this);
    apvts.removeParameterListener("Drive", this);
}

// Returns a new instance of the plugin when called by the host
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DRFilterAudioProcessor();
}


//==============================================================================
// PARAMETERS
//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout DRFilterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Add parameters to the layout
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Cutoff", 1), "Cutoff", -100.0f, 100.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Resonance", 1), "Resonance", 0.0f, 10.0f, 0.05f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Drive", 1), "Drive", 0.0f, 100.0f, 1.0f));

    return layout;
}

void DRFilterAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "Cutoff")
    {
        // Set the target value for smoothedCutoff and determine the filter type changed
        updateFilterType();
        updateFrequency();
    }
    else if (parameterID == "Resonance")
    {
        // Set the target value for smoothedResonance
        updateResonance();
    }
}

void DRFilterAudioProcessor::updateFilterType() {
    auto cutoff = apvts.getRawParameterValue("Cutoff")->load();

    if (cutoff > FILTER_DEAD_ZONE)
    {
        currentFilterType = FilterType::HighPass;
        filterProcessor.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    }
    else if (cutoff < -FILTER_DEAD_ZONE)
    {
        currentFilterType = FilterType::LowPass;
        filterProcessor.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    }
    else
    {
        currentFilterType = FilterType::Disabled;
        filterProcessor.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    }
}

void DRFilterAudioProcessor::updateFrequency()
{   
    auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
    auto cutoffAmount = abs(cutoff);

    if (currentFilterType == FilterType::HighPass)
    {
        auto highPassCutoff = juce::jmap(cutoffAmount, FILTER_DEAD_ZONE, 100.0f, HIGHPASS_CUTOFF_MIN, HIGHPASS_CUTOFF_MAX);
        filterProcessor.setCutoffFrequency(highPassCutoff);
    }
    else if (currentFilterType == FilterType::LowPass)
    {
        auto lowPassCutoff = juce::jmap(cutoffAmount, FILTER_DEAD_ZONE, 100.0f, LOWPASS_CUTOFF_MAX, LOWPASS_CUTOFF_MIN);
        filterProcessor.setCutoffFrequency(lowPassCutoff);
    }
    // else
    // {
    //     auto lowPassCutoff = juce::jmap(cutoffAmount, 0.0f, 100.0f, FILTER_FREQ_MIN, FILTER_FREQ_MAX);
    //     filterProcessor.setLowPassFrequency(lowPassCutoff);
    //     auto highPassCutoff = juce::jmap(cutoffAmount, 0.0f, 100.0f, FILTER_FREQ_MIN, FILTER_FREQ_MAX);
    //     filterProcessor.setHighPassFrequency(highPassCutoff);
    // }
    
}

void DRFilterAudioProcessor::updateResonance()
{
    float resonance = apvts.getRawParameterValue("Resonance")->load();
    resonance = juce::jmap(resonance, 0.0f, 10.0f, FILTER_Q_MIN, FILTER_Q_MAX);
    // resonanceSmoothed.setTargetValue(resonance);
    filterProcessor.setResonance(resonance);
}

// // IIR FILTER
// void DRFilterAudioProcessor::updateFilter() { 
    
//     // Get the smoothed and skewed values
//     auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
    
//     filterProcessor.setType(juce::dsp::StateVariableTPTFilterType::highpass);

//     if (cutoff > FILTER_DEAD_ZONE)
//     {
//         filterProcessor.setType(juce::dsp::StateVariableTPTFilterType::highpass);
//     }
//     else if (cutoff < -FILTER_DEAD_ZONE)
//     {
//         filterProcessor.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
//     }
//     else
//     {
//         filterProcessor.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
//     }
// }


//==============================================================================
// AUDIO PROCESSING
//==============================================================================

void DRFilterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // Prepare filter    
    updateFilterType();
    updateFrequency();
    updateResonance();
    filterProcessor.prepare(spec);
    // filterProcessor.reset();


    // Set the smoothing time for smoothedCutoff and smoothedResonance
    // lowPassCutoffSmoothed.reset(sampleRate, SMOOTHING_TIME_SECONDS);
    // highPassCutoffSmoothed.reset(sampleRate, SMOOTHING_TIME_SECONDS);
    // resonanceSmoothed.reset(sampleRate, SMOOTHING_TIME_SECONDS);
    
    // Prepare waveshaper
    // saturationProcessor.prepare(spec);
    // saturationProcessor.reset();
}

void DRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // No denormals ensures that the processor does not hang on denormalized numbers
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    // auto cutoff = apvts.getRawParameterValue("Cutoff")->load();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    // Apply filter if it's not disabled
    juce::dsp::AudioBlock<float> block(buffer);
    if (currentFilterType != FilterType::Disabled)
    {
        filterProcessor.process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    // filterProcessor.process(juce::dsp::ProcessContextReplacing<float>(block));

    // Check if smoothed parameters are at their target values
    // if (lowPassCutoffSmoothed.isSmoothing() || highPassCutoffSmoothed.isSmoothing() || resonanceSmoothed.isSmoothing())
    // {
        // juce::Logger::writeToLog("Smoothing");
        // updateFilter();
    // }
}



void DRFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//==============================================================================
// GETTERS AND SETTERS
//==============================================================================

void DRFilterAudioProcessor::getStateInformation(juce::MemoryBlock& memoryBlock)
{

}

void DRFilterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

}

const juce::String DRFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DRFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DRFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DRFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DRFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DRFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DRFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DRFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DRFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void DRFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DRFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


//==============================================================================
// LINK TO GUI EDITOR
//==============================================================================

bool DRFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DRFilterAudioProcessor::createEditor()
{
    return new DRFilterAudioProcessorEditor (*this);
}

