#include "SynthVoice.h"
#include "SynthSound.h"

//what the hell is this
//Initalise Resonant Filters
SynthVoice::SynthVoice() //:fundamentalResonator(juce::dsp::IIR::Coefficients<float>::makeBandPass(44100, 130, 100))
{   
    gain.setGainLinear(10.0f);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    //need a way to start note
    //exciter env.noteOn?
    exciter.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    //need a way to end note and clearCurrentNote()
    exciter.noteOff();
    //if (!exciter.isActive())
    //{
        //clearCurrentNote();
    //}
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    auto numChannels = outputBuffer.getNumChannels();
    processingBuffer.setSize(numChannels, numSamples, false, false, true);
    processingBuffer.clear();


    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = processingBuffer.getWritePointer(channel);

        for (auto sampleIndex = 0; sampleIndex < processingBuffer.getNumSamples(); sampleIndex++)
        {
            *(channelData + sampleIndex) = 1;
            //*(channelData + sampleIndex) = exciter.getNextSample();
        }
    }
    
    exciter.applyEnvelopeToBuffer(processingBuffer, 0, numSamples);

    //firstResonator.setType(bandpassType);
    //firstResonator.setCutoffFrequency(fundimentalFreq);
    //firstResonator.setResonance(res);

    //juce::dsp::AudioBlock<float> block(processingBuffer);
    //juce::dsp::ProcessContextReplacing<float> context(block);
    //firstResonator.process(context);

    //set cutoff

    juce::dsp::AudioBlock<float> block(processingBuffer);

    updateFundamentalResonator();
    fundamentalResonator.process(juce::dsp::ProcessContextReplacing<float>(block));
    gain.process(juce::dsp::ProcessContextReplacing<float>(block));

    //use this as a way to add buffers from res filters together?
    for (int channel = 0; channel < numChannels; ++channel)
    {
        outputBuffer.addFrom(channel, startSample, processingBuffer, channel, 0, numSamples);
    }

    //if (!exciter.isActive())
    //{
        //clearCurrentNote();
    //}
}

void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate; 

    exciter.setSampleRate(spec.sampleRate);
    exciter.setParameters(juce::ADSR::Parameters(0.001f, 0.01f, 0, 0));
    //exciter.setParameters(juce::ADSR::Parameters(0.001f, 1.0f, 0, 0));

    firstResonator.prepare(spec);
    fundamentalResonator.prepare(spec);

    reset();
}

void SynthVoice::reset()
{
    exciter.reset();

    firstResonator.reset();
    fundamentalResonator.reset();
}

void SynthVoice::updateFundamentalResonator()
{
    *fundamentalResonator.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, fundimentalFreq, fundimentalRes);
}


void SynthVoice::setFilter(double newFundimentalFreq)
{
    //renname func: setFundamentalFreq
    if (newFundimentalFreq < 20.0f)
        newFundimentalFreq = 20.0f;

    fundimentalFreq = newFundimentalFreq; 
}
//make a function that is passed a filter and sets the freq and q?