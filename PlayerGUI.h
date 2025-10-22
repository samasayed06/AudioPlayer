
#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component
{
public:
    PlayerGUI(PlayerAudio& player);
    ~PlayerGUI() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PlayerAudio& audioPlayer;

    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ ">" };
    juce::TextButton pauseButton{ "||" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton startButton{ "|<" };
    juce::TextButton endButton{ ">|" };
    juce::TextButton muteButton{ "Mute" }; // 🔹 زرار الميوت

    std::unique_ptr<juce::FileChooser> fileChooser;

    void setupButtons();
};

