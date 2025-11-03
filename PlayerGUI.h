/*
  ==============================================================================

    PlayerGUI.h
    Created: 21 Oct 2025 4:34:07pm
    Author:  USER

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

class PlayerAudio;

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ListBoxModel,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void setAudioBackend(PlayerAudio* backend); // bind backend
    void paint(juce::Graphics&) override;
    void resized() override;

    // listeners
    void buttonClicked(juce::Button* b) override;
    void sliderValueChanged(juce::Slider* s) override;
    void sliderDragStarted(juce::Slider* s) override;
    void sliderDragEnded(juce::Slider* s) override;

    // ListBoxModel
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;

    // Timer
    void timerCallback() override;

private:
    // Buttons
    juce::TextButton loadButton{ "Load" };
    juce::TextButton playlistButton{ "Load Playlist" };
    juce::TextButton playButton{ "Play >" };
    juce::TextButton pauseButton{ "Pause ||" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton startButton{ "|<" };
    juce::TextButton endButton{ ">|" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop OFF" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton abLoopButton{ "A–B Loop OFF" };

    // sliders & labels
    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Label currentTime{ "00:00" }, totalTime{ "00:00" }, infoLabel{ "info", "No file loaded" };
    juce::Label aLabel{ "A", "A: --:--" }, bLabel{ "B", "B: --:--" };

    juce::ListBox playlistBox{ "Playlist", this };
    juce::Array<juce::File> playlistFiles;
    std::unique_ptr<juce::FileChooser> fileChooser;

    PlayerAudio* audio = nullptr;
    bool draggingPosition = false;
    float lastVolume = 0.8f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
