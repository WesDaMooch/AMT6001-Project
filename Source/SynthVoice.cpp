#include "SynthVoice.h"
#include "SynthSound.h"

SynthVoice::SynthVoice()
{   
    fundimentalFreq = 130;

    shape = 0.0f;
    spread = 1.0f;
    decay = 200.0f;

    freqBank.resize(maxResonators);
    for (auto i = 0; i < maxResonators; i++)
        freqBank[i] = 440.0f;

    
    qBank.resize(maxResonators);
    for (auto i = 0; i < maxResonators; i++)
        qBank[i] = 200.0f;

    filterBank.resize(maxResonators);
    bufferBank.resize(maxResonators);

    // Should vectors be dynamicly resized...
    resonatorMakeUpGainBank.resize(maxResonators);
    for (auto i = 0; i < maxResonators; i++)
        resonatorMakeUpGainBank[i].setGainLinear(1.0f);

    harmoAttenuatorBank.resize(maxResonators);
    for (auto i = 0; i < maxResonators; i++)
        harmoAttenuatorBank[i].setGainLinear(1.0f);

    outputGain.setGainLinear(outputGainValue);

    // tanh soft clipping lamda function
    softClipper.functionToUse = [] (float x) { return std::tanh(x); };
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    // Update most params at the start of note, no need to have params change after they have been triggered
    updateParameters();

    // Trigger exciter
    exciter.noteOn();
    // Trigger punch envelope
    punchModulationEnvelope.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    exciter.noteOff();
    //punchModulationEnvelope.noteOff();

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

    for (auto i = 0; i < maxResonators; i++)
    {
        // Init bufferBank
        bufferBank[i].setSize(numChannels, numSamples, false, false, true);
        bufferBank[i].clear();
    }

    // Get first buffer in bank and fill with exciter
    juce::AudioBuffer<float> baseBuffer = bufferBank[0];

    punchModulationEnvelope.applyEnvelopeToBuffer(baseBuffer, 0, numSamples);  // Quirk of ADSR, have to apply env for process sample to work
    baseBuffer.clear();

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
    
    // Apply Exciter Envelope
    exciter.applyEnvelopeToBuffer(baseBuffer, 0, numSamples);
    
    // Process buffers through filterBank
    for (auto i = 0; i < numResonators; i++)
    {
        // Copy Exciter in baseBuffer to other buffers
        bufferBank[i].makeCopyOf(baseBuffer, numSamples);

        float freq = freqBank[i];
        float freqMod = punchModulationEnvelope.getNextSample();

        // Get punch filter modulation envelope
        freq = freq + (freqMod * punchDepth);

        // Limit freq at 20Hz and Niquist
        freq = std::fmax(freq, 20.0f);
        freq = std::fmin(freq, sampleRate * 0.5);

        // Update filter parameters
        *filterBank[i].state = juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sampleRate,
            freq, qBank[i]);

        // Apply filter to buffer
        juce::dsp::AudioBlock<float> block(bufferBank[i]);
        filterBank[i].process(juce::dsp::ProcessContextReplacing<float>(block));

        // Apply gain and attenuation
        resonatorMakeUpGainBank[i].process(juce::dsp::ProcessContextReplacing<float>(block));
        harmoAttenuatorBank[i].process(juce::dsp::ProcessContextReplacing<float>(block));

        // Apply output gain
        outputGain.setGainLinear(outputGainValue);
        outputGain.process(juce::dsp::ProcessContextReplacing<float>(block));
    }

    // Mix bufferBank to outputBuffer
    for (int channel = 0; channel < numChannels; ++channel)
    {   
        for (auto i = 0; i < numResonators; i++)
        {
            outputBuffer.addFrom(channel, startSample, bufferBank[i], channel, 0, numSamples);
        }
    }

    // Apply soft clipping
    juce::dsp::AudioBlock<float> block(outputBuffer);
    softClipper.process(juce::dsp::ProcessContextReplacing<float>(block));

    if (!exciter.isActive())
    {
        clearCurrentNote();
    }
}

void SynthVoice::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate; 

    exciter.setSampleRate(sampleRate);
    exciter.setParameters(juce::ADSR::Parameters(exciterAttack, exciterRelease, 0, 0));

    punchModulationEnvelope.setSampleRate(sampleRate);
    punchModulationEnvelope.setParameters(juce::ADSR::Parameters(0, punchRelease, 0, 0));

    for (auto i = 0; i < maxResonators; i++)
    {
        resonatorMakeUpGainBank[i].prepare(spec);
        harmoAttenuatorBank[i].prepare(spec);
        filterBank[i].prepare(spec);
    }

    softClipper.prepare(spec);

    reset();
}

void SynthVoice::reset()
{
    exciter.reset();
    punchModulationEnvelope.reset();

    for (auto i = 0; i < maxResonators; i++)
    {
        resonatorMakeUpGainBank[i].reset();
        harmoAttenuatorBank[i].reset();
        filterBank[i].reset();
    }

    softClipper.reset();
}

void SynthVoice::updateParameters() //change name to updateResonators
{
    // Update filter coefs
    auto prevHarmonicRatio = 1;
    for (auto i = 0; i < numResonators; i++)
    {

        float freq;
        float q;

        int gainOn = 1;
        float harmoAttenuator = 1.0f;

        if (i < 1)
        {
            // Treat fundimental resinator differently
            freq = fundimentalFreq;
            if (freq > sampleRate * 0.5f || freq < 20.0f)
            {
                // If freq is higher than niqust set gain to 0
                freq = 440;     // Set to arbitrary value, resinator is muted anyway
                gainOn = 0;
            }

            q = decay;

            qBank[0] = q;
            freqBank[0] = freq;
        }
        else
        {   
            float squareHarmonicRatio = (prevHarmonicRatio * (spread + 1)); //+1?, truncate to only get harmoic ratios
            float circularHarmonicRatio = (prevHarmonicRatio * spread) * circularModes[i-1];

            auto harmonicRatio = linearInterpolator(circularHarmonicRatio, squareHarmonicRatio, shape);

            freq = harmonicRatio * fundimentalFreq;

            if (freq > sampleRate * 0.5f || freq < 20.0f)
            {
                freq = 440;  
                gainOn = 0;
            }

            q = decay;

            qBank[i] = q;
            freqBank[i] = freq;

            prevHarmonicRatio = harmonicRatio;

            harmoAttenuator = harmo - i;
            if (harmoAttenuator < 0.0f)
                harmoAttenuator = 0.0f;
            else if (harmoAttenuator > 1.0f)
                harmoAttenuator = 1.0f;
        }

        // Set make up gain
        resonatorMakeUpGainBank[i].setGainLinear(q * 0.5f);

        harmoAttenuator *= gainOn;
        // Set attenuation from harmo param, only effects overtone harmonics
        harmoAttenuatorBank[i].setGainLinear(harmoAttenuator);
    }
    
    // Update exciter
    exciter.setParameters(juce::ADSR::Parameters(exciterAttack * 0.001f, exciterRelease * 0.001f, 0.0f, 0.0f));
    punchModulationEnvelope.setParameters(juce::ADSR::Parameters(0.0f, (punchRelease * 5.0f) * 0.001f, 0.0f, 0.0f));
}

//==============================================================================
// Setters
void SynthVoice::setFundamentalFreq(float newFundimentalFreq)
{
    //if (newFundimentalFreq < 20.0f)
        //newFundimentalFreq = 20.0f;

    newFundimentalFreq = std::fmax(newFundimentalFreq, 20.0f);
    newFundimentalFreq = std::fmin(newFundimentalFreq, sampleRate * 0.5f);

    fundimentalFreq = newFundimentalFreq; 
}
void SynthVoice::setDecay(float newDecay) { decay = newDecay; }
void SynthVoice::setHarmo(float newHarmo)
{
    harmo = newHarmo;
    // numResonators one bigger than harmo
    int tempNumResonators = int(newHarmo + 1);
    if (tempNumResonators > maxResonators)
        tempNumResonators = maxResonators;
    numResonators = tempNumResonators;
}
void SynthVoice::setSpread(float newSpread) { spread = newSpread; }
void SynthVoice::setShape(float newShape) { shape = newShape; }
void SynthVoice::setExciterAttack(float newExciterAttack) { exciterAttack = newExciterAttack; }
void SynthVoice::setExciterRelease(float newExciterRelease) { exciterRelease = newExciterRelease; }
void SynthVoice::setExciterNoiseAmount(float newExciterNoiseAmount) { exciterNoiseAmount = newExciterNoiseAmount; }
void SynthVoice::setPunchRelease(float newPunchRelease) { punchRelease = newPunchRelease; }
void SynthVoice::setOutputGainValue(float newOutputGainValue) { outputGainValue = newOutputGainValue; }

//==============================================================================
// Helpers
float SynthVoice::linearInterpolator(float inputOne, float inputTwo, float mix)
{
    auto output = (inputOne * (1.0f - mix)) + (inputTwo * mix);
    return output;
}