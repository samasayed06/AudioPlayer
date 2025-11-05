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

    // --- Callbacks (optional) for Mix Mode; when set, they will be called in addition to local behavior
    std::function<void()> onPlayPressed;
    std::function<void()> onPausePressed;
    std::function<void()> onRestartPressed;
    std::function<void()> onStartPressed;
    std::function<void()> onEndPressed;
    std::function<void(float)> onVolumeChanged;
    std::function<void(double)> onSpeedChanged;

    std::function<void()> onMuteToggled;
    std::function<void()> onLoopToggled;
    std::function<void()> onABLoopToggled;
    std::function<void(double)> onSetPositionRequest; // seconds

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
    juce::TextButton jumpBackButton{ "-10s" };
    juce::TextButton jumpForwardButton{ "+10s" };


    // sliders & labels
    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Label currentTime{ "00:00" }, totalTime{ "00:00" }, infoLabel{ "info", "No file loaded" };
    juce::Label aLabel{ "A", "A: --:--" }, bLabel{ "B", "B: --:--" };
    juce::Slider speedSlider;
    // Waveform display
    juce::AudioBuffer<float> waveformBuffer;
    juce::Path waveformPath;
    int waveformYOffset = 150;  // positive = move down, negative = move up




    juce::ListBox playlistBox{ "Playlist", this };
    juce::Array<juce::File> playlistFiles;
    std::unique_ptr<juce::FileChooser> fileChooser;

    PlayerAudio* audio = nullptr;
    bool draggingPosition = false;
    float lastVolume = 0.8f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
