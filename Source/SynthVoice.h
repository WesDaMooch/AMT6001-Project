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

    void setSpread(double newSpread);
    void setShape(double newShape);
    void setExciterAttack(double newExciterAttack);
    void setExciterRelease(double newExciterRelease);
    void setExciterNoiseAmount(double newExciterNoiseAmount);

private:

    double sampleRate = 44100; 

    juce::ADSR exciter;

    double fundimentalFreq = 130;
    double fundimentalRes = 200; //this will be decay soon

    //Params
    //HARMO 1 - numResonators (6)
    //defaults here???
    double spread = 1;
    double shape = 0;

    // Exciter
    double exciterAttack = 1.0f; //in ms (*0.001 for s)
    double exciterRelease = 1.0f;
    double exciterNoiseAmount = 0.0f;
    juce::Random randomFloat;

    //maybe use this for punch env?
    juce::dsp::BallisticsFilter<float> exciterShape; 

    //Vibrational modes of a circular membrane
    //after Berg and Stork - who dis?
    //is there a way of calulating nth mode - wave equation help
    std::array<float, 5> circularModes = {1.59f, 2.14f, 2.3f, 2.65f, 2.92f}; //add 12 or 16

    //IIR Filter Bank
    const int numResonators = 6;
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> filterBank;
    std::vector<juce::AudioBuffer<float>> bufferBank;
    //std::vector<double> resBank;
    std::vector<juce::dsp::Gain<float>> gainBank;


    //Helper func
    float basicLerp(float a, float b, float t);
};

