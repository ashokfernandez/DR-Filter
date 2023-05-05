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
    
}

DRFilterAudioProcessor::~DRFilterAudioProcessor()
{
}


juce::AudioProcessorValueTreeState::ParameterLayout DRFilterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Add parameters to the layout
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Cutoff", 1), "Cutoff", 0.0f, 1.0f, 10.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Resonance", 1), "Resonance", 0.1f, 10.0f, 0.05f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("Drive", 1), "Drive", 0.0f, 2.0f, 0.01f));

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

void DRFilterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Update the spec object with the current sample rate and block size
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    // Initialize the low-pass and high-pass filters and the saturation processor
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);
    saturationProcessor.prepare(spec);

    // Set the initial coefficients for the low-pass and high-pass filters
    lowPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, 1000.0f, 1.0f);
    highPassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, 1000.0f, 1.0f);

    // Set the saturation processor to use a simple waveshaper
    saturationProcessor.functionToUse = [](float x) {
        return std::tanh(x); // You can experiment with different waveshaping functions for different saturation characteristics
    };
}

juce::dsp::IIR::Coefficients<float>::Ptr createFilterCoefficients(double sampleRate, float frequency, float Q, bool isLowPass)
{
    if (frequency <= 0 || frequency >= sampleRate * 0.5 || Q <= 0)
    {
        juce::Logger::writeToLog("Invalid filter parameters: sampleRate = " + juce::String(sampleRate) +
                                 ", frequency = " + juce::String(frequency) + ", Q = " + juce::String(Q) +
                                 ", isLowPass = " + (isLowPass ? "true" : "false"));

        return nullptr;
    }

    if (isLowPass)
    {
        return juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, frequency, Q);
    }
    else
    {
        return juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, frequency, Q);
    }
}



void DRFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto cutoff = apvts.getRawParameterValue("Cutoff")->load();
    auto resonance = apvts.getRawParameterValue("Resonance")->load();
    auto drive = apvts.getRawParameterValue("Drive")->load();

    // Calculate the cutoff frequencies for the low-pass and high-pass filters with new frequency ranges
    float lowPassCutoff = juce::jmap(cutoff, 0.0f, 1.0f, 20.0f, 20000.0f);
    float highPassCutoff = juce::jmap(1.0f - cutoff, 0.0f, 1.0f, 20.0f, 20000.0f);

    // Map the resonance value to a valid Q factor range (0.1 to 10)
    float Q = juce::jmap(resonance, 0.1f, 10.0f);

    // Set the filter coefficients for low-pass and high-pass filters
    auto lpCoefficients = createFilterCoefficients(spec.sampleRate, lowPassCutoff, Q, true);
    auto hpCoefficients = createFilterCoefficients(spec.sampleRate, highPassCutoff, Q, false);


    if (lpCoefficients && hpCoefficients)
    {
        lowPassFilter.coefficients = lpCoefficients;
        highPassFilter.coefficients = hpCoefficients;

        // Process the buffer through the biquad filter
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                // Apply the low-pass and high-pass filters
                float lowPassSample = lowPassFilter.processSample(channelData[sample]);
                float highPassSample = highPassFilter.processSample(channelData[sample]);

                // Mix the low-pass and high-pass filtered signals
                channelData[sample] = lowPassSample + highPassSample;

                // Apply the drive control to the signal
                // channelData[sample] *= drive;

                // Apply the saturation effect
                // channelData[sample] = saturationProcessor.processSample(channelData[sample]);
            }
        }
    }
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
