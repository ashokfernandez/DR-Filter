#pragma once

#include <JuceHeader.h>

class Distortion
{
public:
    Distortion();
    ~Distortion();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(const juce::dsp::ProcessContextReplacing<float>& context);
    void setDrive(float newDrive);

private:
    static float waveshapingFunction(float x, float drive);
    juce::dsp::WaveShaper<float, std::function<float(float)>> waveshaper;
    std::atomic<float> drive;
};
