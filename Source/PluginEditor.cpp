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
    //juce::Colour isabelline = juce::Colour(242, 239, 234);
    juce::Colour isabelline = juce::Colour(221, 221, 221);
    juce::Colour platinum = juce::Colour(221, 221, 221);
    juce::Colour timberwolf = juce::Colour(214, 214, 214);

    //Dark Greys
    //juce::Colour gray33 = juce::Colour(84, 84, 84);
    juce::Colour gray33 = juce::Colour(59,59,59);
    juce::Colour jet = juce::Colour(60, 55, 68);
    juce::Colour gunmetal = juce::Colour(43, 47, 59);
    //juce::Colour eerieBlack = juce::Colour(31, 31, 31);
    juce::Colour eerieBlack = juce::Colours::grey;

    //Accent Colours
    juce::Colour coquelicot = juce::Colour(241, 80, 37);
}

namespace gridUI
{
    namespace exciterBox
    {
        const float columnOne = 40;
        const float columnTwo = 100;

        const float rowOne = 120;
        const float rowTwo = 300;

    }
    const float columnOne = 40;
    const float columnTwo = columnOne + 60;
    const float columnThree = 210;
    const float columnFour = columnThree + 60;
    const float columnFive = columnFour + 60;
    
    const float rowZero = 20;
    const float rowOne = 100;
    const float rowTwo = 300;
    const float rowThree = rowTwo + 60;
}

//maybe remove
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
    slider.setColour(juce::Slider::ColourIds::trackColourId, customColours::coquelicot);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, customColours::gray33);
    slider.setColour(juce::Slider::ColourIds::backgroundColourId, customColours::eerieBlack);
    slider.setRange(minRange, maxRange, interval);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}

void styleHorizontalSlider(juce::Slider& slider, double minRange, double maxRange, double defaultValue,
    double interval = 0)
{
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setColour(juce::Slider::ColourIds::trackColourId, customColours::eerieBlack);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, customColours::gray33);
    slider.setColour(juce::Slider::ColourIds::backgroundColourId, customColours::eerieBlack);
    slider.setRange(minRange, maxRange, interval);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}


void drawVerticalLabel(juce::Graphics& g, juce::Font font, juce::Colour colour,
    const juce::String name, float targetSliderX, float targetSliderY, double value, int precision=2)
{
    //draw a vertical label for slider

    //make value box bigger...

    if (value >= 100)
        precision = 0;
    else if (value >= 10 && value < 100)
        precision = 1;
    else
        precision = 2;


    auto sliderSize = 133; 

    // Convert double to string with decimal precision
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    std::string valueString = stream.str();

    float valueWidth = juce::Font().getStringWidth(valueString);

    //float fontWidth = font.getStringWidthFloat(name);
    float horizontalLabelPadding = 38.0f;
    float verticalValuePadding = 40.0f;

    float labelX = targetSliderX + horizontalLabelPadding;
    float labelY = targetSliderY + 20.0f;

    float valueX = targetSliderX + sliderSize;
    float valueY = targetSliderY + 20.0f; 

    juce::GlyphArrangement ga;
    ga.addLineOfText(font, name, labelX, labelY);
    ga.addJustifiedText(font, valueString, valueX, valueY, verticalValuePadding,
        juce::Justification::right);
    juce::Path p;
    ga.createPath(p);

    auto pathBounds = p.getBounds();

    p.applyTransform(juce::AffineTransform().rotated(juce::MathConstants<float>::halfPi,
        pathBounds.getX(), pathBounds.getY()));

    g.setColour(colour);
    g.fillPath(p);
}

void drawHorizontalLabel(juce::Graphics& g, juce::Font font, juce::Colour colour,
    const juce::String name, float targetSliderX, float targetSliderY, double value, int precision=2)
{
    //draw a vertical label for slider
    auto sliderSize = 133; // get this from somewhere, put in gridUI?

    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    std::string valueString = stream.str();

    float valueWidth = juce::Font().getStringWidthFloat(valueString);

    float fontWidth = font.getStringWidthFloat(name);
    float fontPadding = 10.0f;

    float x = targetSliderX + 5.0f;
    float y = targetSliderY - fontPadding;

    float valueX = x + sliderSize - valueWidth;
    float valueY = y;

    juce::GlyphArrangement ga;
    ga.addLineOfText(font, name, x, y);
    ga.addLineOfText(font, valueString, valueX, valueY);
    
    juce::Path p;
    ga.createPath(p);
    auto pathBounds = p.getBounds();

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

    setSize (500, 570);
    setResizable(true, true); 

    //UI elements
    addAndMakeVisible(&keyboardComponent);
    keyboardState.addListener(&audioProcessor.getMidiMessageCollector());

    // Exciter Attack
    styleVerticalSlider(attackSlider, 0.1f, 10.0f, 1.0f); //in ms
    attackSlider.addListener(this);
    addAndMakeVisible(&attackSlider);
    // Exciter Release
    styleVerticalSlider(releaseSlider, 0.1f, 100.0f, 1.0f); 
    releaseSlider.addListener(this);
    addAndMakeVisible(&releaseSlider);
    // Exciter Noise Amount
    styleVerticalSlider(exciterNoiseAmountSlider, 0.0f, 1.0f, 0.0f);
    exciterNoiseAmountSlider.addListener(this);
    addAndMakeVisible(&exciterNoiseAmountSlider);
    // Punch Amount
    styleVerticalSlider(punchAmountSlider, 0.0f, 100.0f, 0.0f);
    punchAmountSlider.addListener(this);
    addAndMakeVisible(&punchAmountSlider);

    //Harmo Amount
    styleVerticalSlider(harmoSlider, 1.0f, 12.0f, 6.0f);
    harmoSlider.addListener(this);
    addAndMakeVisible(&harmoSlider);
    // Res
    styleVerticalSlider(resonanceSlider, 1.0f, 300.0f, 100.0f);
    resonanceSlider.addListener(this); 
    addAndMakeVisible(&resonanceSlider);
    // Spread Amount
    styleVerticalSlider(spreadSlider, 0.5f, 3.0f, 1.0f);
    spreadSlider.addListener(this);
    addAndMakeVisible(&spreadSlider);
    // Shape
    styleHorizontalSlider(shapeSlider, 0.0f, 1.0f, 0.0f);
    shapeSlider.addListener(this);
    addAndMakeVisible(&shapeSlider);
    // Pitch Offset
    styleHorizontalSlider(pitchSlider, -12, 12, 0, 1);
    pitchSlider.addListener(this);
    addAndMakeVisible(&pitchSlider);
}

ReSoundAudioProcessorEditor::~ReSoundAudioProcessorEditor()
{
    // Clean up listeners
    keyboardState.removeListener(&audioProcessor.getMidiMessageCollector());
    // Reset LookAndFeel to default
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void ReSoundAudioProcessorEditor::paint (juce::Graphics& g)
{

       
    //Background Colour
    //g.fillAll(customColours::eerieBlack);
    g.fillAll(juce::Colour(31, 31, 31));

    g.setColour(customColours::isabelline);

    // Draw Titel
    juce::Font font = juce::Font("Bahnschrift", 22.0f, juce::Font::bold);

    g.setFont(font);
    g.drawText(juce::String("ReSound"), 10, 10, 100, juce::Justification::centredLeft, false);

    auto window = getBounds().toFloat();

    auto rectOneSize = 140.0f;
    auto rectTwoSize = 200.0f;

    auto rectTopPadding = 25.0f;
    auto rectBottomPadding = 120.0f;
    auto rectLeftPadding = 25.0f;
    auto rectCornerSize = 10.0f;

    auto exciterBoxX = 25.0f;
    auto exciterBoxY = 100.0f;
    auto exciterBoxWidth = 140.0f;
    auto exciterBowHeight = 365;
        

    // Exciter box
    //g.fillRoundedRectangle(rectLeftPadding, 100, rectOneSize,
        //490 - rectBottomPadding, rectCornerSize);

    g.fillRoundedRectangle(exciterBoxX, exciterBoxY, exciterBoxWidth,
        exciterBowHeight, rectCornerSize);
    //window.getHeight()

    g.fillRoundedRectangle(rectOneSize + rectLeftPadding * 2, rectTopPadding, rectTwoSize,
        window.getHeight() - rectBottomPadding, rectCornerSize);

    // Draw horizontal lines
    g.setColour(juce::Colours::grey);
    //const float dashLength[] = { 5, 5 };
    //g.drawDashedLine(juce::Line<float>(0, 0, 100, 100), dashLength, 2, 1.5f);

    // Draw Labels
    // Bahnschrift is a good font, is it on mac?
    //juce::Font font = juce::Font("Bahnschrift", 22.0f, juce::Font::bold);
    juce::Colour fontColour = juce::Colours::grey;

    drawVerticalLabel(g, font, fontColour, juce::String("ATK"),
        gridUI::exciterBox::columnOne, gridUI::exciterBox::rowOne, attackSlider.getValue());

    drawVerticalLabel(g, font, fontColour, juce::String("REL"),
        gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowOne, releaseSlider.getValue(), 0);

    drawVerticalLabel(g, font, fontColour, juce::String("NOISE"),
        gridUI::exciterBox::columnOne, gridUI::exciterBox::rowTwo, exciterNoiseAmountSlider.getValue());

    drawVerticalLabel(g, font, fontColour, juce::String("PUNCH"),
        gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowTwo, punchAmountSlider.getValue());

    drawVerticalLabel(g, font, fontColour, juce::String("HARMO"),
        gridUI::columnThree, gridUI::rowOne, harmoSlider.getValue());

    drawVerticalLabel(g, font, fontColour, juce::String("RES"),
        gridUI::columnFour, gridUI::rowOne, resonanceSlider.getValue(), 0);

    drawVerticalLabel(g, font, fontColour, juce::String("SPREAD"),
        gridUI::columnFive, gridUI::rowOne, spreadSlider.getValue());

    drawHorizontalLabel(g, font, fontColour, juce::String("SHAPE"),
        gridUI::columnThree, gridUI::rowTwo, shapeSlider.getValue());

    drawHorizontalLabel(g, font, fontColour, juce::String("PITCH"),
        gridUI::columnThree, gridUI::rowThree, pitchSlider.getValue(), 0);
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

    // Exciter
    attackSlider.setBounds(gridUI::exciterBox::columnOne, gridUI::exciterBox::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    releaseSlider.setBounds(gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    exciterNoiseAmountSlider.setBounds(gridUI::exciterBox::columnOne, gridUI::exciterBox::rowTwo,
        verticalSliderWidth, verticalSliderHeight);

    punchAmountSlider.setBounds(gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowTwo,
        verticalSliderWidth, verticalSliderHeight);

    // Resonant Body
    harmoSlider.setBounds(gridUI::columnThree, gridUI::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    resonanceSlider.setBounds(gridUI::columnFour, gridUI::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    spreadSlider.setBounds(gridUI::columnFive, gridUI::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    shapeSlider.setBounds(gridUI::columnThree, gridUI::rowTwo,
        horizontalSliderWidth, horizontalSliderHeight);

    pitchSlider.setBounds(gridUI::columnThree, gridUI::rowThree,
        horizontalSliderWidth, horizontalSliderHeight);
}

void ReSoundAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // editor -> processor -> voice
    if (slider == &harmoSlider)
    {
            audioProcessor.setHarmonics(slider->getValue());
    }
    else if (slider == &resonanceSlider)
    { 
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
    else if (slider == &pitchSlider)
    {
        audioProcessor.setPitchOffset(slider->getValue());
    }
    else if (slider == &attackSlider)
    {
        audioProcessor.setAttack(slider->getValue());
    }
    else if (slider == &releaseSlider)
    {
        audioProcessor.setRelease(slider->getValue());
    }
    else if (slider == &exciterNoiseAmountSlider)
    {
        audioProcessor.setExciterNoiseAmount(slider->getValue());
    }
    else if (slider == &punchAmountSlider)
    {
        audioProcessor.setPunchAmount(slider->getValue());
    }

    // Repaint UI if slider value change
    repaint();
}
