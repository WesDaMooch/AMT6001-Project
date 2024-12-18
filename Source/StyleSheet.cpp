

#include "StyleSheet.h"

namespace juce
{
	void CustomLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
	{
        auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
        auto fill = slider.findColour(Slider::rotarySliderFillColourId);

        auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

        auto makeSmaller = 0.5f;

        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin(8.0f, radius * 0.5f)*makeSmaller;
        auto arcRadius = radius - lineW * 0.5f;

        //Background of dial arc
        Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            rotaryEndAngle,
            true);

        g.setColour(outline);
        g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

        //Arc that follows dial indicator
        if (slider.isEnabled())
        {
            Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                rotaryStartAngle,
                toAngle,
                true);

            g.setColour(fill);
            g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
        }

        //Dial indicator line
        auto linePadding = 4.0f;
        Point<float> thumbPoint(bounds.getCentreX() + (arcRadius - linePadding) * std::cos(toAngle - MathConstants<float>::halfPi),
            bounds.getCentreY() + (arcRadius - linePadding) * std::sin(toAngle - MathConstants<float>::halfPi));

        g.setColour(slider.findColour(Slider::thumbColourId));
        g.drawLine(backgroundArc.getBounds().getCentreX(), backgroundArc.getBounds().getCentreY(),
            thumbPoint.getX(), thumbPoint.getY(), lineW);
	}
}
