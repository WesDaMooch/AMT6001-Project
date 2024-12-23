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

    void setResonatorAmount(double newResonatorAmount);
    void setSpread(double newSpread);
    void setShape(double newShape);
    void setExciterAttack(double newExciterAttack);
    void setExciterRelease(double newExciterRelease);
    void setExciterNoiseAmount(double newExciterNoiseAmount);

private:

    double sampleRate = 44100; 

    juce::ADSR exciter;

    int numResonators = 6; //need a attenuator thing, gain = 1/numResonators
    const int maxResonators = 6;
    // Attenuates output of filter bank based on active filters (numResonators)
    juce::dsp::Gain<float> filterBankAttenuator;    // 0 to 1

    double fundimentalFreq = 130;
    double fundimentalRes = 200; //this will be decay soon
    //  Makeup gain for high resonance
    std::vector<juce::dsp::Gain<float>> resonatorMakeUpGainBank;    //  1 - 300

    //Params
    double harmo = 6;
    //  Allows harmonics (resonators) to be added smoothly, instead of setting with an int numResonators
    std::vector<juce::dsp::Gain<float>> harmoAttenuatorBank;    // 0 - 1

    double spread = 1;
    double shape = 0;

    // Exciter
    double exciterAttack = 1.0f; //in ms (*0.001 for s)
    double exciterRelease = 1.0f;
    double exciterNoiseAmount = 0.0f;
    juce::Random randomFloat;

    //maybe use this for punch env?
    juce::dsp::BallisticsFilter<float> exciterShape; 

    // Vibrational modes of a circular membrane
    std::array<float, 5> circularModes = {1.59f, 2.14f, 2.3f, 2.65f, 2.92f}; //add 12 or 16

    //IIR Filter Bank
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> filterBank;
    std::vector<juce::AudioBuffer<float>> bufferBank;
    //std::vector<double> resBank;


    //Helper func
    float basicLerp(float a, float b, float t);
};

