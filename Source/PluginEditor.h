/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StyleSheet.h"

//==============================================================================
/**
*/
class ReSoundAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                     public juce::Slider::Listener
{
public:
    ReSoundAudioProcessorEditor (ReSoundAudioProcessor&);
    ~ReSoundAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;

    ReSoundAudioProcessor& audioProcessor;

    juce::CustomLookAndFeel myCustomLookAndFeel;

    //Replace With DECAY
    juce::Label resonanceLabel;
    juce::Slider resonanceSlider; 

    juce::Label spreadLabel;
    juce::Slider spreadSlider;

    juce::Label shapeLabel;
    juce::Slider shapeSlider;

    juce::MidiKeyboardState keyboardState;          //midi keyboard in standalone
    juce::MidiKeyboardComponent keyboardComponent;  //the keyboard UI

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReSoundAudioProcessorEditor)
};
