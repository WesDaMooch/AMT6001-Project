/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthSound.h"

namespace Constants
{
    const int num_voices = 16;
}

//==============================================================================
ReSoundAudioProcessor::ReSoundAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), exciterAttackRange(0.1f, 10.0f)
                        
                        
#endif
{   
    for (int i = 0; i < Constants::num_voices; i++)
    {
        // Add voices
        synth.addVoice(new SynthVoice());
    }

    synth.clearSounds();
    synth.addSound(new SynthSound());

    exciterAttack = new juce::AudioParameterFloat({ "attack", 1 }, "Exciter Attack",
        float(exciterAttackRange.start), float(exciterAttackRange.end), 1.0f);
    addParameter(exciterAttack);
    
}

ReSoundAudioProcessor::~ReSoundAudioProcessor()
{
}

//==============================================================================
const juce::String ReSoundAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReSoundAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReSoundAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReSoundAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReSoundAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReSoundAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReSoundAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReSoundAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReSoundAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReSoundAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReSoundAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{   
    // Set Synth SampleRate
    synth.setCurrentPlaybackSampleRate(sampleRate);                    
    midiMessageCollector.reset(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    //spec.numChannels = getTotalNumInputChannels();    // THIS CAUSES AN ERROR, WHY DOES IT NOT RETURN 2??
    spec.numChannels = 2;   //

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        SynthVoice* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i));
        if (voice != nullptr)
        {
            voice->prepare(spec);
        }
    }
}

void ReSoundAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReSoundAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ReSoundAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear Midi Queue
    midiMessageCollector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());

    // Get Midi Message and convert to Hz
    double fundimentalFreq;
    for (juce::MidiMessageMetadata metadata : midiMessages)
    {
        if (metadata.numBytes > 0)
        {
            juce::MidiMessage message = metadata.getMessage();
            if (message.isNoteOn())
            {
                int midiNote = message.getNoteNumber() + pitchOffset;
                fundimentalFreq = message.getMidiNoteInHertz(midiNote); // Convert MIDI to Hz
                break;
            }
        }
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        SynthVoice* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i));
        if (voice != nullptr)
        {
            // SynthVoice sets
            voice->setFundamentalFreq(fundimentalFreq);
           
            voice->setExciterAttack(exciterAttack->get());
            voice->setExciterRelease(exciterRelease);
            voice->setExciterNoiseAmount(exciterNoiseAmount);
            voice->setPunchRelease(punchAmount);
            
            voice->setHarmo(harmonics);
            voice->setDecay(decay);
            voice->setSpread(spread);
            voice->setShape(shape);

            voice->setOutputGainValue(outputGain);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool ReSoundAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReSoundAudioProcessor::createEditor()
{
    return new ReSoundAudioProcessorEditor (*this);
}

//==============================================================================
void ReSoundAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ReSoundAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReSoundAudioProcessor();
}

juce::MidiMessageCollector& ReSoundAudioProcessor::getMidiMessageCollector()
{
    return midiMessageCollector;
}

//==============================================================================
// Param setters
// Resonator
void ReSoundAudioProcessor::setDecay(double newDecay) { decay = static_cast<float>(newDecay); }
void ReSoundAudioProcessor::setHarmonics(double newHarmonics) { harmonics = static_cast<float>(newHarmonics); }
void ReSoundAudioProcessor::setSpread(double newSpread) { spread = static_cast<float>(newSpread); }
void ReSoundAudioProcessor::setShape(double newShape) { shape = static_cast<float>(newShape); }
void ReSoundAudioProcessor::setPitchOffset(double newPitchOffset) { pitchOffset = static_cast<float>(newPitchOffset); }

// Exciter
// Attack exposed to host
double ReSoundAudioProcessor::getExciterAttack()
{
    return exciterAttack->get();
}

void ReSoundAudioProcessor::setExciterAttack(double newAttack)
{
    *exciterAttack = static_cast<float>(newAttack);
}

juce::NormalisableRange<double> ReSoundAudioProcessor::getExciterAttackRange()
{
    return exciterAttackRange;
}
juce::AudioParameterFloat* ReSoundAudioProcessor::getExciterAttackParameter()
{
    return exciterAttack;
}
void ReSoundAudioProcessor::setExciterRelease(double newExciterRelease) { exciterRelease = static_cast<float>(newExciterRelease); }
void ReSoundAudioProcessor::setExciterNoiseAmount(double newExciterNoiseAmount) { exciterNoiseAmount = static_cast<float>(newExciterNoiseAmount); }
void ReSoundAudioProcessor::setPunchAmount(double newPunchAmount) { punchAmount = static_cast<float>(newPunchAmount); }
void ReSoundAudioProcessor::setOutputGain(double newOutputGain) { outputGain = static_cast<float>(newOutputGain); }
