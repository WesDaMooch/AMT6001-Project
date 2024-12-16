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

    //internal update funtions
    void updateFundamentalResonator();

    //calls for UI
    void setFilter(double newFundimentalFreq);
    

private:

    double sampleRate = 44100; 

    juce::ADSR exciter;

    //SVF Resonator
    juce::dsp::StateVariableTPTFilterType bandpassType = juce::dsp::StateVariableTPTFilterType::bandpass;
    
    double res = 20;
    juce::dsp::StateVariableTPTFilter<float> firstResonator;

    //IIR Resonator
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> fundamentalResonator;
    double fundimentalFreq = 130;
    double fundimentalRes = 20; 
    juce::dsp::Gain<float> gain;

    juce::AudioBuffer<float> processingBuffer;
};
