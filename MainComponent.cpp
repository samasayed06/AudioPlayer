#pragma once
#include "MainComponent.h"
#include "PlayerGUI.h"
#include "PlayerAudio.h"

MainComponent::MainComponent()
{
    // create backends
    audio1.reset(new PlayerAudio());
    audio2.reset(new PlayerAudio());

    // create GUIs and bind to backends
    gui1.reset(new PlayerGUI());
    gui2.reset(new PlayerGUI());
    gui1->setAudioBackend(audio1.get());
    gui2->setAudioBackend(audio2.get());

    addAndMakeVisible(gui1.get());
    addAndMakeVisible(gui2.get());

    setSize(960, 520);

    setAudioChannels(0, 2); // stereo output
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (audio1) audio1->prepareToPlay(samplesPerBlockExpected, sampleRate);
    if (audio2) audio2->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    // temporary buffer for mixing
    juce::AudioBuffer<float> temp(bufferToFill.buffer->getNumChannels(),
        bufferToFill.numSamples);
    juce::AudioSourceChannelInfo tempInfo(&temp, 0, bufferToFill.numSamples);

    // Track 1
    if (audio1 && audio1->isLoadedAndPlaying())
    {
        temp.clear();
        audio1->getNextAudioBlock(tempInfo);
        for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch)
            bufferToFill.buffer->addFrom(ch, bufferToFill.startSample,
                temp, ch, 0,
                bufferToFill.numSamples);
    }

    // Track 2
    if (audio2 && audio2->isLoadedAndPlaying())
    {
        temp.clear();
        audio2->getNextAudioBlock(tempInfo);
        for (int ch = 0; ch < bufferToFill.buffer->getNumChannels(); ++ch)
            bufferToFill.buffer->addFrom(ch, bufferToFill.startSample,
                temp, ch, 0,
                bufferToFill.numSamples);
    }

    // reduce overall gain to avoid clipping
    bufferToFill.buffer->applyGain(0.8f);
}
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkslategrey);
}

void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(10);
    auto top = r.removeFromTop(r.getHeight() / 2 - 5);
    gui1->setBounds(top);
    gui2->setBounds(r);
}
void MainComponent::releaseResources()
{
    if (audio1)
        audio1->releaseResources();
    if (audio2)
        audio2->releaseResources();
}

