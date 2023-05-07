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
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Resonance", 1), "Resonance", 0.1f, 10.0f, 0.05f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Drive", 1), "Drive", 0.0f, 100.0f, 1.0f));

    return layout;
}

void DRFilterAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "Cutoff" || parameterID == "Resonance")
    {
        updateFilterCoefficients();
    }
}

// IIR FILTER
void DRFilterAudioProcessor::updateFilterCoefficients() { 
    auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
    auto resonance = apvts.getRawParameterValue("Resonance")->load();
    auto sampleRate = getSampleRate();
    float Q = juce::jmap(resonance, 0.0f, 10.0f, 0.707f, 1.3f);

    if (cutoff < -FILTER_DEAD_ZONE)
    {
        float lowPassCutoff = juce::jmap(cutoff, -100.0f, -FILTER_DEAD_ZONE, 20.0f, 20000.0f);
        auto lowPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, lowPassCutoff, Q);
        
        filterProcessor.coefficients = lowPassCoefficients;
        filterProcessor.reset();
    }
    else if (cutoff > FILTER_DEAD_ZONE)
    {
        float highPassCutoff = juce::jmap(cutoff, FILTER_DEAD_ZONE, 100.0f, 20.0f, 7000.0f);
        // float highPassCutoff = 5000.0f;
        auto highPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, highPassCutoff, Q);

        // float cutoff = ...; // input value
        // float deadZone = ...; // dead zone value
        // float minValue = 20.0f; // minimum output value
        // float maxValue = 7000.0f; // maximum output value
        // float skewFactor = 2.0f; // skew factor

        // // create a NormalisableRange with the given range and skew factor
        // juce::NormalisableRange<float> range(minValue, maxValue, skewFactor);

        // // map the input value to a normalized value
        // float normalizedValue = range.convertTo0to1(cutoff);

        // // map the normalized value to the output range
        // float highPassCutoff = range.convertFrom0to1(normalizedValue);

        // // outputValue now contains the mapped value with the skew factor applied


        // // float skewFactor = 0.5f;
        // // float skewedCutoff = std::pow(cutoff, skewFactor);
        // // float highPassCutoff = juce::jmap(outputValue, FILTER_DEAD_ZONE, 100.0f, 20.0f, 7000.0f);
        // auto highPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, highPassCutoff, Q);
        
        filterProcessor.coefficients = highPassCoefficients;
        filterProcessor.reset();
    }
    else
    {
        auto allPassCoefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, 1000.0f); // Replace 1000.0f with the desired frequency
        filterProcessor.coefficients = allPassCoefficients;
    }
}



// STATE VARIABLE FILTER
// void DRFilterAudioProcessor::updateFilterCoefficients()
// {
//     auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
//     auto resonance = apvts.getRawParameterValue("Resonance")->load();
//     auto deadZone = FILTER_DEAD_ZONE;
//     float Q = juce::jmap(resonance, 0.0f, 10.0f, 0.707f, 1.3f);

//     if (cutoff < -deadZone)
//     {
//         float lowPassCutoff = juce::jmap(cutoff, -100.0f, -deadZone, 20.0f, 20000.0f);
//         stateVariableTPTFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
//         stateVariableTPTFilter.setCutoffFrequency(lowPassCutoff);
//         stateVariableTPTFilter.setResonance(Q);
//     }
//     else if (cutoff > deadZone)
//     {
//         float highPassCutoff = juce::jmap(cutoff, deadZone, 100.0f, 20.0f, 20000.0f);
//         stateVariableTPTFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
//         stateVariableTPTFilter.setCutoffFrequency(highPassCutoff);
//         stateVariableTPTFilter.setResonance(Q);
//     }
//     else
//     {
//         stateVariableTPTFilter.reset();
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
    stateVariableTPTFilter.prepare(spec);
    stateVariableTPTFilter.reset();
    updateFilterCoefficients();

    filterProcessor.prepare(spec);
    filterProcessor.reset();

    // Prepare waveshaper
    saturationProcessor.prepare(spec);
    saturationProcessor.reset();
    // updateSaturation();
}

// void DRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
// {
//     // No denormals ensures that the processor does not hang on denormalized numbers
//     juce::ScopedNoDenormals noDenormals;

//     auto totalNumInputChannels = getTotalNumInputChannels();
//     auto totalNumOutputChannels = getTotalNumOutputChannels();
//     auto cutoff = apvts.getRawParameterValue("Cutoff")->load();

//     // Clear the output channels to avoid processing uninitialized data
//     for (auto i = 0; i < totalNumOutputChannels; ++i)
//     {
//         if (i < totalNumInputChannels)
//             continue;

//         buffer.clear(i, 0, buffer.getNumSamples());
//     }

//     // Process each channel
//     for (int channel = 0; channel < totalNumInputChannels; ++channel)
//     {
//         auto* channelData = buffer.getWritePointer(channel);
//         juce::dsp::AudioBlock<float> audioBlock(buffer);
//         juce::dsp::AudioBlock<float> singleChannelBlock = audioBlock.getSingleChannelBlock(channel);
//         juce::dsp::ProcessContextReplacing<float> context(singleChannelBlock);

//         // STATE VARIABLE FILTER
//         // If not in the deadzone, process the filter
//         // if (cutoff > deadZone || cutoff < -deadZone)
//         // {
//             // Process the filter
//             // stateVariableTPTFilter.process(context);
//         // }

//         // IIR FILTER
//         // If not in the deadzone, process the filter
//         if (cutoff > FILTER_DEAD_ZONE || cutoff < -FILTER_DEAD_ZONE)
//         {
//             // Process the filter
//             filterProcessor.process(context);
//         }
        
        
//         // Process the waveshaper
//         // saturationProcessor.process(context);
//     }
// }


void DRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // No denormals ensures that the processor does not hang on denormalized numbers
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto cutoff = apvts.getRawParameterValue("Cutoff")->load();

    // Clear the output channels to avoid processing uninitialized data
    for (auto i = 0; i < totalNumOutputChannels; ++i)
    {
        if (i < totalNumInputChannels)
            continue;

        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // Create a separate buffer for each channel
        juce::AudioBuffer<float> channelBuffer(1, buffer.getNumSamples());
        channelBuffer.copyFrom(0, 0, buffer.getReadPointer(channel), buffer.getNumSamples());

        // Create an audio block from the channel buffer
        juce::dsp::AudioBlock<float> singleChannelBlock(channelBuffer);

        // Process the channel
        juce::dsp::ProcessContextReplacing<float> context(singleChannelBlock);

        // STATE VARIABLE FILTER
        // If not in the deadzone, process the filter
        // if (cutoff > deadZone || cutoff < -deadZone)
        // {
            // Process the filter
            // stateVariableTPTFilter.process(context);
        // }

        // IIR FILTER
        // If not in the deadzone, process the filter
        if (cutoff > FILTER_DEAD_ZONE || cutoff < -FILTER_DEAD_ZONE)
        {
            // Process the filter
            filterProcessor.process(context);
        }

        // Process the waveshaper
        // saturationProcessor.process(context);

        // Copy the processed data back into the original buffer
        buffer.copyFrom(channel, 0, channelBuffer.getReadPointer(0), channelBuffer.getNumSamples());
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
//    auto state = apvts->copyState();
//    std::unique_ptr<juce::XmlElement> xml(state.createXml());
//    memoryBlock.append(xml->toString().toRawUTF8(), xml->toString().getNumBytesAsUTF8() + 1);
}

void DRFilterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
//    std::unique_ptr<juce::XmlElement> xmlState(juce::XmlDocument::parse(juce::String::createStringFromData(static_cast<const char*>(data), sizeInBytes)));
//
//    if (xmlState.get() != nullptr)
//    {
//        if (xmlState->hasTagName(apvts->state.getType()))
//        {
//            juce::ValueTree tree = juce::ValueTree::fromXml(*xmlState);
//            apvts->replaceState(tree);
//        }
//    }
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

