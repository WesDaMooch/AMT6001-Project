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
    filterBank.resize(maxResonators);
    bufferBank.resize(maxResonators);

    // Should vectors be dynamicly resized
    resonatorMakeUpGainBank.resize(maxResonators);
    for (auto i = 0; i < maxResonators; i++)
        resonatorMakeUpGainBank[i].setGainLinear(1.0f);

    harmoAttenuatorBank.resize(maxResonators);
    for (auto i = 0; i < maxResonators; i++)
        harmoAttenuatorBank[i].setGainLinear(1.0f);

    filterBankAttenuator.setGainLinear(1.0f / numResonators);   // Division...
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

    for (auto i = 0; i < maxResonators; i++)
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

        resonatorMakeUpGainBank[i].process(juce::dsp::ProcessContextReplacing<float>(block));
        harmoAttenuatorBank[i].process(juce::dsp::ProcessContextReplacing<float>(block));
        filterBankAttenuator.process(juce::dsp::ProcessContextReplacing<float>(block));
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

    //prepare gains 
    filterBankAttenuator.prepare(spec);

    for (auto i = 0; i < maxResonators; i++)
    {
        resonatorMakeUpGainBank[i].prepare(spec);
        harmoAttenuatorBank[i].prepare(spec);
        filterBank[i].prepare(spec);
    }

    reset();
}

void SynthVoice::reset()
{
    exciter.reset();
    exciterShape.reset();

    //reset gains
    filterBankAttenuator.reset();


    for (auto i = 0; i < maxResonators; i++)
    {
        resonatorMakeUpGainBank[i].reset();
        harmoAttenuatorBank[i].reset();
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
        float harmoAttenuator = 1.0f;

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
            //print the freqs out, see if they are correct, I think they are not!
            
            float squareHarmonicRatio = (prevHarmonicRatio * (spread + 1)); //+1?, truncate to only get harmoic ratios
            float circularHarmonicRatio = (prevHarmonicRatio * spread) * circularModes[i-1];

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

            harmoAttenuator = harmo - i;
            if (harmoAttenuator < 0.0f)
                harmoAttenuator = 0.0f;
            else if (harmoAttenuator > 1.0f)
                harmoAttenuator = 1.0f;
        }

        // Set make up gain
        resonatorMakeUpGainBank[i].setGainLinear(q * 0.5);

        harmoAttenuator *= gainOn;
        // Set attenuation from harmo param, only effects overtone harmonics
        harmoAttenuatorBank[i].setGainLinear(harmoAttenuator);
    }
    
    // Update filter bank attenuation based on active filters
    filterBankAttenuator.setGainLinear(1.0f / numResonators);

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
void SynthVoice::setResonatorAmount(double newHarmo)    // rename setHarmo 
{
    harmo = newHarmo;
    // numResonators one bigger than harmo
    int tempNumResonators = int(newHarmo + 1);
    if (tempNumResonators > maxResonators)
        tempNumResonators = maxResonators;
    numResonators = tempNumResonators;
}
void SynthVoice::setSpread(double newSpread) { spread = newSpread; }
void SynthVoice::setShape(double newShape) { shape = newShape; }
void SynthVoice::setExciterAttack(double newExciterAttack) { exciterAttack = newExciterAttack; }
void SynthVoice::setExciterRelease(double newExciterRelease) { exciterRelease = newExciterRelease; }
void SynthVoice::setExciterNoiseAmount(double newExciterNoiseAmount) { exciterNoiseAmount = newExciterNoiseAmount; }

//RENAME TO MAKE MORE SENSE
float SynthVoice::basicLerp(float a, float b, float t)
{
    //basic linear interpreter
    auto output = (a * (1.0f - t)) + (b * t);
    return output;
}