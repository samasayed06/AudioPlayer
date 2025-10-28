/*
  ==============================================================================

    PlayerGUI.h
    Created: 21 Oct 2025 4:34:07pm
    Author:  USER

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI(PlayerAudio& player);
    ~PlayerGUI() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

private:
    PlayerAudio& audioPlayer;

    // Buttons
    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ ">" };
    juce::TextButton pauseButton{ "||" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton startButton{ "|<" };
    juce::TextButton endButton{ ">|" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop OFF" };

    // A-B Loop Buttons
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton loopABButton{ "A-B Loop OFF" };

    // Labels for A-B info
    juce::Label loopALabel;
    juce::Label loopBLabel;

    juce::Slider positionSlider;
    std::unique_ptr<juce::FileChooser> fileChooser;

    void setupButtons();
};
