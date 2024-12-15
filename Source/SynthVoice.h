#pragma once

#include <JuceHeader.h>

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();

private:
    juce::ADSR exciter;

    juce::dsp::StateVariableTPTFilterType bandpassType = juce::dsp::StateVariableTPTFilterType::bandpass;
    double cutoff = 440;
    double res = 10;
    juce::dsp::StateVariableTPTFilter<float> firstResonator;

    juce::AudioBuffer<float> processingBuffer;
};
