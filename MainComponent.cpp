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

    // Mix mode button (visible above players)
    addAndMakeVisible(mixModeButton);
    mixModeButton.onClick = [this]()
        {
            mixModeEnabled = !mixModeEnabled;
            mixModeButton.setButtonText(mixModeEnabled ? "Mix Mode ON" : "Mix Mode OFF");
            if (mixModeEnabled) enableMixMode();
            else disableMixMode();
        };

    addAndMakeVisible(gui1.get());
    addAndMakeVisible(gui2.get());

    setSize(960, 520);

    setAudioChannels(0, 2); // stereo output
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::enableMixMode()
{
    // Wire GUI callbacks so their controls affect both audio1 & audio2
    if (!gui1 || !gui2 || !audio1 || !audio2) return;

    // Play
    gui1->onPlayPressed = [this]() {
        if (audio1 && audio2) { audio1->play(); audio2->play(); }
        };
    gui2->onPlayPressed = gui1->onPlayPressed;

    // Pause
    gui1->onPausePressed = [this]() {
        if (audio1 && audio2) { audio1->pause(); audio2->pause(); }
        };
    gui2->onPausePressed = gui1->onPausePressed;

    // Restart (stop+play)
    gui1->onRestartPressed = [this]() {
        if (audio1 && audio2) { audio1->stop(); audio2->stop(); audio1->play(); audio2->play(); }
        };
    gui2->onRestartPressed = gui1->onRestartPressed;

    // Start (|<)
    gui1->onStartPressed = [this]() {
        if (audio1 && audio2) { audio1->setPosition(0.0); audio2->setPosition(0.0); }
        };
    gui2->onStartPressed = gui1->onStartPressed;

    // End (>|)
    gui1->onEndPressed = [this]() {
        if (audio1 && audio2) {
            double len1 = audio1->getLength();
            double len2 = audio2->getLength();
            // set both to their own ends (closest possible)
            if (len1 > 0.0) audio1->setPosition(len1 - 0.01);
            if (len2 > 0.0) audio2->setPosition(len2 - 0.01);
        }
        };
    gui2->onEndPressed = gui1->onEndPressed;

    // Volume - set both gains
    gui1->onVolumeChanged = [this](float v) {
        if (audio1 && audio2) { audio1->setGain(v); audio2->setGain(v); }
        };
    gui2->onVolumeChanged = gui1->onVolumeChanged;
    // Speed - set both playback rates
    gui1->onSpeedChanged = [this](double speed) {
        if (audio1 && audio2) { audio1->setSpeed(speed); audio2->setSpeed(speed); }
        };
    gui2->onSpeedChanged = gui1->onSpeedChanged;

    // Mute toggle
    gui1->onMuteToggled = [this]() {
        if (audio1 && audio2) { audio1->toggleMute(); audio2->toggleMute(); }
        };
    gui2->onMuteToggled = gui1->onMuteToggled;

    // Loop toggle (sync both to same state)
    gui1->onLoopToggled = [this]() {
        if (audio1 && audio2) {
            bool v = !audio1->isLoopingEnabled();
            audio1->setLooping(v);
            audio2->setLooping(v);
        }
        };
    gui2->onLoopToggled = gui1->onLoopToggled;

    // A-B loop toggle
    gui1->onABLoopToggled = [this]() {
        if (audio1 && audio2) {
            bool v = !audio1->isABLoopEnabled();
            audio1->setABLoopEnabled(v);
            audio2->setABLoopEnabled(v);
        }
        };
    gui2->onABLoopToggled = gui1->onABLoopToggled;

    // Position requests (when user sets position explicitly we set both)
    gui1->onSetPositionRequest = [this](double pos) {
        if (audio1 && audio2) { audio1->setPosition(pos); audio2->setPosition(pos); }
        };
    gui2->onSetPositionRequest = gui1->onSetPositionRequest;
}

void MainComponent::disableMixMode()
{
    // Clear callbacks so GUIs behave independently again
    if (!gui1 || !gui2) return;

    gui1->onPlayPressed = nullptr;
    gui2->onPlayPressed = nullptr;

    gui1->onPausePressed = nullptr;
    gui2->onPausePressed = nullptr;

    gui1->onRestartPressed = nullptr;
    gui2->onRestartPressed = nullptr;

    gui1->onStartPressed = nullptr;
    gui2->onStartPressed = nullptr;

    gui1->onEndPressed = nullptr;
    gui2->onEndPressed = nullptr;

    gui1->onVolumeChanged = nullptr;
    gui2->onVolumeChanged = nullptr;

    gui1->onSpeedChanged = nullptr;
    gui2->onSpeedChanged = nullptr;


    gui1->onMuteToggled = nullptr;
    gui2->onMuteToggled = nullptr;

    gui1->onLoopToggled = nullptr;
    gui2->onLoopToggled = nullptr;

    gui1->onABLoopToggled = nullptr;
    gui2->onABLoopToggled = nullptr;

    gui1->onSetPositionRequest = nullptr;
    gui2->onSetPositionRequest = nullptr;
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
    g.fillAll(juce::Colours::grey);
}

void MainComponent::resized()
{
    auto r = getLocalBounds().reduced(10);

    // place mix button at top
    auto topArea = r.removeFromTop(40);
    mixModeButton.setBounds(topArea.removeFromLeft(160).reduced(4));

    // remaining split for two GUIs
    auto guisArea = r;
    auto top = guisArea.removeFromTop(guisArea.getHeight() / 2 - 5);
    gui1->setBounds(top);
    gui2->setBounds(guisArea);
}

void MainComponent::releaseResources()
{
    if (audio1)
        audio1->releaseResources();
    if (audio2)
        audio2->releaseResources();
}
