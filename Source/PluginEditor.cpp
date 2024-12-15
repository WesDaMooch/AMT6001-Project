/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReSoundAudioProcessorEditor::ReSoundAudioProcessorEditor (ReSoundAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (500, 600);

    addAndMakeVisible(&keyboardComponent);
}

ReSoundAudioProcessorEditor::~ReSoundAudioProcessorEditor()
{
}

//==============================================================================
void ReSoundAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    //g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

}

void ReSoundAudioProcessorEditor::resized()
{
    int keyboardHeight = 64;
    keyboardComponent.setBounds(0, getHeight()-keyboardHeight, getWidth(), keyboardHeight);
}
