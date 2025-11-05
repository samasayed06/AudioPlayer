/*
  ==============================================================================

    PlayerAudio.cpp
    Created: 21 Oct 2025 4:33:12pm
    Author:  USER

  ==============================================================================
*/
#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
    previousGain = 0.8f;
    transportSource.setGain(previousGain);
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

}

void PlayerAudio::releaseResources()
{
    resampleSource.releaseResources();

}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    resampleSource.getNextAudioBlock(bufferToFill);


    // A-B loop (audio thread)
    if (abLoop && aMarker >= 0.0 && bMarker > aMarker)
    {
        double pos = transportSource.getCurrentPosition();
        if (pos >= bMarker)
            transportSource.setPosition(aMarker);
    }
    else if (looping)
    {
        double pos = transportSource.getCurrentPosition();
        double len = transportSource.getLengthInSeconds();
        if (len > 0.0 && pos >= len - 0.02)
        {
            transportSource.setPosition(0.0);
            transportSource.start();
        }
    }
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (!file.existsAsFile())
        return false;

    auto* reader = formatManager.createReaderFor(file);
    if (reader == nullptr)
        return false;

    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
    transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

    currentFile = file;
    readMetadata(file);

    aMarker = -1.0;
    bMarker = -1.0;
    abLoop = false;

    return true;
}

void PlayerAudio::play()    { if (readerSource) transportSource.start(); }
void PlayerAudio::pause()   { transportSource.stop(); }
void PlayerAudio::stop()    { transportSource.stop(); transportSource.setPosition(0.0); }
void PlayerAudio::setPosition(double seconds) { transportSource.setPosition(seconds); }
double PlayerAudio::getPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLength() const { return transportSource.getLengthInSeconds(); }

void PlayerAudio::setGain(float g)
{
    previousGain = juce::jlimit(0.0f, 1.0f, g);
    if (!muted) transportSource.setGain(previousGain);
}
float PlayerAudio::getGain() const { return previousGain; }
void PlayerAudio::setSpeed(double ratio)
{
    // Clamp between 0.5x and 2.0x
    ratio = juce::jlimit(0.5, 2.0, ratio);
    resampleSource.setResamplingRatio(ratio);
}

double PlayerAudio::getSpeed() const
{
    return resampleSource.getResamplingRatio();
}

void PlayerAudio::toggleMute()
{
    muted = !muted;
    if (muted) transportSource.setGain(0.0f);
    else transportSource.setGain(previousGain);
}
bool PlayerAudio::isMuted() const { return muted; }

void PlayerAudio::setLooping(bool on) { looping = on; }
bool PlayerAudio::isLoopingEnabled() const { return looping; }

void PlayerAudio::setMarkerA(double sec) { aMarker = juce::jmax(0.0, sec); }
void PlayerAudio::setMarkerB(double sec) { bMarker = juce::jmax(0.0, sec); if (bMarker < aMarker) std::swap(aMarker, bMarker); }
void PlayerAudio::setABLoopEnabled(bool on) { abLoop = on; }
bool PlayerAudio::isABLoopEnabled() const { return abLoop; }
double PlayerAudio::markerA() const { return aMarker; }
double PlayerAudio::markerB() const { return bMarker; }

bool PlayerAudio::isLoadedAndPlaying() const
{
    return readerSource && transportSource.isPlaying();
}

void PlayerAudio::readMetadata(const juce::File& f)
{
    metaTitle.clear(); metaArtist.clear(); metaDurationSeconds = 0.0;

    std::string path = f.getFullPathName().toStdString();
    TagLib::FileRef ref(path.c_str());
    if (!ref.isNull() && ref.tag())
    {
        auto t = ref.tag()->title();
        auto a = ref.tag()->artist();
        if (!t.isEmpty()) metaTitle = juce::String(t.toCString(true));
        if (!a.isEmpty()) metaArtist = juce::String(a.toCString(true));
    }
    if (!ref.isNull() && ref.audioProperties())
    {
#if defined(TAGLIB_MAJOR) && TAGLIB_MAJOR >= 1
        metaDurationSeconds = ref.audioProperties()->lengthInSeconds();
#else
        metaDurationSeconds = ref.audioProperties()->length();
#endif
    }
}

juce::String PlayerAudio::getDisplayInfo() const
{
    juce::String dur = "00:00";
    if (metaDurationSeconds > 0.0)
    {
        int mins = int(metaDurationSeconds) / 60;
        int secs = int(metaDurationSeconds) % 60;
        dur = juce::String::formatted("%02d:%02d", mins, secs);
    }
    if (metaTitle.isNotEmpty())
        return metaTitle + (metaArtist.isNotEmpty() ? " - " + metaArtist : juce::String()) + " (" + dur + ")";
    return currentFile.getFileName() + " (" + dur + ")";
}
