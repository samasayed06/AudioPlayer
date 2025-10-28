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
    auto* reader = formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

double PlayerAudio::getLengthInSeconds() const
{
    return transportSource.getLengthInSeconds();
}

double PlayerAudio::getCurrentPosition() const
{
    return transportSource.getCurrentPosition();
}

void PlayerAudio::setPosition(double positionInSeconds)
{
    transportSource.setPosition(positionInSeconds);
}

// ====== التحكم ======
void PlayerAudio::play() { transportSource.start(); }
void PlayerAudio::pause() { transportSource.stop(); }
void PlayerAudio::stop() { transportSource.stop(); transportSource.setPosition(0.0); }
void PlayerAudio::restart() { transportSource.setPosition(0.0); transportSource.start(); }
void PlayerAudio::start() { transportSource.setPosition(0.0); }
void PlayerAudio::end() { transportSource.setPosition(transportSource.getLengthInSeconds()); }

// ====== A-B Looping ======
void PlayerAudio::setLoopA()
{
    loopStart = transportSource.getCurrentPosition();
    DBG("Loop Start (A) set at: " << loopStart);
}

void PlayerAudio::setLoopB()
{
    loopEnd = transportSource.getCurrentPosition();
    if (loopEnd < loopStart)
        std::swap(loopStart, loopEnd);

    DBG("Loop End (B) set at: " << loopEnd);
    DBG("Loop Range: " << loopStart << " -> " << loopEnd);
}

void PlayerAudio::toggleABLoop()
{
    isLoopingAB = !isLoopingAB;
    DBG("A-B Looping " << (isLoopingAB ? "Enabled" : "Disabled"));
}

void PlayerAudio::checkAndLoop()
{
    if (isLoopingAB && transportSource.isPlaying())
    {
        double pos = transportSource.getCurrentPosition();
        if (pos >= loopEnd)
        {
            transportSource.setPosition(loopStart);
        }
    }
}
