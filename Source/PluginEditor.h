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
                                     public juce::Slider::Listener,
                                     public juce::AudioProcessorParameter::Listener
{
public:
    ReSoundAudioProcessorEditor (ReSoundAudioProcessor&);
    ~ReSoundAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override; 

private:
     
    void sliderValueChanged(juce::Slider* slider) override;
    void parameterValueChanged(int parameterIndex, float newValue) override;            
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override;  

    ReSoundAudioProcessor& audioProcessor;

    juce::CustomLookAndFeel myCustomLookAndFeel;

    // Exciter sliders
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider exciterNoiseAmountSlider;
    juce::Slider punchAmountSlider;
    // Resonator sliders
    juce::Slider harmoSlider;
    juce::Slider decaySlider;
    juce::Slider spreadSlider;
    juce::Slider shapeSlider;
    juce::Slider pitchSlider;
    // Gain slider
    juce::Slider outputGainSlider;
    // UI keyboard
    juce::MidiKeyboardState keyboardState;  // Midi keyboard for standalone testing
    juce::MidiKeyboardComponent keyboardComponent;  

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReSoundAudioProcessorEditor)
};
