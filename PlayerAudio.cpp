/*
  ==============================================================================

    PlayerAudio.cpp
    Created: 21 Oct 2025 4:33:12pm
    Author:  USER

  ==============================================================================
*/

#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

void PlayerAudio::loadFile(const juce::File& file)
{
    DBG("Loading file: " + file.getFullPathName());

    auto* reader = formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(
            new juce::AudioFormatReaderSource(reader, true));

        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());

        
        transportSource.setGain(0.9f);
        transportSource.prepareToPlay(512, reader->sampleRate);

        DBG("Loaded file successfully. Sample rate: " << reader->sampleRate);
    }
    else
    {
        DBG("Failed to load file (unsupported format or missing codec).");
    }
}


void PlayerAudio::play() { transportSource.start(); }
void PlayerAudio::pause() { transportSource.stop(); }
void PlayerAudio::stop() { transportSource.stop(); transportSource.setPosition(0.0); }
void PlayerAudio::restart() { transportSource.setPosition(0.0); transportSource.start(); }
void PlayerAudio::start() { transportSource.setPosition(0.0); }
void PlayerAudio::end() { transportSource.setPosition(transportSource.getLengthInSeconds()); }
void PlayerAudio::setLooping(bool shouldLoop)
{
    transportSource.setLooping(shouldLoop);
}
