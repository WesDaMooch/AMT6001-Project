/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"

//==============================================================================
/**
*/
class ReSoundAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ReSoundAudioProcessor();
    ~ReSoundAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    juce::MidiMessageCollector& getMidiMessageCollector();
    void setRes(double newRes); 
    void setHarmonics(double newHarmonics);
    void setSpread(double newSpread);
    void setShape(double newShape);
    void setPitchOffset(double newPitchOffset);

    void setAttack(double newAttack);
    void setRelease(double newRelease);
    void setExciterNoiseAmount(double newExciterNoiseAmount);

private:
    //==============================================================================
    juce::Synthesiser synth;

    juce::MidiMessageCollector midiMessageCollector;
    
    //have defaults here?
    double res; 
    double harmonics = 6;
    double spread = 1; 
    double shape = 0;
    int pitchOffset = 0;

    double attack = 1; //rename exciterAttack
    double release = 1; 
    double exciterNoiseAmount = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReSoundAudioProcessor)
};
