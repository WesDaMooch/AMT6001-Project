/*
*	Tutorial for custom LookAndFeel class
*	https://www.youtube.com/watch?v=iH9c4uNPLwE&ab_channel=TheAudioProgrammer
*/

#pragma once
#include <JuceHeader.h>

namespace juce
{

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
	void drawLinearSlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider& slider);
};
}
