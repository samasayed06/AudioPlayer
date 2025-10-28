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

    // التحكم في الملفات والصوت
    void loadFile(const juce::File& file);
    void play();
    void pause();
    void restart();
    void stop();
    void start();
    void end();

    // إعداد وتشغيل الصوت
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    // معلومات الوقت
    double getLengthInSeconds() const;
    double getCurrentPosition() const;
    void setPosition(double positionInSeconds);

    // --- A-B Looping ---
    void setLoopA();
    void setLoopB();
    void toggleABLoop();
    void checkAndLoop();

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    // A-B Loop Variables
    double loopStart = 0.0;
    double loopEnd = 0.0;
    bool isLoopingAB = false;
};
