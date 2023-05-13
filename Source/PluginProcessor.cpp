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
        lowPassCutoffRange(LOWPASS_CUTOFF_MIN, LOWPASS_CUTOFF_MAX),
        highPassCutoffRange(HIGHPASS_CUTOFF_MIN, HIGHPASS_CUTOFF_MAX)
        // ,
        // saturationProcessor(apvts)

{
    // Add parameter listeners    
    apvts.addParameterListener("Cutoff", this);
    apvts.addParameterListener("Resonance", this);
    apvts.addParameterListener("Drive", this);

    // Set skew factors for normalised control ranges
    highPassCutoffRange.setSkewForCentre(HIGHPASS_CUTOFF_SKEW_MIDPOINT);
    lowPassCutoffRange.setSkewForCentre(LOWPASS_CUTOFF_SKEW_MIDPOINT);
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

// Called by the DAW when the plugin should reset and flush and processing pipelines
void DRFilterAudioProcessor::reset() noexcept
{
    // Reset the filter
    filterProcessor.reset();

    // Reset the saturation processor
    distortionProcessor.reset();
}

//==============================================================================
// PARAMETERS
//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout DRFilterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Add parameters to the layout
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Cutoff", 1), "Cutoff", -100.0f, 100.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Resonance", 1), "Resonance", 0.0f, 10.0f, 1.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Drive", 1), "Drive", 0.0f, 100.0f, 0.0f));

    return layout;
}

void DRFilterAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "Cutoff")
    {
        // Update filter type and frequency when cutoff is moved
        updateFilterType();
        updateFrequency();
    }
    else if (parameterID == "Resonance")
    {
        // Set the resonance value
        updateResonance();
    }
    else if (parameterID == "Drive")
    {
        // Update the gain for the distorion processor
        updateDrive();
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
        auto highPassCutoff = juce::jmap(cutoffAmount, FILTER_DEAD_ZONE, 100.0f, 0.0f, 1.0f);
        highPassCutoff = highPassCutoffRange.convertFrom0to1(highPassCutoff);
        filterProcessor.setCutoffFrequency(highPassCutoff);
    }
    else if (currentFilterType == FilterType::LowPass)
    {
        auto lowPassCutoff = juce::jmap(cutoffAmount, FILTER_DEAD_ZONE, 100.0f, 1.0f, 0.0f);
        lowPassCutoff = lowPassCutoffRange.convertFrom0to1(lowPassCutoff);
        filterProcessor.setCutoffFrequency(lowPassCutoff);
    }
}

void DRFilterAudioProcessor::updateResonance()
{
    float resonance = apvts.getRawParameterValue("Resonance")->load();
    resonance = juce::jmap(resonance, 0.0f, 10.0f, FILTER_Q_MIN, FILTER_Q_MAX);
    // resonanceSmoothed.setTargetValue(resonance);
    filterProcessor.setResonance(resonance);
}

void DRFilterAudioProcessor::updateDrive()
{
    float drive = apvts.getRawParameterValue("Drive")->load();
    drive = juce::jmap(drive, 0.0f, 100.0f, 1.0f, 5.0f);
    distortionProcessor.setDrive(drive);
}

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
    
    // Prepare waveshaper
    updateDrive();
    distortionProcessor.prepare(spec);
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
        distortionProcessor.process(juce::dsp::ProcessContextReplacing<float>(block));
    }
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

