#pragma once
#include <JuceHeader.h>

/*
    ISSUES
    Can click when changing HARMO param quickly - fix by having two numResonates varables, one for updateParameters
    and another for renderNextBlock, stop using the same variable for both...
*/


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

    // Internal update funtions
    void updateParameters();

    // Calls for silders
    void setFundamentalFreq(double newFundimentalFreq);
    void setFundamentalRes(double newFundimentalRes);

    void setResonatorAmount(double newResonatorAmount);
    void setSpread(double newSpread);
    void setShape(double newShape);
    void setExciterAttack(double newExciterAttack);
    void setExciterRelease(double newExciterRelease);
    void setExciterNoiseAmount(double newExciterNoiseAmount);
    void setPunchRelease(double newPunchRelease);

private:
    float sampleRate = 44100; 

    juce::ADSR exciter;

    int numResonators = 6;
    const int maxResonators = 12;
    // Attenuates output of filter bank based on active filters (numResonators)
    juce::dsp::Gain<float> filterBankAttenuator;    // 0 to 1

    double fundimentalFreq = 130;


    std::vector<float> freqBank;
    std::vector<float> qBank;   
    double fundimentalRes = 200; //this will be decay soon
    std::vector<juce::dsp::Gain<float>> resonatorMakeUpGainBank;    //  Makeup gain for high resonance, 1 - 300

    //Params
    double harmo = 6;
    std::vector<juce::dsp::Gain<float>> harmoAttenuatorBank; 

    double spread = 1;
    double shape = 0;

    // Exciter
    double exciterAttack = 1.0f; //in ms (*0.001 for s)
    double exciterRelease = 1.0f;
    double exciterNoiseAmount = 0.0f;
    juce::Random randomFloat;
    // Punch
    juce::ADSR punchModulationEnvelope;
    float punchRelease = 0; 
    float punchDepth = 100.0f;  // Depth of filter cutoff modulation in Hz

    // Vibrational modes of a circular membrane
    std::array<float, 11> circularModes = {1.59f, 2.14f, 2.3f, 2.65f, 2.92f,
    3.16, 3.5, 3.6, 3.65, 4.06, 4.15}; 

    //  IIR Filter Bank
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> filterBank;
    std::vector<juce::AudioBuffer<float>> bufferBank;

    // Helper functions
    //USE JUCE Interpolator??
    float basicLerp(float a, float b, float t);
};

