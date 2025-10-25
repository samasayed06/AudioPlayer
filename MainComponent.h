#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    // Audio
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // GUI
    void paint(juce::Graphics& g) override;
    void resized() override;

    // Listeners
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void sliderDragStarted(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;

    // Timer
    void timerCallback() override;

private:
    // Audio
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::Label currentTimeLabel;
    juce::Label totalTimeLabel;

    // Buttons
    juce::TextButton loadButton{ "Load" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton playButton{ ">" };
    juce::TextButton pauseButton{ "||" };
    juce::TextButton startButton{ "|<" };
    juce::TextButton endButton{ ">|" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop OFF" };

    // Sliders
    juce::Slider volumeSlider;
    juce::Slider progressSlider; 

    bool isMuted = false;
    bool isLooping = false;
    bool isDraggingSlider = false;
    float previousGain = 0.5f;

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
