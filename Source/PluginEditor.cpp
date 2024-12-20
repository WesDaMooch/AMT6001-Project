/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace customColours
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
    
    //Light Colours
    juce::Colour isabelline = juce::Colour(242, 239, 234);
    juce::Colour platinum = juce::Colour(221, 221, 221);
    juce::Colour timberwolf = juce::Colour(214, 214, 214);

    //Dark Greys
    juce::Colour gray33 = juce::Colour(84, 84, 84);
    juce::Colour jet = juce::Colour(60, 55, 68);
    juce::Colour gunmetal = juce::Colour(43, 47, 59);
    juce::Colour eerieBlack = juce::Colour(31, 31, 31);

    //Accent Colours
    juce::Colour coquelicot = juce::Colour(241, 80, 37);
}

namespace gridUI
{
    const float columnOne = 40;
    const float columnTwo = 100;

    const float rowOne = 100;


}


void styleDial(juce::Slider& slider, double minRange, double maxRange, double defaultValue,
    double interval=0, bool textBox=false)
{   
    //this text not working
    juce::Slider::TextEntryBoxPosition textBoxStyle = juce::Slider::NoTextBox;
    if (textBox == true)
        juce::Slider::TextEntryBoxPosition textBoxStyle = juce::Slider::TextBoxBelow;

    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, customColours::eerieBlack);
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, customColours::coquelicot);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, customColours::coquelicot);
    slider.setRange(minRange, maxRange, interval);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 0);
}

void styleVerticalSlider(juce::Slider& slider, double minRange, double maxRange, double defaultValue,
    double interval=0)
{
    //this could be put in CustomLookAndFeel
    slider.setSliderStyle(juce::Slider::LinearVertical);
    //slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, customColours::eerieBlack);
    //slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, customColours::eerieBlack);
    slider.setColour(juce::Slider::ColourIds::trackColourId, customColours::coquelicot);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, customColours::gray33);
    slider.setColour(juce::Slider::ColourIds::backgroundColourId, customColours::eerieBlack);
    slider.setRange(minRange, maxRange, interval);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}

void styleHorizontalSlider(juce::Slider& slider, double minRange, double maxRange, double defaultValue)
{
    //this could be put in CustomLookAndFeel
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colour(43, 47, 59));
    slider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colour(27, 161, 132));
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour(27, 161, 132));
    slider.setRange(minRange, maxRange);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}


void styleLabel(juce::Label& label, const juce::String name)
{
    label.setColour(juce::Label::textColourId, customColours::jet);
    auto font = label.getFont();
    font.setSizeAndStyle(font.getHeight(), "Arial", 1.0f, 0.0f);
    //font.setBold(true);
    //font.setItalic(true);
    label.setFont(font);
    label.setJustificationType(juce::Justification::centred);
    //label.setTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, 0.5 * label.getHeight(), 0.5 * label.getWidth()));
    //90 degrees in radians = pi/2
    //label.setTransform(juce::AffineTransform::rotation(juce::MathConstants<float>::halfPi));

    label.setText(name, juce::dontSendNotification);
    
}

void drawVerticalLabel(juce::Graphics& g, juce::Font font, juce::Colour colour,
    const juce::String name, float targetSliderX, float targetSliderY)
{
    //draw a vertical label for slider

    float fontWidth = font.getStringWidthFloat(name);
    float fontPadding = 5.0f;

    float x = targetSliderX; // +fontPadding;
    float y = targetSliderY; //+ fontWidth;
    //float y = 355;

    juce::GlyphArrangement ga;
    ga.addLineOfText(font, name, x, y);     
    juce::Path p;
    ga.createPath(p);

    auto pathBounds = p.getBounds();

    p.applyTransform(juce::AffineTransform().rotated(juce::MathConstants<float>::halfPi,
        pathBounds.getCentreX(), pathBounds.getCentreY()));
    
    g.setColour(colour);
    g.fillPath(p);
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


    styleVerticalSlider(attackSlider, 0.1f, 10.0f, 1.0f); //in ms
    attackSlider.addListener(this);
    addAndMakeVisible(&attackSlider);

    styleVerticalSlider(releaseSlider, 0.1f, 100.0f, 1.0f); 
    releaseSlider.addListener(this);
    addAndMakeVisible(&releaseSlider);

    styleLabel(pitchLabel, "PITCH");
    addAndMakeVisible(&pitchLabel);
    styleVerticalSlider(pitchSlider, -12, 12, 0, 1); 
    pitchSlider.addListener(this);
    addAndMakeVisible(&pitchSlider);

    styleLabel(resonanceLabel, "RES");
    addAndMakeVisible(&resonanceLabel);
    styleVerticalSlider(resonanceSlider, 1, 300, 100);
    resonanceSlider.addListener(this); 
    addAndMakeVisible(&resonanceSlider);


    styleVerticalSlider(spreadSlider, 1, 3, 1);
    spreadSlider.addListener(this);
    addAndMakeVisible(&spreadSlider);

    styleLabel(spreadLabel, "SPREAD");
    spreadLabel.attachToComponent(&spreadSlider, false); 
    addAndMakeVisible(&spreadLabel);


    styleHorizontalSlider(shapeSlider, 0, 1, 0);
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

       
    //Background Colour
    g.fillAll(customColours::eerieBlack);
    
    g.setColour(customColours::isabelline);
    auto window = getBounds().toFloat();
    auto rectTopPadding = 25.0f;
    auto rectBottomPadding = 120.0f;
    auto rectLeftPadding = 25.0f;
    auto rectCornerSize = 10.0f;
    g.fillRoundedRectangle(rectLeftPadding, rectTopPadding, window.getWidth() / 4,
        window.getHeight() - rectBottomPadding, rectCornerSize);


    g.fillRoundedRectangle((window.getWidth() / 4) + rectLeftPadding * 2, rectTopPadding, window.getWidth() / 4,
        window.getHeight() - rectBottomPadding, rectCornerSize);

    juce::Font font = juce::Font("Arial", 22.0f, juce::Font::bold);

    juce::Colour fontColour = juce::Colours::grey;

    drawVerticalLabel(g, font, fontColour, juce::String("ATTACK"),
        gridUI::columnOne, gridUI::rowOne);

    drawVerticalLabel(g, font, fontColour, juce::String("RELEASE"),
        gridUI::columnTwo, gridUI::rowOne);
}

void ReSoundAudioProcessorEditor::resized()
{
    auto keyboardHeight = 64;
    keyboardComponent.setBounds(0, getHeight()-keyboardHeight, getWidth(), keyboardHeight);

    auto dialSize = 70;

    auto verticalSliderHeight = 150;
    auto verticalSliderWidth = 20;
    auto horizontalSliderWidth = verticalSliderHeight;
    auto horizontalSliderHeight = verticalSliderWidth;

    auto labelHeight = 30;

    auto rowOne = 100;

    auto rowTwo = 300;


    attackSlider.setBounds(gridUI::columnOne, gridUI::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    releaseSlider.setBounds(gridUI::columnTwo, gridUI::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    //pitchSlider.setBounds(columnOne, rowTwo, verticalSliderWidth, verticalSliderHeight);

    //resonanceSlider.setBounds(columnOne*2, rowOne, verticalSliderWidth, verticalSliderHeight);
    //resonanceLabel.setBounds(columnOne * 2, rowOne + verticalSliderHeight, 100, 100);

    //spreadSlider.setBounds(columnTwo, rowOne, verticalSliderWidth, verticalSliderHeight);

    //shapeSlider.setBounds(200, 300, horizontalSliderWidth, horizontalSliderHeight);
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
    else if (slider == &attackSlider)
    {
        audioProcessor.setAttack(slider->getValue());
    }
    else if (slider == &releaseSlider)
    {
        audioProcessor.setRelease(slider->getValue());
    }
}
