#include "Distortion.h"

Distortion::Distortion() : drive(1.0f)
{
    waveshaper.functionToUse = [this](float x) { return waveshapingFunction(x, drive.load()); };
}

Distortion::~Distortion()
{
}

void Distortion::prepare(const juce::dsp::ProcessSpec& spec)
{
    waveshaper.prepare(spec);
}

void Distortion::reset()
{
    waveshaper.reset();
}

void Distortion::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    waveshaper.process(context);
}

void Distortion::setDrive(float newDrive)
{
    drive.store(newDrive);
}

float Distortion::waveshapingFunction(float x, float drive)
{
    // This is a sample distortion function that uses "drive".
    // Replace this with a function that suits your specific needs.
    return x * (fabs(x) + drive) / (x * x + (drive - 1) * fabs(x) + 1);
}
