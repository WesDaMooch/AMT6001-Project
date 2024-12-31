/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
/*
* ISSUES
* One parameter, Exciter Attack has been made open to the host DAW,
* however modulation of this parameter causes the plugin or DAW to crash.
* Tested on FL Studio 20, no way to test in another DAW - maybe it will work in Ableton...
* Have not opened up other parameters to the host as the current method has problems.
* 
* Can click when changing HARMO param quickly - fix by having two numResonates varables, one for updateParameters
* and another for renderNextBlock, stop using the same variable for both...
*
* CPU usage seems pretty bad...
* 
* 
* TO-DO LIST
* Fix exciter attack slider crash and expose all sliders to host.
* Replace UI Keyboard with a trigger button.
* 
* Add decompose section, ways to take the generated sound apart in time and space - this was a big part of the plugin concept.
* Slider 'SPACE' or 'STEREO' randomly places a harmonic overtone (resonant filter) in the L or R buffer.
* 'DECOMP' slider delays harmonics in time. 
* Can make wind chime sounds and can get a bit granular with short decay times (tested in Max/MSP).
* 
* Lots of UI impovements can be made - a sytle a bit like Shakmat Modular with the diagonal stripes.
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
    void setDecay(double newDecay); 
    void setHarmonics(double newHarmonics);
    void setSpread(double newSpread);
    void setShape(double newShape);
    void setPitchOffset(double newPitchOffset);

    double getExciterAttack();                                
    void setExciterAttack(double newAttack);                       
    juce::NormalisableRange<double> getExciterAttackRange();
    juce::AudioParameterFloat* getExciterAttackParameter();

    void setExciterRelease(double newExciterRelease);
    void setExciterNoiseAmount(double newExciterNoiseAmount);
    void setPunchAmount(double newPunchAmount);

    void setOutputGain(double newOutputGain);

private:
    //==============================================================================
    juce::Synthesiser synth;

    juce::MidiMessageCollector midiMessageCollector;
    int pitchOffset = 0;

    juce::AudioParameterFloat* exciterAttack;
    juce::NormalisableRange<double> exciterAttackRange;
    float exciterRelease = 1;
    float exciterNoiseAmount = 0;
    float punchAmount = 0;

    float decay;
    float harmonics = 6;
    float spread = 1;
    float shape = 0;
    
    float outputGain = 0.7;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReSoundAudioProcessor)
};
