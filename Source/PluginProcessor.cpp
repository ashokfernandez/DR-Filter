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
    // Create the parameter layout for the plugin
    // auto layout = createParameterLayout();

    // Add parameter listeners
    // apvts.addParameterListener("cutoff", this);
    // apvts.addParameterListener("resonance", this);
    // apvts.addParameterListener("drive", this);

    // Initialize the AudioProcessorValueTreeState object with the layout
    // apvts = std::make_unique<juce::AudioProcessorValueTreeState>(*this, nullptr, "Parameters", std::move(layout));

    // Set up the filter
    // iirFilterState[0] = std::make_unique<juce::IIRFilter>();
    // iirFilterState[1] = std::make_unique<juce::IIRFilter>();
    // filter.setType(juce::dsp::IIR::FilterType::lowPass);
    // filter.setCoefficients(juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), 1000.0f, 1.0f));
    

    // cutoff = apvts.getRawParameterValue("cutoff");
    // resonance = apvts.getRawParameterValue("resonance");
    // drive = apvts.getRawParameterValue("drive");

    // Initialize the filter and gain objects
    // spec.sampleRate = getSampleRate();
    // spec.maximumBlockSize = getBlockSize();
    // spec.numChannels = getTotalNumInputChannels();
    
    // filter.prepare(spec);
    // driveGain.prepare(spec);

    

    // iirFilterState[0] = std::make_unique<juce::IIRFilter>();
    // iirFilterState[1] = std::make_unique<juce::IIRFilter>();
    
    // Initialize filter coefficients with a low-pass filter, default cutoff frequency of 500 Hz, and Q-factor of 1
    // float defaultCutoff = 500.0f;
    // float defaultQ = 1.0f;
    // filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), defaultCutoff, defaultQ);
    // driveGain.setGainLinear(1.0f);
}

DRFilterAudioProcessor::~DRFilterAudioProcessor()
{
    // Remove listeners from the parameters
    // apvts->removeParameterListener("cutoff", cutoffListener);
    // apvts->removeParameterListener("resonance", resonanceListener);
    // apvts->removeParameterListener("drive", driveListener);
}

// void DRFilterAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
// {
//     if (parameterID == "cutoff")
//     {
//         cutoff = newValue;
//         updateFilter();
//     }
//     else if (parameterID == "resonance")
//     {
//         resonance = newValue;
//         updateFilter();
//     }
//     else if (parameterID == "drive")
//     {
//         drive = newValue;
//         updateFilter();
//     }
// }


// juce::AudioProcessorValueTreeState::ParameterLayout DRFilterAudioProcessor::createParameterLayout()
// {
//     // Create a ParameterLayout object to hold the parameters
//     juce::AudioProcessorValueTreeState::ParameterLayout layout;

//     // Create a unique ID for each parameter
//     const juce::String cutoffID = "cutoff";
//     const juce::String resonanceID = "resonance";
//     const juce::String driveID = "drive";

//     // Add the parameters to the layout, using version hints
//     layout.add(std::make_unique<juce::AudioParameterFloat>(
//         cutoffID, "Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.5f), 1000.0f, "Hz",
//         juce::AudioProcessorParameter::genericParameter, nullptr, nullptr, true, 1));
//     layout.add(std::make_unique<juce::AudioParameterFloat>(
//         resonanceID, "Resonance", juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f, 1.0f), 1.0f, "",
//         juce::AudioProcessorParameter::genericParameter, nullptr, nullptr, true, 1));
//     layout.add(std::make_unique<juce::AudioParameterFloat>(
//         driveID, "Drive", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.5f, "",
//         juce::AudioProcessorParameter::genericParameter, nullptr, nullptr, true, 1));

//     return layout;
// }


juce::AudioProcessorValueTreeState::ParameterLayout DRFilterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Add parameters to the layout
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Cutoff", 1), "Cutoff", 20.0f, 20000.0f, 1000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Resonance", 1), "Resonance", 1.0f, 5.0f, 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Drive", 1), "Drive", 1.0f, 10.0f, 1.0f));

    return layout;
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
void DRFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    // spec.sampleRate = sampleRate;
    // spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    // spec.numChannels = getTotalNumOutputChannels();

    // filter.prepare(spec);
    // driveGain.prepare(spec);
}

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

void DRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // juce::ScopedNoDenormals noDenormals;
    // auto currentCutoff = cutoff->load();
    // auto currentResonance = resonance->load();
    // auto currentDrive = drive->load();

    auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
    auto resonance = apvts.getRawParameterValue("Resonance")->load();
    auto drive = apvts.getRawParameterValue("Drive")->load();



    // auto totalNumInputChannels  = getTotalNumInputChannels();
    // auto totalNumOutputChannels = getTotalNumOutputChannels();

    // for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    //     buffer.clear(i, 0, buffer.getNumSamples());

    // auto currentCutoff = apvts->getRawParameterValue("CUTOFF")->load();
    // auto currentResonance = apvts->getRawParameterValue("RESONANCE")->load();
    // auto currentDrive = apvts->getRawParameterValue("DRIVE")->load();

    // if (currentCutoff != lastCutoff || currentResonance != lastResonance)
    // {
    //     for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //     {
    //         iirFilterState[channel]->setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), currentCutoff, currentResonance));
    //     }

    //     lastCutoff = currentCutoff;
    //     lastResonance = currentResonance;
    // }

    // for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // {
    //     auto* channelData = buffer.getWritePointer(channel);

    //     if (currentDrive != lastDrive)
    //     {
    //         for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    //         {
    //             channelData[sample] *= juce::jlimit<float>(1.0f, 10.0f, currentDrive);
    //         }
    //         lastDrive = currentDrive;
    //     }

    //     iirFilterState[channel]->processSamples(channelData, buffer.getNumSamples());
    // }
}




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
