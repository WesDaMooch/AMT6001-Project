/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace customColours
{
    const juce::Colour isabelline = juce::Colour(221, 221, 221);
    const juce::Colour gunmetal = juce::Colour(59, 59, 59);
    const juce::Colour eerieBlack = juce::Colour(31, 31, 31);
}

namespace gridUI
{
    namespace exciterBox
    {
        const float columnOne = 40;
        const float columnTwo = 100;

        const float rowOne = 125;
        const float rowTwo = 305;
    }
    namespace resonatorBox
    {
        const float columnOne = 210; 
        const float columnTwo = 270; 
        const float columnThree = 330;

        const float rowOne = 70; 
        const float rowTwo = 260; 
        const float rowThree = rowTwo + 60;
    }
    namespace outputGainBox
    {
        const float columnOne = 210;
        const float rowOne = 445;
    }
}

void styleVerticalSlider(juce::Slider& slider, double minRange, double maxRange, double defaultValue,
    double interval=0)
{
    // Could be put in CustomLookAndFeel
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, customColours::gunmetal);
    slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::grey);
    slider.setRange(minRange, maxRange, interval);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}

void styleHorizontalSlider(juce::Slider& slider, double minRange, double maxRange, double defaultValue,
    double interval = 0)
{
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::grey);
    slider.setColour(juce::Slider::ColourIds::thumbColourId, customColours::gunmetal);
    slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::grey);
    slider.setRange(minRange, maxRange, interval);
    slider.setValue(defaultValue);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
}

void drawVerticalLabel(juce::Graphics& g, juce::Font font, juce::Colour colour,
    const juce::String name, float targetSliderX, float targetSliderY, double value, int precision=2)
{
    // Draw a vertical label for slider

    // Calculate precision based on value
    // Always display three digits
    if (value >= 100)
        precision = 0;
    else if (value >= 10 && value < 100)
        precision = 1;
    else
        precision = 2;

    float sliderSize = 133; 

    // Convert double to string with decimal precision
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    std::string valueString = stream.str();

    float horizontalLabelPadding = 38.0f;
    float verticalValuePadding = 45.0f;

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

    // Rotate
    p.applyTransform(juce::AffineTransform().rotated(juce::MathConstants<float>::halfPi,
        pathBounds.getX(), pathBounds.getY()));

    g.setColour(colour);
    g.fillPath(p);
}

void drawHorizontalLabel(juce::Graphics& g, juce::Font font, juce::Colour colour,
    const juce::String name, float targetSliderX, float targetSliderY, double value, int precision=2)
{
    // Draw a horizontal label for slider
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
    // Use custom LookAndFeel Class
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLookAndFeel);

    setSize (420, 570);
    //setResizable(true, true); 

    // UI elements
    addAndMakeVisible(&keyboardComponent);
    keyboardState.addListener(&audioProcessor.getMidiMessageCollector());

    // Exciter Attack - this param is available to the host
    // It uses different get and set functions than the others
    //styleVerticalSlider(attackSlider, 0.1f, 10.0f, 1.0f); //in ms
    attackSlider.setNormalisableRange(audioProcessor.getExciterAttackRange());
    attackSlider.setValue(audioProcessor.getExciterAttack());
    attackSlider.setSliderStyle(juce::Slider::LinearVertical);
    attackSlider.setColour(juce::Slider::ColourIds::thumbColourId, customColours::gunmetal);
    attackSlider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::grey);
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
    // Harmo Amount
    styleVerticalSlider(harmoSlider, 1.0f, 12.0f, 6.0f);
    harmoSlider.addListener(this);
    addAndMakeVisible(&harmoSlider);
    // Decay
    styleVerticalSlider(decaySlider, 1.0f, 300.0f, 100.0f);
    decaySlider.addListener(this); 
    addAndMakeVisible(&decaySlider);
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
    // Output Gain
    styleHorizontalSlider(outputGainSlider, 0.0f, 1.5f, 0.7f);
    outputGainSlider.addListener(this);
    addAndMakeVisible(&outputGainSlider);

    for (auto* param : audioProcessor.getParameters())
    {
        param->addListener(this); //adding listerner for host (DAW) to see it... 
    }
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
    auto window = getBounds().toFloat();

    auto fontHeight = 40.0f;
    auto titelY = 28.0f;

    auto boxCornerSize = 10.0f;

    auto exciterTitelY = 75.0f;
    auto exciterBoxX = 25.0f;
    auto exciterBoxY = 80.0f;
    auto exciterBoxWidth = 140.0f;
    auto exciterBoxHeight = 400.0f;

    auto resonatorTitelY = 20.0f;
    auto resonatorBoxX = 190.0f;
    auto resonatorBoxY = 25.0f;
    auto resonatorBoxWidth = 200.0f;
    auto resonatorBoxHeight = 350.0f;

    auto gainBoxX = 190.0f;
    auto gainBoxY = 400.0f;
    auto gainBoxWidth = 200.0f;
    auto gainBoxHeight = 80.0f;

    // Background Colour
    g.fillAll(customColours::eerieBlack);
   
    // Exciter box
    g.setColour(customColours::isabelline); // Box colour
    g.fillRoundedRectangle(exciterBoxX, exciterBoxY, exciterBoxWidth,
        exciterBoxHeight, boxCornerSize);
    // Resonator box
    g.fillRoundedRectangle(resonatorBoxX, resonatorBoxY, resonatorBoxWidth,
        resonatorBoxHeight, boxCornerSize);

    // Gain box
    g.fillRoundedRectangle(gainBoxX, gainBoxY, gainBoxWidth,
        gainBoxHeight, boxCornerSize);


    // Draw horizontal lines
    //const float dashLength[] = { 5, 5 };
    //g.drawDashedLine(juce::Line<float>(0, 0, 100, 100), dashLength, 2, 1.5f);

    // Draw titel
    g.setColour(juce::Colours::grey);   // Titel colour
    juce::Font titelFont = juce::Font("Bahnschrift", 28.0f, juce::Font::bold);
    g.setFont(titelFont);
    g.drawText(juce::String("ReSound"), exciterBoxX, titelY, exciterBoxWidth, fontHeight, juce::Justification::centred, false);
    // Draw box titel
    juce::Font boxTitelFont = juce::Font("Bahnschrift", 22.0f, juce::Font::bold);
    g.setFont(boxTitelFont);
    g.drawText(juce::String("exciter"), exciterBoxX, exciterTitelY, exciterBoxWidth, fontHeight, juce::Justification::centredBottom, false);
    // Draw resonator titel
    g.drawText(juce::String("resonator"), resonatorBoxX, resonatorTitelY, resonatorBoxWidth, fontHeight, juce::Justification::centredBottom, false);

    // Draw Labels
    // Bahnschrift is a good font, is it on mac?
    juce::Font labelFont = juce::Font("Bahnschrift", 22.0f, juce::Font::bold);
    juce::Colour fontColour = juce::Colours::grey;

    drawVerticalLabel(g, labelFont, fontColour, juce::String("ATK"),
        gridUI::exciterBox::columnOne, gridUI::exciterBox::rowOne, attackSlider.getValue());

    drawVerticalLabel(g, labelFont, fontColour, juce::String("REL"),
        gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowOne, releaseSlider.getValue(), 0);

    drawVerticalLabel(g, labelFont, fontColour, juce::String("NOISE"),
        gridUI::exciterBox::columnOne, gridUI::exciterBox::rowTwo, exciterNoiseAmountSlider.getValue());

    drawVerticalLabel(g, labelFont, fontColour, juce::String("PUNCH"),
        gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowTwo, punchAmountSlider.getValue());

    drawVerticalLabel(g, labelFont, fontColour, juce::String("HARMO"),
        gridUI::resonatorBox::columnOne, gridUI::resonatorBox::rowOne, harmoSlider.getValue());

    drawVerticalLabel(g, labelFont, fontColour, juce::String("DECAY"),
        gridUI::resonatorBox::columnTwo, gridUI::resonatorBox::rowOne, decaySlider.getValue(), 0);

    drawVerticalLabel(g, labelFont, fontColour, juce::String("SPREAD"),
        gridUI::resonatorBox::columnThree, gridUI::resonatorBox::rowOne, spreadSlider.getValue());

    drawHorizontalLabel(g, labelFont, fontColour, juce::String("SHAPE"),
        gridUI::resonatorBox::columnOne, gridUI::resonatorBox::rowTwo, shapeSlider.getValue());

    drawHorizontalLabel(g, labelFont, fontColour, juce::String("PITCH"),
        gridUI::resonatorBox::columnOne, gridUI::resonatorBox::rowThree, pitchSlider.getValue(), 0);

    drawHorizontalLabel(g, labelFont, fontColour, juce::String("GAIN"),
        gridUI::outputGainBox::columnOne, gridUI::outputGainBox::rowOne, outputGainSlider.getValue());
}

void ReSoundAudioProcessorEditor::resized()
{
    auto keyboardHeight = 64.0f;
    keyboardComponent.setBounds(0, getHeight()-keyboardHeight, getWidth(), keyboardHeight);

    auto verticalSliderHeight = 150.0f;
    auto verticalSliderWidth = 20.0f;
    auto horizontalSliderWidth = verticalSliderHeight + 10.0f;
    auto horizontalSliderHeight = verticalSliderWidth;

    auto labelHeight = 30.0f;

    // Exciter sliders
    attackSlider.setBounds(gridUI::exciterBox::columnOne, gridUI::exciterBox::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    releaseSlider.setBounds(gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    exciterNoiseAmountSlider.setBounds(gridUI::exciterBox::columnOne, gridUI::exciterBox::rowTwo,
        verticalSliderWidth, verticalSliderHeight);

    punchAmountSlider.setBounds(gridUI::exciterBox::columnTwo, gridUI::exciterBox::rowTwo,
        verticalSliderWidth, verticalSliderHeight);

    // Resonator sliders
    harmoSlider.setBounds(gridUI::resonatorBox::columnOne, gridUI::resonatorBox::rowOne,
        verticalSliderWidth, verticalSliderHeight);

   decaySlider.setBounds(gridUI::resonatorBox::columnTwo, gridUI::resonatorBox::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    spreadSlider.setBounds(gridUI::resonatorBox::columnThree, gridUI::resonatorBox::rowOne,
        verticalSliderWidth, verticalSliderHeight);

    shapeSlider.setBounds(gridUI::resonatorBox::columnOne, gridUI::resonatorBox::rowTwo,
        horizontalSliderWidth, horizontalSliderHeight);

    pitchSlider.setBounds(gridUI::resonatorBox::columnOne, gridUI::resonatorBox::rowThree,
        horizontalSliderWidth, horizontalSliderHeight);

    // Output gain slider
    outputGainSlider.setBounds(gridUI::outputGainBox::columnOne, gridUI::outputGainBox::rowOne,
        horizontalSliderWidth, horizontalSliderHeight);
}

void ReSoundAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // editor -> processor -> voice
    // Would like to set voice from editor, maybe this is a bad idea
    if (slider == &harmoSlider)
    {
            audioProcessor.setHarmonics(slider->getValue());
    }
    else if (slider == &decaySlider)
    { 
        audioProcessor.setDecay(slider->getValue());
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
        audioProcessor.setExciterAttack(slider->getValue());
    }
    else if (slider == &releaseSlider)
    {
        audioProcessor.setExciterRelease(slider->getValue());
    }
    else if (slider == &exciterNoiseAmountSlider)
    {
        audioProcessor.setExciterNoiseAmount(slider->getValue());
    }
    else if (slider == &punchAmountSlider)
    {
        audioProcessor.setPunchAmount(slider->getValue());
    }
    else if (slider == &outputGainSlider)
    {
        audioProcessor.setOutputGain(slider->getValue());
    }

    // Repaint UI if slider value changes
    repaint();
}

void ReSoundAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{   
    // Updates sliders with values sent from host
    auto* parameter = audioProcessor.getParameters()[parameterIndex];
    if (parameter == audioProcessor.getExciterAttackParameter())
    {
        auto range = audioProcessor.getExciterAttackRange();
        auto exciterAttackValue = range.convertFrom0to1(newValue);
        attackSlider.setValue(exciterAttackValue, juce::dontSendNotification);
    }
    
}

void ReSoundAudioProcessorEditor::parameterGestureChanged(int /*parameterIndex*/, bool /*gestureIsStarting*/)
{
}