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
    void setSpread(double newSpread);
    void setShape(double newShape);


private:
    //==============================================================================
    juce::Synthesiser synth;

    juce::MidiMessageCollector midiMessageCollector;
    
    //have defaults here?
    double res; 
    double spread; 
    double shape;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReSoundAudioProcessor)
};
