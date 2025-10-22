

#pragma once
#include <JuceHeader.h>

class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void loadFile(const juce::File& file);
    void play();
    void pause();
    void restart();
    void stop();
    void start();
    void end();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

   
    void mute();
    void unmute();
    bool isMuted() const { return muted; }

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    bool muted = false;
    float previousGain = 0.5f;
};




