#include "PluginProcessor.h"
#include "PluginEditor.h"

struct SynthSound : public juce::SynthesiserSound {
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

struct SynthVoice : public juce::SynthesiserVoice {
    bool canPlaySound(juce::SynthesiserSound* sound) override {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override {
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        phase = 0.0;
        level = velocity;
    }

    void stopNote(float, bool) override { clearCurrentNote(); }
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override {
        if (!isVoiceActive()) return;

        double cyclesPerSample = frequency / getSampleRate();
        while (--numSamples >= 0) {
            auto currentSample = (float)((2.0 * phase) - 1.0) * level * 0.25f;

            for (int i = 0; i < outputBuffer.getNumChannels(); ++i)
                outputBuffer.addSample(i, startSample, currentSample);

            phase += cyclesPerSample;
            if (phase >= 1.0) phase -= 1.0;
            startSample++;
        }
    }

private:
    double frequency = 440.0;
    double phase = 0.0;
    float level = 0.0f;
};

CyberWaveAudioProcessor::CyberWaveAudioProcessor()
{
    for (int i = 0; i < 16; ++i) synth.addVoice(new SynthVoice());
    synth.addSound(new SynthSound());
}

CyberWaveAudioProcessor::~CyberWaveAudioProcessor() {}

void CyberWaveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    delayBuffer.setSize(2, (int)(sampleRate * 2.0));
    delayBuffer.clear();
}

void CyberWaveAudioProcessor::releaseResources() {}

void CyberWaveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    int numSamples = buffer.getNumSamples();
    int delayBufferLength = delayBuffer.getNumSamples();
    int delayTimeInSamples = (int)(getSampleRate() * 0.375);

    for (int channel = 0; channel < getTotalNumOutputChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        auto* delayData = delayBuffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            int readPosition = (delayWritePosition - delayTimeInSamples + delayBufferLength) % delayBufferLength;
            
            float inSample = channelData[sample];
            float delayedSample = delayData[readPosition];

            channelData[sample] = inSample + (delayedSample * wetDelayAmount);
            delayData[(delayWritePosition + sample) % delayBufferLength] = inSample + (delayedSample * 0.5f);
        }
    }
    delayWritePosition = (delayWritePosition + numSamples) % delayBufferLength;
}

void CyberWaveAudioProcessor::loadPresetById(int presetId)
{
    switch (presetId) {
        case 2:  wetDelayAmount = 0.05f; break;
        case 4:  wetDelayAmount = 0.45f; break;
        case 6:  wetDelayAmount = 0.65f; break;
        case 8:  wetDelayAmount = 0.50f; break;
        case 10: wetDelayAmount = 0.30f; break;
        case 11: wetDelayAmount = 0.25f; break;
        default: break;
    }
}

juce::AudioProcessorEditor* CyberWaveAudioProcessor::createEditor()
{
    return new CyberWaveAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CyberWaveAudioProcessor();
}
