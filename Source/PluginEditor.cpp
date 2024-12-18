/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void styleSlider(juce::Slider& slider)
{
    //this could be put in CustomLookAndFeel
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(43, 47, 59));
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(27, 161, 132));
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(27, 161, 132));
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}

void styleLabel(juce::Label& label, const juce::String name)
{
    label.setColour(juce::Label::textColourId, juce::Colours::black);
    //auto font = label.getFont();
    //font.setHeight(font.getHeight() - 2);
    //label.setFont(font);
    label.setText(name, juce::dontSendNotification);
}

//==============================================================================
ReSoundAudioProcessorEditor::ReSoundAudioProcessorEditor (ReSoundAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    //Use custom LookAndFeel Class
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLookAndFeel);

    setSize (500, 600);
    setResizable(true, true); 


    //UI elements
    addAndMakeVisible(&keyboardComponent);
    keyboardState.addListener(&audioProcessor.getMidiMessageCollector());

    //This is getting messy
 
    styleSlider(resonanceSlider);
    //set value, getter
    resonanceSlider.setRange(1, 300);
    resonanceSlider.setValue(100);
    resonanceSlider.addListener(this); 
    addAndMakeVisible(&resonanceSlider);

    styleLabel(resonanceLabel, "RES");
    resonanceLabel.attachToComponent(&resonanceSlider, false);  //looks bad but it'll do for now
    addAndMakeVisible(&resonanceLabel);

    styleSlider(spreadSlider);
    spreadSlider.setRange(0.5, 3);
    spreadSlider.setValue(2);
    spreadSlider.addListener(this);
    addAndMakeVisible(&spreadSlider);

    styleLabel(spreadLabel, "SPREAD");
    spreadLabel.attachToComponent(&spreadSlider, false); 
    addAndMakeVisible(&spreadLabel);


    styleSlider(shapeSlider);
    shapeSlider.setRange(0.0f, 1.0f);
    shapeSlider.setValue(0.0f);
    shapeSlider.addListener(this);
    addAndMakeVisible(&shapeSlider);

    styleLabel(shapeLabel, "SHAPE");
    shapeLabel.attachToComponent(&shapeSlider, false);
    addAndMakeVisible(&shapeLabel);


}

ReSoundAudioProcessorEditor::~ReSoundAudioProcessorEditor()
{
    //Clean up listeners
    keyboardState.removeListener(&audioProcessor.getMidiMessageCollector());
    //Reset LookAndFeel to default
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
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
    //orange = 245, 143, 41
    //Coquelicot = 241, 80, 37 
    //Blue (Munsell) = #34AA7
    //Pine Greesn = 19, 111, 91

    juce::Colour isabelline = juce::Colour(242, 239, 234);

    //Background Colour
    g.fillAll(isabelline);

    //g.setColour (juce::Colours::white);
    //g.setFont (juce::FontOptions (15.0f));
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);

}

void ReSoundAudioProcessorEditor::resized()
{
    auto keyboardHeight = 64;
    keyboardComponent.setBounds(0, getHeight()-keyboardHeight, getWidth(), keyboardHeight);

    auto dialSize = 90;
    auto labelSize = 80; 
    auto labelPadding = 100;

    auto firstRowParams = 100;
    auto firstColumnParams = 100;
    
    //auto labelTextWidth = resonanceLabel.getFont().getStringWidthFloat(resonanceLabel.getText());

    resonanceSlider.setBounds(firstRowParams, firstColumnParams, dialSize, dialSize);
    //resonanceLabel.setBounds(firstRowParams+(resonanceSlider.getWidth()/2)-labelSize, firstColumnParams + labelPadding,
        //labelSize, labelSize);

    spreadSlider.setBounds(200, 100, dialSize, dialSize);
    shapeSlider.setBounds(100, 200, dialSize, dialSize);
}

void ReSoundAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &resonanceSlider)
    {
        //editor -> processor -> voice
        //make a better way of doing this?  
        audioProcessor.setRes(slider->getValue());
    }
    else if (slider == &spreadSlider)
    {
        audioProcessor.setSpread(slider->getValue());
    }
    else if (slider == &shapeSlider)
    {
        audioProcessor.setShape(slider->getValue());
    }
}
