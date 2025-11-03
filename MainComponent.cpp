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

    setAudioChannels(0, 2); // we will call prepareToPlay below
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
    // Simple design: only audio1 plays if it's playing, otherwise audio2.
    // (You can change mixing logic if you want both mixed.)
    bufferToFill.clearActiveBufferRegion();

    if (audio1 && audio1->isLoadedAndPlaying())
    {
        audio1->getNextAudioBlock(bufferToFill);
    }
    else if (audio2 && audio2->isLoadedAndPlaying())
    {
        audio2->getNextAudioBlock(bufferToFill);
    }
}

void MainComponent::releaseResources()
{
    if (audio1) audio1->releaseResources();
    if (audio2) audio2->releaseResources();
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
