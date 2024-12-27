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
                       )
#endif
{   
    for (int i = 0; i < Constants::num_voices; i++)
    {
        //add voices
        synth.addVoice(new SynthVoice());
    }

    synth.clearSounds();
    synth.addSound(new SynthSound());
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
    //Set Synth SampleRate
    synth.setCurrentPlaybackSampleRate(sampleRate);                    
    midiMessageCollector.reset(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    //spec.numChannels = getTotalNumInputChannels();
    spec.numChannels = 2;

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


    //Clear Midi Queue
    midiMessageCollector.removeNextBlockOfMessages(midiMessages, buffer.getNumSamples());

    //Get Midi Message and convert to 
    double fundimentalFreq;
    for (juce::MidiMessageMetadata metadata : midiMessages)
    {
        if (metadata.numBytes > 0)
        {
            juce::MidiMessage message = metadata.getMessage();
            if (message.isNoteOn())
            {
                int midiNote = message.getNoteNumber() + pitchOffset;
                fundimentalFreq = message.getMidiNoteInHertz(midiNote);
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
            //sets
            //set first reson freq with midi
            voice->setFundamentalFreq(fundimentalFreq);
            voice->setResonatorAmount(harmonics);

            voice->setExciterAttack(attack);
            voice->setExciterRelease(release);
            voice->setExciterNoiseAmount(exciterNoiseAmount);
            voice->setPunchRelease(punchAmount);
            
            voice->setFundamentalRes(res);
            voice->setSpread(spread);
            voice->setShape(shape);
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
// Body
void ReSoundAudioProcessor::setRes(double newRes) { res = newRes; }
void ReSoundAudioProcessor::setHarmonics(double newHarmonics) { harmonics = newHarmonics; }
void ReSoundAudioProcessor::setSpread(double newSpread) { spread = newSpread; }
void ReSoundAudioProcessor::setShape(double newShape) { shape = newShape; }
void ReSoundAudioProcessor::setPitchOffset(double newPitchOffset) { pitchOffset = newPitchOffset; }
// Exciter
void ReSoundAudioProcessor::setAttack(double newAttack) { attack = newAttack; }
void ReSoundAudioProcessor::setRelease(double newRelease) { release = newRelease; }
void ReSoundAudioProcessor::setExciterNoiseAmount(double newExciterNoiseAmount) { exciterNoiseAmount = newExciterNoiseAmount; }
void ReSoundAudioProcessor::setPunchAmount(double newPunchAmount) { punchAmount = newPunchAmount; }
