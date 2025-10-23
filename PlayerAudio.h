/*
  ==============================================================================

    PlayerAudio.h
    Created: 21 Oct 2025 4:33:49pm
    Author:  USER

  ==============================================================================
*/

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

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
};



