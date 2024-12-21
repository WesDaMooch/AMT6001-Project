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

    //dont like this rly, pass the exciter through a filter maybe to get more interesting shapes
    //exciterShape.setAttackTime(exciterAttack)
    //exciterShape.setReleaseTime(e);

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
            float noise = randomFloat.nextFloat(); // White noise 0 - 1
            float noiseAttenuation = ((exciterNoiseAmount * 0.5f) - 1.0f) * -1.0f;

            *(channelData + sampleIndex) = ((noise * exciterNoiseAmount) + 1) * noiseAttenuation;
            //*(channelData + sampleIndex) = 1;
        }
    }
    
    //Apply Exciter Env
    exciter.applyEnvelopeToBuffer(baseBuffer, 0, numSamples);
    

    //juce::dsp::AudioBlock<float> exciterBlock(baseBuffer); //maybe this kinda thing for punch env
   // exciterShape.process(juce::dsp::ProcessContextReplacing<float>(exciterBlock));


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

    //Very short click impulse
    exciter.setSampleRate(sampleRate);
    exciter.setParameters(juce::ADSR::Parameters(exciterAttack, exciterRelease, 0, 0));
    exciterShape.prepare(spec);

    for (auto i = 0; i < numResonators; i++)
    {
        filterBank[i].prepare(spec);
    }

    reset();
}

void SynthVoice::reset()
{
    exciter.reset();
    exciterShape.reset();

    for (auto i = 0; i < numResonators; i++)
    {
        filterBank[i].reset();
    }
}

void SynthVoice::updateFundamentalResonator() //change name to updateResonators
{
    //update filters
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
            //ArrayCoefficients does not allocate memory and Coefficients does...
            *filterBank[0].state = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate, freq, q);
        }
        else
        {

            //no lerp in my version of c++?
            //auto harmonicRatio = std::lerp((int(prevHarmonicRatio) * spread), (prevHarmonicRatio * circularModes[i]), shape);

            float squareHarmonicRatio = int(prevHarmonicRatio * (spread+1));    //truncate to only get harmoic ratios
            float circularHarmonicRatio = (prevHarmonicRatio * spread) * circularModes[i-1];

            //shape is a negative number when dial has not been used HOW!
            //quick fix
            //if (shape < 0)
                //shape = 0;

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

    // Update exciter
    exciter.setParameters(juce::ADSR::Parameters(exciterAttack*0.001, exciterRelease*0.001, 0, 0));
}

//==============================================================================
//Setters
void SynthVoice::setFundamentalFreq(double newFundimentalFreq)
{
    if (newFundimentalFreq < 20.0f)
        newFundimentalFreq = 20.0f;

    fundimentalFreq = newFundimentalFreq; 
}
void SynthVoice::setFundamentalRes(double newFundimentalRes) { fundimentalRes = newFundimentalRes; }
void SynthVoice::setSpread(double newSpread) { spread = newSpread; }
void SynthVoice::setShape(double newShape) { shape = newShape; }
void SynthVoice::setExciterAttack(double newExciterAttack) { exciterAttack = newExciterAttack; }
void SynthVoice::setExciterRelease(double newExciterRelease) { exciterRelease = newExciterRelease; }
void SynthVoice::setExciterNoiseAmount(double newExciterNoiseAmount) { exciterNoiseAmount = newExciterNoiseAmount; }

float SynthVoice::basicLerp(float a, float b, float t)
{
    //basic linear interpreter
    auto output = (a * (1.0f - t)) + (b * t);
    return output;
}