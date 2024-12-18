#include "SynthVoice.h"
#include "SynthSound.h"

//what the hell is this
//Initalise Resonant Filters
SynthVoice::SynthVoice()//:fundamentalResonator(juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, fundimentalFreq, fundimentalRes))
{   
    shape = 0;
    spread = 1;

    fundimentalFreq = 130;
    fundimentalRes = 200;

    //move this if I want to make numResonators a param
    filterBank.resize(numResonators);
    bufferBank.resize(numResonators);
    gainBank.resize(numResonators);
    for (auto i = 0; i < numResonators; i++)
        gainBank[i].setGainLinear(1.0f);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    //Trigger Exciter
    exciter.noteOn();

    //make this except midi number? - updateFundamentalResonator(midiNoteNumber)
    updateFundamentalResonator();

    //Set the gain to resonance, rudimentary scaling
    //gain.setGainLinear(fundimentalRes*0.5f);
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

    for (auto i = 0; i < numResonators; i++)
    {
        //Init bufferBank
        bufferBank[i].setSize(numChannels, numSamples, false, false, true);
        bufferBank[i].clear();
    }

    //Get first buffer in bank and fill with exciter
    juce::AudioBuffer<float> baseBuffer = bufferBank[0];

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = baseBuffer.getWritePointer(channel);

        for (auto sampleIndex = 0; sampleIndex < baseBuffer.getNumSamples(); sampleIndex++)
        {
            *(channelData + sampleIndex) = 1;
            //*(channelData + sampleIndex) = exciter.getNextSample();
        }
    }
    
    //Apply Exciter Env
    exciter.applyEnvelopeToBuffer(baseBuffer, 0, numSamples);


    //Copy Exciter in baseBuffer to other buffers - could do thos above
    for (auto i = 0; i < numResonators; i++)
    {
        //bufferBank[i].makeCopyOf(baseBuffer, 0, numSamples);
        bufferBank[i].makeCopyOf(baseBuffer, numSamples);
    }


    //Process buffers through filterBank
    for (auto i = 0; i < numResonators; i++)
    {
        juce::dsp::AudioBlock<float> block(bufferBank[i]);
        filterBank[i].process(juce::dsp::ProcessContextReplacing<float>(block));
        gainBank[i].process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    //Mix bufferBank to outputBuffer
    for (int channel = 0; channel < numChannels; ++channel)
    {   
        for (auto i = 0; i < numResonators; i++)
        {
            outputBuffer.addFrom(channel, startSample, bufferBank[i], channel, 0, numSamples);
        }
    }

    //if (!exciter.isActive())
    //{
        //clearCurrentNote();
    //}
}

void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate; 

    exciter.setSampleRate(sampleRate);
    exciter.setParameters(juce::ADSR::Parameters(0.001f, 0.01f, 0, 0));
    //exciter.setParameters(juce::ADSR::Parameters(0.001f, 1.0f, 0, 0));

    for (auto i = 0; i < numResonators; i++)
    {
        filterBank[i].prepare(spec);
    }

    reset();
}

void SynthVoice::reset()
{
    exciter.reset();

    for (auto i = 0; i < numResonators; i++)
    {
        filterBank[i].reset();
    }
}

void SynthVoice::updateFundamentalResonator() //change name to updateResonators
{
    //ArrayCoefficients does not allocate memory and Coefficients does...

    //CRASH IF SHAPE IS NOT MOVED, NOT INITED PROPERLY?

    auto prevHarmonicRatio = 1;
    for (auto i = 0; i < numResonators; i++)
    {

        float freq;
        float q;
        int gainOn = 1;

        if (i < 1)
        {
            //Tread fundimental resinator differently
            freq = fundimentalFreq;
            if (freq > sampleRate * 0.5 || freq < 20.0f)
            {
                //if freq is higher than niqust set gain to 0
                freq = 440;     //set to arbitrary value, resinator is muted anyway
                gainOn = 0;
            }

            q = fundimentalRes;
            *filterBank[0].state = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, freq, q);
        }
        else
        {

            //no lerp in my version of c++?
            //auto harmonicRatio = std::lerp((int(prevHarmonicRatio) * spread), (prevHarmonicRatio * circularModes[i]), shape);

            float squareHarmonicRatio = int(prevHarmonicRatio * spread);    //truncate to only get harmoic ratios
            float circularHarmonicRatio = (prevHarmonicRatio * spread) * circularModes[i];

            //shape is a negative number when dial has not been used HOW!
            //quick fix
            if (shape < 0)
                shape = 0;

            auto harmonicRatio = basicLerp(circularHarmonicRatio, squareHarmonicRatio, shape);

            freq = harmonicRatio * fundimentalFreq;
            if (freq > sampleRate * 0.5 || freq < 20.0f)
            {
                //maybe if they exceed niquit they get refected
                //aliasing on perpose?!??!
                freq = 440;  
                gainOn = 0;
            }

            q = fundimentalRes;
            *filterBank[i].state = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, freq, q);

            prevHarmonicRatio = harmonicRatio;
        }

        //Set gain
        gainBank[i].setGainLinear(q * 0.5 * gainOn);
    }
}

//==============================================================================
//Setters
void SynthVoice::setFundamentalFreq(double newFundimentalFreq)
{
    if (newFundimentalFreq < 20.0f)
        newFundimentalFreq = 20.0f;

    fundimentalFreq = newFundimentalFreq; 
}

void SynthVoice::setFundamentalRes(double newFundimentalRes)
{
    fundimentalRes = newFundimentalRes; 
}

void SynthVoice::setSpread(double newSpread)
{
    spread = newSpread;
}

void SynthVoice::setShape(double newShape)
{
    shape = newShape;
}

float SynthVoice::basicLerp(float a, float b, float t)
{
    //basic linear interpreter
    auto output = (a * (1.0f - t)) + (b * t);
    return output;
}