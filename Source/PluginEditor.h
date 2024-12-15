/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ReSoundAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ReSoundAudioProcessorEditor (ReSoundAudioProcessor&);
    ~ReSoundAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    ReSoundAudioProcessor& audioProcessor;

    juce::MidiKeyboardState keyboardState;          //midi keyboard in standalone
    juce::MidiKeyboardComponent keyboardComponent;  //the keyboard UI



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReSoundAudioProcessorEditor)
};
