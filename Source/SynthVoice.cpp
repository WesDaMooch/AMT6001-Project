#include "SynthVoice.h"
#include "SynthSound.h"

SynthVoice::SynthVoice()
{

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
    if (!exciter.isActive())
    {
        clearCurrentNote();
    }
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

    firstResonator.setType(bandpassType);
    firstResonator.setCutoffFrequency(cutoff);
    firstResonator.setResonance(res);

    juce::dsp::AudioBlock<float> block(processingBuffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    firstResonator.process(context);


    //use this as a way to add buffers from res filters together?
    for (int channel = 0; channel < numChannels; ++channel)
    {
        outputBuffer.addFrom(channel, startSample, processingBuffer, channel, 0, numSamples);
    }

    //if (!envelope.isActive())
    //{
    //    clearCurrentNote();
    //}
}

void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
              

    exciter.setSampleRate(spec.sampleRate);
    //exciter.setParameters(juce::ADSR::Parameters(0.001f, 0.01f, 0, 0));
    exciter.setParameters(juce::ADSR::Parameters(0.01f, 0.01f, 0, 0));

    firstResonator.prepare(spec);

    reset();
}

void SynthVoice::reset()
{
    exciter.reset();

    firstResonator.reset();
}
