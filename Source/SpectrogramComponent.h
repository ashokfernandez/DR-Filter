/*
  ==============================================================================

    SpectrogramComponent.h
    Created: 5 May 2023 5:33:49pm
    Author:  Ashok Fernandez

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SpectrogramComponent : public juce::Component
{
public:
    SpectrogramComponent();
    ~SpectrogramComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // TODO: Add members for the spectrogram display

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};

