/*
  ==============================================================================

    PlayerGUI.cpp
    Created: 21 Oct 2025 4:33:31pm
    Author:  USER

  ==============================================================================
*/


#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& player) : audioPlayer(player)
{
    setupButtons();
}

void PlayerGUI::setupButtons()
{
    addAndMakeVisible(loadButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(pauseButton);
    addAndMakeVisible(restartButton);
    addAndMakeVisible(startButton);
    addAndMakeVisible(endButton);
    addAndMakeVisible(loopButton);
    loopButton.onClick = [this]()
        {
            audioPlayer.setLooping(loopButton.getToggleState());
        };

    loadButton.onClick = [this]()
        {
            auto chooser = std::make_unique<juce::FileChooser>(
                "Select an audio file...", juce::File(), "*.mp3;*.wav;*.aiff");

            chooser->launchAsync(
                juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& fc)
                {
                    auto file = fc.getResult();
                    if (file.existsAsFile())
                        audioPlayer.loadFile(file);
                });

            fileChooser = std::move(chooser);

            

            
        };

    playButton.onClick = [this]() { audioPlayer.play(); };
    pauseButton.onClick = [this]() { audioPlayer.pause(); };
    restartButton.onClick = [this]() { audioPlayer.restart(); };
    startButton.onClick = [this]() { audioPlayer.start(); };
    endButton.onClick = [this]() { audioPlayer.end(); };
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto buttonWidth = area.getWidth() / 7;

    loadButton.setBounds(area.removeFromLeft(buttonWidth).reduced(5));
    playButton.setBounds(area.removeFromLeft(buttonWidth).reduced(5));
    pauseButton.setBounds(area.removeFromLeft(buttonWidth).reduced(5));
    restartButton.setBounds(area.removeFromLeft(buttonWidth).reduced(5));
    startButton.setBounds(area.removeFromLeft(buttonWidth).reduced(5));
    endButton.setBounds(area.removeFromLeft(buttonWidth).reduced(5));
    loopButton.setBounds(area.removeFromLeft(buttonWidth).reduced(5));

}

