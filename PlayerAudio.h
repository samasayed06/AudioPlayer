/*
  ==============================================================================

    PlayerAudio.h
    Created: 21 Oct 2025 4:33:49pm
    Author:  USER

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio() override;

    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // controls
    bool loadFile(const juce::File& file); // returns true if loaded
    void play();
    void pause();
    void stop();
    void setPosition(double seconds);
    double getPosition() const;
    double getLength() const;
    void setGain(float g);
    float getGain() const;

    // mute
    void toggleMute();
    bool isMuted() const;

    // loop
    void setLooping(bool on);
    bool isLoopingEnabled() const;

    // A-B loop
    void setMarkerA(double sec);
    void setMarkerB(double sec);
    void setABLoopEnabled(bool on);
    bool isABLoopEnabled() const;
    double markerA() const;
    double markerB() const;

    // status
    bool isLoadedAndPlaying() const;

    // metadata
    juce::String getDisplayInfo() const;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    juce::File currentFile;

    bool muted = false;
    float previousGain = 0.8f;
    bool looping = false;

    bool abLoop = false;
    double aMarker = -1.0, bMarker = -1.0;

    juce::String metaTitle, metaArtist;
    double metaDurationSeconds = 0.0;

    void readMetadata(const juce::File& f);
};
