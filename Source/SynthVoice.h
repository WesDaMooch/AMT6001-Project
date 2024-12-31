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

    // Internal update funtion
    void updateParameters();

    // Calls for silders
    void setFundamentalFreq(float newFundimentalFreq);
    void setDecay(float newDecay);
    void setHarmo(float newHarmo);
    void setSpread(float newSpread);
    void setShape(float newShape);
    void setExciterAttack(float newExciterAttack);
    void setExciterRelease(float newExciterRelease);
    void setExciterNoiseAmount(float newExciterNoiseAmount);
    void setPunchRelease(float newPunchRelease);
    void setOutputGainValue(float newsOutputGainValue);

private:
    float sampleRate = 44100; 

    const int maxResonators = 12;

    float fundimentalFreq = 130;

    // Exciter
    juce::ADSR exciter;
    float exciterAttack = 1.0f; // In ms (* 0.001 for s)
    float exciterRelease = 1.0f;
    float exciterNoiseAmount = 0.0f;
    juce::Random randomFloat;
    // Punch
    juce::ADSR punchModulationEnvelope;
    float punchRelease = 0;
    float punchDepth = 100.0f;  // Depth of filter cutoff modulation in Hz

    // Params
    float harmo = 6;
    int numResonators = 6;
    std::vector<juce::dsp::Gain<float>> harmoAttenuatorBank;
    float decay = 200;
    std::vector<juce::dsp::Gain<float>> resonatorMakeUpGainBank;    // Makeup gain for high resonance, 1 - 300
    float spread = 1;
    float shape = 0;

    float outputGainValue = 0.7f;
    juce::dsp::Gain<float> outputGain;
    juce::dsp::WaveShaper<float> softClipper; //tanh(x)

    // Vibrational modes of a circular membrane
    std::array<float, 11> circularModes = {1.59f, 2.14f, 2.3f, 2.65f, 2.92f,
    3.16, 3.5, 3.6, 3.65, 4.06, 4.15}; 

    std::vector<float> freqBank;
    std::vector<float> qBank;   // Resonance is currently all set the same for all filters, this will changes

    //  IIR Filter Bank
    std::vector<juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>> filterBank;
    std::vector<juce::AudioBuffer<float>> bufferBank;

    // Helper functions
    float linearInterpolator(float inputOne, float inputTwo, float mix); // use a JUCE Interpolator instead?
};

