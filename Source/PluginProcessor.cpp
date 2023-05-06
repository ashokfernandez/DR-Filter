/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DRFilterAudioProcessor::DRFilterAudioProcessor()
    : AudioProcessor(BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Set the saturation function to the static saturationFunction
    saturationProcessor.functionToUse = &DRFilterAudioProcessor::saturationFunction;

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
    else if (parameterID == "Drive")
    {
        updateSaturation();
    }
}

// Define the static member variable in the implementation file
std::atomic<float> DRFilterAudioProcessor::saturationAmount;

void DRFilterAudioProcessor::updateSaturation()
{
    auto drive = apvts.getRawParameterValue("Drive")->load();
    saturationAmount.store(juce::jmap(drive, 0.0f, 100.0f, 1.0f, 50.0f));
}

float DRFilterAudioProcessor::saturationFunction(float x)
{
    auto saturationAmountLocal = saturationAmount.load();
    float saturated = std::atan(saturationAmountLocal * x) / std::atan(saturationAmountLocal);
    return saturated;
}


void DRFilterAudioProcessor::updateFilterCoefficients()
{
    auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
    auto resonance = apvts.getRawParameterValue("Resonance")->load();
    auto deadZone = 5.0f;
    float Q = juce::jmap(resonance, 0.0f, 10.0f, 0.02f, 1.5f);

    if (cutoff < -deadZone)
    {
        float lowPassCutoff = juce::jmap(cutoff, -100.0f, -deadZone, 20.0f, 20000.0f);
        stateVariableTPTFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        stateVariableTPTFilter.setCutoffFrequency(lowPassCutoff);
        stateVariableTPTFilter.setResonance(Q);
    }
    else if (cutoff > deadZone)
    {
        float highPassCutoff = juce::jmap(cutoff, deadZone, 100.0f, 20.0f, 20000.0f);
        stateVariableTPTFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        stateVariableTPTFilter.setCutoffFrequency(highPassCutoff);
        stateVariableTPTFilter.setResonance(Q);
    }
    else
    {
        stateVariableTPTFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        stateVariableTPTFilter.setCutoffFrequency(20000.0f);
        stateVariableTPTFilter.setResonance(0.1f);
    }
}







//==============================================================================
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

    // Prepare waveshaper
    saturationProcessor.prepare(spec);
    saturationProcessor.reset();
    updateSaturation();
}


void DRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = 0; i < totalNumOutputChannels; ++i)
    {
        if (i < totalNumInputChannels)
            continue;

        buffer.clear(i, 0, buffer.getNumSamples());
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        // Process the filter
        juce::dsp::AudioBlock<float> audioBlock(buffer);
        juce::dsp::AudioBlock<float> singleChannelBlock = audioBlock.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> context(singleChannelBlock);
        stateVariableTPTFilter.process(context);

        // Process the waveshaper
        saturationProcessor.process(context);
    }
}








// void DRFilterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
// {
//     // Update the spec object with the current sample rate and block size
//     spec.sampleRate = sampleRate;
//     spec.maximumBlockSize = samplesPerBlock;
//     spec.numChannels = getTotalNumOutputChannels();

//     // Initialize the low-pass and high-pass filters and the saturation processor
//     lowPassFilter.prepare(spec);
//     highPassFilter.prepare(spec);
//     saturationProcessor.prepare(spec);

//     // Set the initial coefficients for the low-pass and high-pass filters
//     // lowPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, 1000.0f, 1.0f);
//     // highPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, 1000.0f, 1.0f);

//     // Set the saturation processor to use a simple waveshaper
//     saturationProcessor.functionToUse = [](float x) {
//         return std::tanh(x); // You can experiment with different waveshaping functions for different saturation characteristics
//     };
// }


// void DRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
// {
//      auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
//      auto resonance = apvts.getRawParameterValue("Resonance")->load();
//      auto drive = apvts.getRawParameterValue("Drive")->load();

    // Calculate the cutoff frequencies for the low-pass and high-pass filters with new frequency ranges
    // float lowPassCutoff = 21000.0f;

    // if (cutoff < 0.5) {
    //     lowPassCutoff = juce::jmap(cutoff, 0.0f, 1.0f, 20.0f, 20000.0f);
    // }
    
    
    

    // Set the filter coefficients for low-pass and high-pass filters
    // auto lpCoefficients = createFilterCoefficients(spec.sampleRate, lowPassCutoff, Q, true);
    // auto hpCoefficients = createFilterCoefficients(spec.sampleRate, highPassCutoff, Q, false);
    // auto lpCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, lowPassCutoff, Q);
    // auto hpCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, highPassCutoff, Q);


    // if (lpCoefficients && hpCoefficients)
    // {
    //     lowPassFilter.coefficients = lpCoefficients;
    //     highPassFilter.coefficients = hpCoefficients;

    //     // Process the buffer through the biquad filter
    //     for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    //     {
    //         auto* channelData = buffer.getWritePointer(channel);

    //         for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    //         {
    //             // Apply the low-pass and high-pass filters
    //             float lowPassSample = lowPassFilter.processSample(channelData[sample]);
    //             // float highPassSample = highPassFilter.processSample(channelData[sample]);

    //             // Mix the low-pass and high-pass filtered signals
    //             channelData[sample] = lowPassSample; // + highPassSample;

    //             // Apply the drive control to the signal
    //              channelData[sample] *= drive;

    //             // Apply the saturation effect
    //              channelData[sample] = saturationProcessor.processSample(channelData[sample]);
    //         }
    //     }
    // }
  // }






void DRFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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
bool DRFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DRFilterAudioProcessor::createEditor()
{
    return new DRFilterAudioProcessorEditor (*this);
}

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



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DRFilterAudioProcessor();
}
