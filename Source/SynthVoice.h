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
    void setFundamentalFreq(double newFundimentalFreq);
    void setFundamentalRes(double newFundimentalRes);
    

private:

    double sampleRate = 44100; 

    juce::ADSR exciter;

    double fundimentalFreq = 130;
    double fundimentalRes = 200;
    juce::dsp::Gain<float> gain;

    //Params
    //SPREAD 1 - 3
    //HARMO 1 - numResonators (6)
    //SHAPE 0 - 1

    //IIR Filter Bank
    const int numResonators = 6;
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> filterBank;
    std::vector<juce::AudioBuffer<float>> bufferBank;
    std::vector<double> freqBank;
    std::vector<double> resBank;
    std::vector<juce::dsp::Gain<float>> gainBank;


};
