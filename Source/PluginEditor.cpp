/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void styleSlider(juce::Slider& slider)
{
    //make cool
    //have styles make with custom look and feel
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setColour(juce::Slider::trackColourId, juce::Colours::whitesmoke);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}

//==============================================================================
ReSoundAudioProcessorEditor::ReSoundAudioProcessorEditor (ReSoundAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (500, 600);
    setResizable(true, true); 

    addAndMakeVisible(&keyboardComponent);
    keyboardState.addListener(&audioProcessor.getMidiMessageCollector());

    styleSlider(resonanceSlider);
    //set value, getter
    resonanceSlider.setRange(1, 200);
    resonanceSlider.setValue(100);
    resonanceSlider.addListener(this); 
    addAndMakeVisible(&resonanceSlider); 
}

ReSoundAudioProcessorEditor::~ReSoundAudioProcessorEditor()
{
    //Clean up listeners
    keyboardState.removeListener(&audioProcessor.getMidiMessageCollector());
}

//==============================================================================
void ReSoundAudioProcessorEditor::paint (juce::Graphics& g)
{
    //Colours
   
    //Dark Greys:
    //Jet = 60, 55, 68
    //Onyx = 54, 57, 70
    //Gunmetal = 43, 47, 59
    //Raisin black = 34, 38, 47
    //Eerie black = 26, 29, 35, or 31, 31, 31
    //Night = 17, 19, 23

    //Light Greys:
    //Isabelline = 242, 239, 234
    //Plat = 221, 221, 221
    //Timber wolf = 214, 214, 214
    
    //Accents:
    //

    //Background Colour
    g.fillAll(juce::Colour(242, 239, 234));

    g.setColour (juce::Colours::white);
    //g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

}

void ReSoundAudioProcessorEditor::resized()
{
    int keyboardHeight = 64;
    keyboardComponent.setBounds(0, getHeight()-keyboardHeight, getWidth(), keyboardHeight);

    int dialSize = 100;
    resonanceSlider.setBounds(100, 100, dialSize, dialSize);
}

void ReSoundAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &resonanceSlider)
    {
        //editor -> processor -> voice
        //make a better way of doing this?  
        audioProcessor.setRes(slider->getValue());
    }
}
