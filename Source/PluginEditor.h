/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <iostream>
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

    //get rid of the labels

    // Exciter pramas
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider exciterNoiseAmountSlider;
    juce::Slider punchAmountSlider;
;

    //Replace With DECAY
    juce::Slider harmoSlider;
    juce::Slider resonanceSlider; 
    juce::Slider spreadSlider;
    juce::Slider shapeSlider;
    juce::Slider pitchSlider;

    juce::MidiKeyboardState keyboardState;          //midi keyboard in standalone
    juce::MidiKeyboardComponent keyboardComponent;  //the keyboard UI



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReSoundAudioProcessorEditor)
};
